#include "spectralfiltering.h"
using namespace myCV;

spectralFiltering::spectralFiltering(cv::Mat &img)
{
    feedImage(img);
}

void spectralFiltering::feedImage(cv::Mat &img)
{
    originImg = img.clone();
    myCV::FFT2D(originImg, originSpectral.real, originSpectral.imag);
    moveSpectral2Center(originSpectral.real);
    moveSpectral2Center(originSpectral.imag);
    computeFFT();
}

void spectralFiltering::computeFFT()
{
    spectral.real = originSpectral.real.clone();
    spectral.imag = originSpectral.imag.clone();

    if(!filter.empty())
    {
        int j, i;
        for(j = 0; j < filter.rows; j++)
        {
            for(i = 0; i < filter.cols; i++)
            {
                spectral.real.at<float>(j,i) *= filter.at<float>(j,i);
                spectral.imag.at<float>(j,i) *= filter.at<float>(j,i);
            }
        }
    }
}

void spectralFiltering::getResult(cv::Mat &img)
{
    cv::Mat&& imageR = spectral.real.clone();
    cv::Mat&& imageI = spectral.imag.clone();
    moveSpectral2Center(imageR);
    moveSpectral2Center(imageI);
    iFFT2D(imageR, imageI, img, originImg.size().width, originImg.size().height);
}

void spectralFiltering::moveSpectral2Center(cv::Mat &img)
{
    cv::Mat temp = cv::Mat::zeros(img.cols, img.rows, CV_32FC1);
    const int move_x = img.cols / 2;
    const int move_y = img.rows / 2;

    int i = 0;
#pragma omp parallel for private(i)
    for(int j = 0 ; j < img.cols / 2; j++)
    {
        for(i = 0 ; i < img.rows / 2; i++)
        {
            temp.at<float>(j + move_y, i + move_y) = img.at<float>(j         ,i);
            temp.at<float>(j + move_y, i         ) = img.at<float>(j         ,i + move_x);
            temp.at<float>(j         , i + move_x) = img.at<float>(j + move_y,i);
            temp.at<float>(j         , i         ) = img.at<float>(j + move_y,i + move_x);
        }
    }
    img.release();
    img = temp.clone();
    temp.release();
}

void spectralFiltering::noFilter()
{
    filter.release();
}

void spectralFiltering::genLowPassFilter(int filter_algorithm, const int threshold)
{
    filter = cv::Mat::zeros(spectral.real.rows, spectral.real.cols, CV_32FC1);
    const int middle_x = spectral.real.rows / 2, middle_y = spectral.real.rows / 2;
    int j, i;
    if(filter_algorithm == FilterAlgorithm::ideal)
    {
        #pragma omp parallel for private(i)
        for(j = 0; j < spectral.real.rows; j++)
        {
            for(i = 0; i < spectral.real.cols; i++)
            {
                int&& d = sqrt(pow((double)(i-middle_x),2)+pow((double)(j-middle_y),2));
                if(d <= threshold)
                {
                    filter.at<float>(j,i)=1;
                }
            }
        }
    }
    else if(filter_algorithm == FilterAlgorithm::butterworth)
    {
        const int butterN = butterworth;
        #pragma omp parallel for private(i)
        for(j = 0; j < spectral.real.rows; j++)
        {
            for(i = 0; i < spectral.real.cols; i++)
            {
                int&& d = sqrt(pow((double)(i-middle_x),2)+pow((double)(j-middle_y),2));
                filter.at<float>(j,i)= 1.0 / (1.0 + pow(((double)d / (double)threshold),(2*butterN)));
            }
        }
    }
    else if(filter_algorithm == FilterAlgorithm::gaussian)
    {
        #pragma omp parallel for private(i)
        for(j = 0; j < spectral.real.rows; j++)
        {
            for(i = 0; i < spectral.real.cols; i++)
            {
                int&& d = sqrt(pow((double)(i-middle_x),2)+pow((double)(j-middle_y),2));
                filter.at<float>(j,i)= exp(-1*(d*d/(2*threshold*threshold)));
            }
        }
    }
}

void spectralFiltering::genHighPassFilter(int filter_algorithm, const int threshold)
{
    filter = cv::Mat::zeros(spectral.real.rows, spectral.real.cols, CV_32FC1);
    const int middle_x = spectral.real.rows / 2, middle_y = spectral.real.rows / 2;
    int j, i;
    if(filter_algorithm == FilterAlgorithm::ideal)
    {
        #pragma omp parallel for private(i)
        for(j = 0; j < spectral.real.rows; j++)
        {
            for(i = 0; i < spectral.real.cols; i++)
            {
                int&& d = sqrt(pow((double)(i-middle_x),2)+pow((double)(j-middle_y),2));
                if(d > threshold)
                {
                    filter.at<float>(j,i)=1;
                }
            }
        }
    }
    else if(filter_algorithm == FilterAlgorithm::butterworth)
    {
        const int butterN = butterworth;
        #pragma omp parallel for private(i)
        for(j = 0; j < spectral.real.rows; j++)
        {
            for(i = 0; i < spectral.real.cols; i++)
            {
                int&& d = sqrt(pow((double)(i-middle_x),2)+pow((double)(j-middle_y),2));
                filter.at<float>(j,i)= 1.0 / (1.0 + pow(((double)threshold / (double)d),(2*butterN)));
            }
        }
    }
    else if(filter_algorithm == FilterAlgorithm::gaussian)
    {
        #pragma omp parallel for private(i)
        for(j = 0; j < spectral.real.rows; j++)
        {
            for(i = 0; i < spectral.real.cols; i++)
            {
                int&& d = sqrt(pow((double)(i-middle_x),2)+pow((double)(j-middle_y),2));
                filter.at<float>(j,i)= 1- exp(-1*(d*d/(2*threshold*threshold)));
            }
        }
    }
}
