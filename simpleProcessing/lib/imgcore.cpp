#include "imgcore.h"
using namespace myCV;

void myCV::myResize(cv::Mat &inputArray, cv::Mat &outputArray, const unsigned int _width, unsigned int _height, const bool _aspectRatioFix, const int method)
{
    double ratioW = (double)_width / (double)inputArray.size().width;
    double ratioH = 0;

    if (_aspectRatioFix) //See if needs to keep aspect ratio.
    {
        ratioH = ratioW;
        _height = (unsigned int)(ratioH * (double)inputArray.size().height);
    }
    else
    {
        ratioH = (double)_height / (double)inputArray.size().height;
    }

    cv::Mat&& tmp = cv::Mat::zeros(_height, _width, inputArray.type());
    int i;
    if(method == 1) //Simple resizing method. Just simply copy pixels to new positions.
    {
        #pragma omp parallel for private(i)
        for (int j = 0; j < _height; j++)
        {
            for (i = 0; i < _width; i++)
            {
                if(inputArray.type()==CV_8UC3)
                {
                    tmp.at<cv::Vec3b>(j,i)[0] = inputArray.at<cv::Vec3b>(cvRound(j / ratioH), cvRound(i / ratioW))[0];
                    tmp.at<cv::Vec3b>(j,i)[1] = inputArray.at<cv::Vec3b>(cvRound(j / ratioH), cvRound(i / ratioW))[1];
                    tmp.at<cv::Vec3b>(j,i)[2] = inputArray.at<cv::Vec3b>(cvRound(j / ratioH), cvRound(i / ratioW))[2];
                }
                else if(inputArray.type()==CV_8UC1)
                {
                    tmp.at<uchar>(j,i) = inputArray.at<uchar>(cvRound(j / ratioH), cvRound(i / ratioW));
                }
            }
        }
    }
    else if(method == 2) //Bilinear resize method.
    {
        int k;
        #pragma omp parallel for private(i,k)
        for (int j = 0; j < _height; j++)
        {
            for (i = 0; i < _width; i++)
            {
                if(inputArray.type()==CV_8UC3)
                {
                    int&& ceilValW = std::ceil(i/ratioW) > inputArray.size().width-1  ? inputArray.size().width-1  : std::ceil(i/ratioW);
                    int&& ceilValH = std::ceil(j/ratioH) > inputArray.size().height-1 ? inputArray.size().height-1 : std::ceil(j/ratioH);
                    double&& alpha = std::ceil(i/ratioW) > inputArray.size().width-1  ? 0 : (double)((double)(i/ratioW) - std::floor(i/ratioW));
                    double&& beta  = std::ceil(j/ratioH) > inputArray.size().height-1 ? 0 : (double)((double)(j/ratioH) - std::floor(j/ratioH));
                    for(k = 0; k < 3; k++)
                    {
                        tmp.at<cv::Vec3b>(j,i)[k] = inputArray.at<cv::Vec3b>(std::floor(j/ratioH), std::floor(i/ratioW))[k]*(1-alpha)*(1-beta) +
                                                    inputArray.at<cv::Vec3b>(std::floor(j/ratioH), ceilValW)[k] *(alpha)  *(1-beta) +
                                                    inputArray.at<cv::Vec3b>(ceilValH,  std::floor(i/ratioW))[k]*(1-alpha)*(beta)   +
                                                    inputArray.at<cv::Vec3b>(ceilValH,  ceilValW)[k] *(alpha)  *(beta);
                    }
                }
                else if(inputArray.type()==CV_8UC1)
                {
                    int&& ceilValW = std::ceil(i/ratioW) > inputArray.size().width-1  ? inputArray.size().width-1  : std::ceil(i/ratioW);
                    int&& ceilValH = std::ceil(j/ratioH) > inputArray.size().height-1 ? inputArray.size().height-1 : std::ceil(j/ratioH);
                    double&& alpha = std::ceil(i/ratioW) > inputArray.size().width-1  ? 0 : (double)((double)(i/ratioW) - std::floor(i/ratioW));
                    double&& beta  = std::ceil(j/ratioH) > inputArray.size().height-1 ? 0 : (double)((double)(j/ratioH) - std::floor(j/ratioH));
                    tmp.at<uchar>(j,i) = inputArray.at<uchar>(std::floor(j/ratioH), std::floor(i/ratioW))*(1-alpha)*(1-beta) +
                                         inputArray.at<uchar>(std::floor(j/ratioH), ceilValW) *(alpha)  *(1-beta) +
                                         inputArray.at<uchar>(ceilValH,  std::floor(i/ratioW))*(1-alpha)*(beta)   +
                                         inputArray.at<uchar>(ceilValH,  ceilValW) *(alpha)  *(beta);
                }
            }
        }
    }


    outputArray.release();
    outputArray = tmp.clone();
}

imgCore::imgCore()
{
}
