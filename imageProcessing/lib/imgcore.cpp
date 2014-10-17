#include "imgcore.h"
using namespace myCV;

void myCV::myResize(cv::Mat &inputArray, cv::Mat &outputArray, const unsigned int _width, unsigned int _height, const bool _aspectRatioFix)
{
    double ratioW = (double)_width / (double)inputArray.size().width;
    double ratioH = 0;

    if (_aspectRatioFix)
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
    #pragma omp parallel for private(i)
    for (int j = 0; j < _height; j++)
    {
        for (i = 0; i < _width; i++)
        {
            if(inputArray.type()==CV_8UC3)
            {
                tmp.at<cv::Vec3b>(j,i)[0] = inputArray.at<cv::Vec3b>((int)(j / ratioH), (int)(i / ratioW))[0];
                tmp.at<cv::Vec3b>(j,i)[1] = inputArray.at<cv::Vec3b>((int)(j / ratioH), (int)(i / ratioW))[1];
                tmp.at<cv::Vec3b>(j,i)[2] = inputArray.at<cv::Vec3b>((int)(j / ratioH), (int)(i / ratioW))[2];
            }
            else if(inputArray.type()==CV_8UC1)
            {
                tmp.at<uchar>(j,i) = inputArray.at<uchar>((int)(j / ratioH), (int)(i / ratioW));
            }
        }
    }

    outputArray.release();
    outputArray = tmp.clone();
}

imgCore::imgCore()
{
}
