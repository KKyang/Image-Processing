#include "spectralfiltering.h"
using namespace myCV;

spectralFiltering::spectralFiltering(cv::Mat &img)
{
    originImg = img.clone();
    myCV::FFT2D(originImg, originSpectralReal, originSpectralImag);
    spectralReal = originSpectralReal.clone();
    spectralImag = originSpectralImag.clone();
    moveSpectral2Center(spectralReal);
    moveSpectral2Center(spectralImag);
}

void spectralFiltering::feedImage(cv::Mat &img)
{
    originImg = img.clone();
    myCV::FFT2D(originImg, originSpectralReal, originSpectralImag);
    spectralReal = originSpectralReal.clone();
    spectralImag = originSpectralImag.clone();
    moveSpectral2Center(spectralReal);
    moveSpectral2Center(spectralImag);
}

void spectralFiltering::getResult(cv::Mat &img)
{
    //iFFT2D(spectralReal, spectralImag, img);
}

void spectralFiltering::moveSpectral2Center(cv::Mat &img)
{
    cv::Mat temp = cv::Mat::zeros(img.cols*2, img.rows*2, CV_32FC1);
    for(int j = 0 ; j < img.cols; j++)
    {
        for(int i = 0 ; i < img.rows; i++)
        {
            temp.at<float>(j,i) = img.at<float>(j,i);
            temp.at<float>(j+img.cols,i) = img.at<float>(j,i);
            temp.at<float>(j,i+img.rows) = img.at<float>(j,i);
            temp.at<float>(j+img.cols,i+img.rows) = img.at<float>(j,i);
        }
    }
    img.release();
    img = temp.clone();
    temp.release();
}
