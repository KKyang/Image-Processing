#include "fft.h"
using namespace myCV;

void myCV::FFT2D(cv::Mat &inputArray, cv::Mat &outputArray_real, cv::Mat &outputArray_imag)
{
    cv::Mat temp;
    if(inputArray.type()==CV_8UC3)
    {
        myCvtColor(inputArray, temp, BGR2GRAY);
    }
    else
        temp = inputArray.clone();

    //Converting array to complex format
    int powerWidth = 1, powerHeight = 1, power_of_w = 0, power_of_h = 0;
    while(powerWidth  < temp.size().width ){powerWidth  *= 2; power_of_w++;}
    while(powerHeight < temp.size().height){powerHeight *= 2; power_of_h++;}
    cv::Mat&& complex = cv::Mat::zeros(powerHeight, powerWidth, CV_32FC2);

    int padded_x = (powerWidth-temp.size().width)/2;
    int padded_y = (powerHeight-temp.size().height)/2;
    for(int j=0;j<temp.size().height;j++)
        for(int i=0;i<temp.size().width;i++)
        {
            complex.at<cv::Vec2f>(j + padded_y,i + padded_x)[0] = (float)temp.at<uchar>(j,i);
        }
    temp.release();

    int i = 0, j = 0;
    std::vector<double> real, imag;
    real.resize(powerWidth);
    imag.resize(powerWidth);
    for(j=0; j < powerHeight;j++)
    {
        for(i=0;i<powerWidth;i++)
        {
            real[i] = complex.at<cv::Vec2f>(j,i)[0];
            imag[i] = complex.at<cv::Vec2f>(j,i)[1];
        }
        FFT(real, imag, power_of_w);
        for(i=0;i<powerWidth;i++)
        {
            complex.at<cv::Vec2f>(j,i)[0] = real[i];
            complex.at<cv::Vec2f>(j,i)[1] = imag[i];
        }
    }

    real.resize(powerHeight);
    imag.resize(powerHeight);
    for(i=0; i < powerWidth;i++)
    {
        for(j=0;j<powerHeight;j++)
        {
            real[j] = complex.at<cv::Vec2f>(j,i)[0];
            imag[j] = complex.at<cv::Vec2f>(j,i)[1];
        }
        FFT(real, imag, power_of_h);
        for(j=0;j<powerHeight;j++)
        {
            complex.at<cv::Vec2f>(j,i)[0] = real[j];
            complex.at<cv::Vec2f>(j,i)[1] = imag[j];
        }
    }

    outputArray_real.release();
    outputArray_imag.release();
    outputArray_real = cv::Mat(powerHeight, powerWidth, CV_32FC1).clone();
    outputArray_imag = cv::Mat(powerHeight, powerWidth, CV_32FC1).clone();
    for(int j=0; j < powerHeight;j++)
    {
        for(int i=0; i < powerWidth;i++)
        {
            outputArray_real.at<float>(j,i) = complex.at<cv::Vec2f>(j,i)[0];
            outputArray_imag.at<float>(j,i) = complex.at<cv::Vec2f>(j,i)[1];
        }
    }
    complex.release();
}

void myCV::iFFT2D(cv::Mat &inputArray_real, cv::Mat &inputArray_imag, cv::Mat &outputArray, const int width, const int height)
{
    int power_of_w = 0, power_of_h = 0;
    int mod = inputArray_real.size().width;
    while(mod != 1)
    {
        mod = mod / 2;
        power_of_w++;
    }
    mod = inputArray_real.size().height;
    while(mod != 1)
    {
        mod = mod / 2;
        power_of_h++;
    }
    int powerWidth  = inputArray_real.size().width;
    int powerHeight = inputArray_real.size().height;
    cv::Mat&& complex = cv::Mat::zeros(powerHeight, powerWidth, CV_32FC2);
    int i = 0, j = 0;
    for(j=0; j < powerHeight;j++)
    {
        for(i=0; i < powerWidth;i++)
        {
            complex.at<cv::Vec2f>(j,i)[0] = inputArray_real.at<float>(j,i);
            complex.at<cv::Vec2f>(j,i)[1] = inputArray_imag.at<float>(j,i);
        }
    }


    std::vector<float> real, imag;
    real.resize(powerWidth);
    imag.resize(powerWidth);
    for(j=0; j < powerHeight;j++)
    {
        for(i=0;i<powerWidth;i++)
        {
            real[i] = complex.at<cv::Vec2f>(j,i)[0];
            imag[i] = complex.at<cv::Vec2f>(j,i)[1];
        }
        FFT(real, imag, power_of_w, true);
        for(i=0;i<powerWidth;i++)
        {
            complex.at<cv::Vec2f>(j,i)[0] = real[i];
            complex.at<cv::Vec2f>(j,i)[1] = imag[i];
        }
    }

    real.resize(powerHeight);
    imag.resize(powerHeight);
    for(i=0; i < powerWidth;i++)
    {
        for(j=0;j<powerHeight;j++)
        {
            real[j] = complex.at<cv::Vec2f>(j,i)[0];
            imag[j] = complex.at<cv::Vec2f>(j,i)[1];
        }
        FFT(real, imag, power_of_h, true);
        for(j=0;j<powerHeight;j++)
        {
            complex.at<cv::Vec2f>(j,i)[0] = real[j];
            complex.at<cv::Vec2f>(j,i)[1] = imag[j];
        }
    }

    cv::Mat&& dst = cv::Mat::zeros(powerHeight, powerWidth, CV_8UC1);
    for(j=0;j<powerHeight;j++)
        for(i=0;i<powerWidth;i++)
        {
            dst.at<uchar>(j,i) = (uchar)complex.at<cv::Vec2f>(j,i)[0];
        }

    outputArray.release();
    outputArray = cv::Mat(height, width, CV_8UC1);
    int padded_x = (dst.size().width - width)/2;
    int padded_y = (dst.size().height - height)/2;
    for(int j=0;j<height;j++)
        for(int i=0;i<width;i++)
        {
            outputArray.at<uchar>(j, i) = dst.at<uchar>(j + padded_y, i + padded_x);
        }
    dst.release();
}

