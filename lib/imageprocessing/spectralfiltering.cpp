#include "spectralfiltering.h"
using namespace myCV;

spectralFiltering::spectralFiltering(cv::Mat &img, bool isColor)
{
    homomorphic.high = 1.0;
    homomorphic.low  = 0.0;
    feedImage(img, isColor);
}

void spectralFiltering::feedImage(cv::Mat &img, bool isColor)
{
    originImg = img.clone();
    colorMode = isColor;
    initial(colorMode);
}

void spectralFiltering::initial(bool isColor)
{
    cv::Mat tmp;
    myCvtColor(originImg, tmp, BGR2GRAY);
    if(originImg.type() == CV_8UC3 && isColor)
    {
        myCvtColor(originImg, HSV, BGR2HSV);
        int j, i;
        #pragma omp parallel for private(i)
        for(j = 0; j < HSV.rows; j++)
        {
            for(i = 0; i < HSV.cols; i++)
            {
                tmp.at<uchar>(j,i) = HSV.at<cv::Vec3f>(j, i)[2];
            }
        }
    }

    myCV::FFT2D(tmp, originSpectral.real, originSpectral.imag);
    moveSpectral2Center(originSpectral.real);
    moveSpectral2Center(originSpectral.imag);
    computeFFT();
}

void spectralFiltering::changeColorMode(bool isColor)
{
    if(colorMode == isColor)
        return;

    colorMode = isColor;
    initial(colorMode);
}

void spectralFiltering::computeFFT()
{
    spectral.real = originSpectral.real.clone();
    spectral.imag = originSpectral.imag.clone();

    if(!filter.empty())
    {
        int j, i;
        //#pragma omp parallel for private(i)
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

    if(originImg.type()==CV_8UC3 && colorMode)
    {
        cv::Mat temp;
        iFFT2D(imageR, imageI, temp, originImg.size().width, originImg.size().height);
        int j, i;
        #pragma omp parallel for private(i)
        for(j = 0; j < temp.rows; j++)
        {
            for(i = 0; i < temp.cols; i++)
            {
                HSV.at<cv::Vec3f>(j,i)[2] = temp.at<uchar>(j,i);
            }
        }
        myCvtColor(HSV, img, HSV2BGR);
    }
    else
    {
        iFFT2D(imageR, imageI, img, originImg.size().width, originImg.size().height);
    }

}

void spectralFiltering::moveSpectral2Center(cv::Mat &img)
{
    cv::Mat temp = cv::Mat::zeros(img.rows, img.cols, CV_32FC1);
    const int move_x = img.cols / 2;
    const int move_y = img.rows / 2;

    int i = 0;
#pragma omp parallel for private(i)
    for(int j = 0 ; j < img.rows / 2; j++)
    {
        for(i = 0 ; i < img.cols / 2; i++)
        {
            temp.at<float>(j + move_y, i + move_x) = img.at<float>(j         ,i);
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
    filter.release();
    filter = cv::Mat::zeros(spectral.real.rows, spectral.real.cols, CV_32FC1);

    const int middle_x = spectral.real.cols / 2, middle_y = spectral.real.rows / 2;
    int j, i;
    const float homoDelta = homomorphic.high - homomorphic.low;

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
                    filter.at<float>(j,i)=homoDelta * 1.0 + homomorphic.low;
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
                filter.at<float>(j,i)= homoDelta *
                                       (1.0 / (1.0 + pow(((double)d / (double)threshold),(2*butterN))))
                                       + homomorphic.low;
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
                filter.at<float>(j,i)= homoDelta *
                                       exp(-1*(d*d/(2*threshold*threshold))) + homomorphic.low;
            }
        }
    }
}

void spectralFiltering::genHighPassFilter(int filter_algorithm, const int threshold)
{
    filter.release();
    filter = cv::Mat::zeros(spectral.real.rows, spectral.real.cols, CV_32FC1);
    const int middle_x = spectral.real.cols / 2, middle_y = spectral.real.rows / 2;
    int j, i;
    const float homoDelta = homomorphic.high - homomorphic.low;

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
                    filter.at<float>(j,i)=homoDelta * 1.0 + homomorphic.low;
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
                filter.at<float>(j,i)= homoDelta *
                                       (1.0 / (1.0 + pow(((double)threshold / (double)d),(2*butterN))))
                                       + homomorphic.low;
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
                filter.at<float>(j,i)= homoDelta *
                                       (1- exp(-1*(d*d/(2*threshold*threshold))))
                                       + homomorphic.low;
            }
        }
    }
}

void spectralFiltering::setHomomorphic(float high, float low)
{
    homomorphic.high = high;
    homomorphic.low  = low >= high ? 0 : low;
}
