#include "spectralfiltering.h"
using namespace myCV;

spectralFiltering::spectralFiltering(cv::Mat &img, bool isColor)
{
    feedImage(img, isColor, GHPF);
}

spectralFiltering::spectralFiltering(cv::Mat &img, bool isColor, bool isHomomorphic)
{
    feedImage(img, isColor, isHomomorphic);
}

void spectralFiltering::feedImage(cv::Mat &img, bool isColor, bool isHomomorphic)
{
    originImg = img.clone();

    colorMode = isColor;
    GHPF = isHomomorphic;

    initial(colorMode);
}

void spectralFiltering::initial(bool isColor)
{
    std::vector<cv::Mat> tmp;

    if(originImg.type() == CV_8UC3 && isColor)
    {
        channel = 3;
        tmp.resize(channel);
        if(!GHPF)
        {
            cv::split(originImg, tmp);
        }
        else
        {
            tmp[0] = cv::Mat(originImg.rows, originImg.cols, CV_32FC1).clone();
            tmp[1] = cv::Mat(originImg.rows, originImg.cols, CV_32FC1).clone();
            tmp[2] = cv::Mat(originImg.rows, originImg.cols, CV_32FC1).clone();
            int j, i, k;
            #pragma omp parallel for private(i, k)
            for(j = 0; j < tmp[0].rows; j++)
                for(i = 0; i < tmp[0].cols; i++)
                {
                    for(k = 0; k < channel; k++)
                        tmp[k].at<float>(j, i)= log(1.0 + (float)originImg.at<cv::Vec3b>(j,i)[k]);
                }
        }
    }
    else
    {
        channel = 1;
        tmp.resize(channel);
        if(!GHPF)
        {
            if(originImg.type()==CV_8UC3)
                myCvtColor(originImg, tmp[0], BGR2GRAY);
            else
                tmp[0] = originImg.clone();
        }
        else
        {
            cv::Mat dst;
            if(originImg.type()==CV_8UC3)
                myCvtColor(originImg, dst, BGR2GRAY);
            else
                dst = originImg.clone();

            tmp[0] = cv::Mat(originImg.rows, originImg.cols, CV_32FC1).clone();
            int j, i;
            #pragma omp parallel for private(i)
            for(j = 0; j < tmp[0].rows; j++)
            {
                for(i = 0; i < tmp[0].cols; i++)
                {
                     tmp[0].at<float>(j, i)= log(1.0 + (float)dst.at<uchar>(j,i));
                }
            }
        }
    }
    spectral.resize(channel);
    originSpectral.resize(channel);

    #pragma omp parallel for
    for(int k = 0; k < channel; k++)
    {
        myCV::FFT2D(tmp[k], originSpectral[k].real, originSpectral[k].imag);
        moveSpectral2Center(originSpectral[k].real);
        moveSpectral2Center(originSpectral[k].imag);
    }
    computeFFT();
/*    if(originImg.type() == CV_8UC3 && isColor)
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
*/
}

void spectralFiltering::setHomomorphic(float high, float low, float c)
{
    homomorphic.high = high;
    homomorphic.low  = low >= high ? 0 : low;
    homomorphic.c = c;
}

void spectralFiltering::changeGHPFMode(bool status)
{
    if(GHPF == status)
        return;

    GHPF = status;
    if(!GHPF)
    {
        homomorphic.high = 1.0;
        homomorphic.low = 0.0;
        homomorphic.c = 1.0;
    }

    switch(filter_pro.type)
    {
    case 0:
        noFilter();
        break;
    case 1:
        genLowPassFilter(FilterAlgorithm::gaussian, filter_pro.threshold);
        break;
    case 2:
        genHighPassFilter(FilterAlgorithm::gaussian, filter_pro.threshold);
        break;
    }

    initial(colorMode);
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
    int j, i;
    #pragma omp parallel for private(i, j)
    for(int k = 0;k < channel; k++)
    {
        spectral[k].real = originSpectral[k].real.clone();
        spectral[k].imag = originSpectral[k].imag.clone();

        if(!filter.empty())
        {
            for(j = 0; j < filter.rows; j++)
            {
                for(i = 0; i < filter.cols; i++)
                {
                    spectral[k].real.at<float>(j,i) *= filter.at<float>(j,i);
                    spectral[k].imag.at<float>(j,i) *= filter.at<float>(j,i);
                }
            }
        }
    }
}

cv::Mat spectralFiltering::getSpectralIntensity()
{
    cv::Mat tmp(spectral[0].real.rows, spectral[0].real.cols, CV_32FC1);
    int j, i;
    #pragma omp parallel for private(i)
    for(j = 0; j < spectral[0].real.rows; j++)
    {
        for(i = 0; i < spectral[0].real.cols; i++)
        {
            float real = spectral[0].real.at<float>(j, i);
            float imag = spectral[0].imag.at<float>(j, i);
            tmp.at<float>(j,i) = log(1 + sqrt((real * real) + (imag * imag)));
        }
    }
    myCV::normalize(tmp, tmp, 0 , 255, normType::MINMAX);
    cv::Mat dst(spectral[0].real.rows, spectral[0].real.cols, CV_8UC1);
    #pragma omp parallel for private(i)
    for(j = 0; j < spectral[0].real.rows; j++)
    {
        for(i = 0; i < spectral[0].real.cols; i++)
        {
            dst.at<uchar>(j,i) = (uchar)(tmp.at<float>(j,i));
        }
    }

    return dst;
    tmp.release();
    dst.release();
}

void spectralFiltering::getResult(cv::Mat &img)
{
    if(originImg.empty())
        return;
    std::vector<cv::Mat> imageR(channel), imageI(channel);
    #pragma omp parallel for
    for(int k = 0;k < channel; k++)
    {
        imageR[k] = spectral[k].real.clone();
        imageI[k] = spectral[k].imag.clone();
        moveSpectral2Center(imageR[k]);
        moveSpectral2Center(imageI[k]);
    }

    if(originImg.type()==CV_8UC3 && colorMode)
    {
        std::vector<cv::Mat> temp(channel);
        #pragma omp parallel for
        for(int k = 0;k < channel; k++)
        {
            if(GHPF)
                iFFT2DHomo(imageR[k], imageI[k], temp[k], originImg.size().width, originImg.size().height);
            else
                iFFT2D(imageR[k], imageI[k], temp[k], originImg.size().width, originImg.size().height);
        }
        img.release();
        cv::merge(temp, img);
    }
    else
    {
        if(GHPF)
            iFFT2DHomo(imageR[0], imageI[0], img, originImg.size().width, originImg.size().height);
        else
            iFFT2D(imageR[0], imageI[0], img, originImg.size().width, originImg.size().height);
    }
    /*
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
    }*/

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
    filter_pro.type = 0;
}

void spectralFiltering::genLowPassFilter(int filter_algorithm, const int threshold)
{
    filter.release();
    filter = cv::Mat::zeros(spectral[0].real.rows, spectral[0].real.cols, CV_32FC1);
    filter_pro.threshold = threshold;
    filter_pro.type = 1;

    const int middle_x = filter.cols / 2, middle_y = filter.rows / 2;
    int j, i;
    const float homoDelta = homomorphic.high - homomorphic.low;

    if(filter_algorithm == FilterAlgorithm::ideal && !GHPF)
    {
        #pragma omp parallel for private(i)
        for(j = 0; j < filter.rows; j++)
        {
            for(i = 0; i < filter.cols; i++)
            {
                double&& d = sqrt(pow((double)(i-middle_x),2)+pow((double)(j-middle_y),2));
                if(d <= threshold)
                {
                    filter.at<float>(j,i)=1.0;
                }
            }
        }
    }
    else if(filter_algorithm == FilterAlgorithm::butterworth && !GHPF)
    {
        const int butterN = butterworth;
        #pragma omp parallel for private(i)
        for(j = 0; j < filter.rows; j++)
        {
            for(i = 0; i < filter.cols; i++)
            {
                double&& d = sqrt(pow((double)(i-middle_x),2)+pow((double)(j-middle_y),2));
                filter.at<float>(j,i)= (1.0 / (1.0 + pow(((double)d / (double)threshold),(2*butterN))));
            }
        }
    }
    else if(filter_algorithm == FilterAlgorithm::gaussian || GHPF)
    {
        #pragma omp parallel for private(i)
        for(j = 0; j < filter.rows; j++)
        {
            for(i = 0; i < filter.cols; i++)
            {
                double&& d = sqrt(pow((double)(i-middle_x),2)+pow((double)(j-middle_y),2));
                filter.at<float>(j,i)= homoDelta *
                                       exp(-homomorphic.c*(d*d/(double)(2*threshold*threshold))) + homomorphic.low;
            }
        }
    }
}

void spectralFiltering::genHighPassFilter(int filter_algorithm, const int threshold)
{
    filter.release();
    filter = cv::Mat::zeros(spectral[0].real.rows, spectral[0].real.cols, CV_32FC1);
    filter_pro.threshold = threshold;
    filter_pro.type = 2;

    const int middle_x = filter.cols / 2, middle_y = filter.rows / 2;
    int j, i;
    const float homoDelta = homomorphic.high - homomorphic.low;

    if(filter_algorithm == FilterAlgorithm::ideal && !GHPF)
    {
        #pragma omp parallel for private(i)
        for(j = 0; j < filter.rows; j++)
        {
            for(i = 0; i < filter.cols; i++)
            {
                double&& d = sqrt(pow((double)(i-middle_x),2)+pow((double)(j-middle_y),2));
                if(d > threshold)
                {
                    filter.at<float>(j,i)=1.0;
                }
            }
        }
    }
    else if(filter_algorithm == FilterAlgorithm::butterworth && !GHPF)
    {
        const int butterN = butterworth;
        #pragma omp parallel for private(i)
        for(j = 0; j < filter.rows; j++)
        {
            for(i = 0; i < filter.cols; i++)
            {
                double&& d = sqrt(pow((double)(i-middle_x),2)+pow((double)(j-middle_y),2));
                filter.at<float>(j,i)= (1.0 / (1.0 + pow(((double)threshold / (double)d),(2*butterN))));
            }
        }
    }
    else if(filter_algorithm == FilterAlgorithm::gaussian || GHPF)
    {
        #pragma omp parallel for private(i)
        for(j = 0; j < filter.rows; j++)
        {
            for(i = 0; i < filter.cols; i++)
            {
                double&& d = sqrt(pow((double)(i-middle_x),2)+pow((double)(j-middle_y),2));
                filter.at<float>(j,i)= homoDelta *
                                       (1- exp(-homomorphic.c*(d*d/(double)(2*threshold*threshold))))
                                       + homomorphic.low;
            }
        }
    }
}
