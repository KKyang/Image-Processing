#include "imageprocessing.h"
#include <omp.h>
#include <iostream>
using namespace myCV;

void myCV::myCvtColor(cv::Mat &inputArray, cv::Mat &outputArray, int colorType, int grayType)
{

    int i=0;

    if(colorType == 1)
    {
        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC1);

        if(grayType == 2)
        {
            #pragma omp parallel for private(i)
            for(int j=0; j < inputArray.size().height;j++)
            {
                for(i = 0; i < inputArray.size().width;i++)
                {
                    tmp.at<uchar>(j,i) = (uchar)(inputArray.at<cv::Vec3b>(j,i)[0] * 0.114+
                            inputArray.at<cv::Vec3b>(j,i)[1] * 0.587+
                            inputArray.at<cv::Vec3b>(j,i)[2] * 0.299);
                }
            }
        }
        else if(grayType == 1)
        {
            #pragma omp parallel for private(i)
            for(int j=0; j < inputArray.size().height;j++)
            {
                for(i = 0; i < inputArray.size().width;i++)
                {
                    tmp.at<uchar>(j,i) = (uchar)((inputArray.at<cv::Vec3b>(j,i)[0]+
                            inputArray.at<cv::Vec3b>(j,i)[1] +
                            inputArray.at<cv::Vec3b>(j,i)[2])/3) ;
                }
            }
        }
        outputArray.release();
        outputArray = tmp.clone();
    }
    else if(colorType == 2)
    {
        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC3);
        #pragma omp parallel for private(i)
        for(int j=0; j < inputArray.size().height;j++)
        {
            for(i = 0; i < inputArray.size().width;i++)
            {
                tmp.at<cv::Vec3b>(j,i)[0] = tmp.at<cv::Vec3b>(j,i)[1] = tmp.at<cv::Vec3b>(j,i)[2] = inputArray.at<uchar>(j,i);
            }
        }
        outputArray.release();
        outputArray = tmp.clone();
    }

}

void myCV::histogram(cv::Mat &inputArray, cv::Mat &histogram)
{
    std::vector<std::vector<int>> data;
    int i=0;
    int largestCount = 0;

    if(inputArray.type() == CV_8UC3)
    {
        cv::Mat&& result = cv::Mat(100, 256, CV_8UC3, cv::Scalar(255,255,255));
        data.resize(3);
        data[0].resize(256, 0);
        data[1].resize(256, 0);
        data[2].resize(256, 0);
        #pragma omp parallel for private(i)
        for(int j=0; j < inputArray.size().height;j++)
        {
            for(i = 0; i < inputArray.size().width;i++)
            {
                data[0][inputArray.at<cv::Vec3b>(j,i)[0]]++;
                data[1][inputArray.at<cv::Vec3b>(j,i)[1]]++;
                data[2][inputArray.at<cv::Vec3b>(j,i)[2]]++;
            }
        }
        for (i = 0; i < 256; i++)
        {
            for(int k = 0; k < 3; k++)
            {
                if (data[k][i] > largestCount)
                {
                    largestCount = data[k][i];
#ifdef QT_DEBUG
                    std::cout << k << " " << i << std::endl;
#endif
                }
            }
        }
#ifdef QT_DEBUG
        std::cout << largestCount << std::endl;
#endif
        for (i = 0; i < 256; i ++)
        {
            for(int k = 0; k < 3 ; k++)
            {
                for (int j = 0; j < 100-(int)(((double)data[k][i] / (double)largestCount) * 100); j++)
                {
                    result.at<cv::Vec3b>(j,i)[k] = 0;
                }
            }
        }
        histogram.release();
        histogram = result.clone();
    }
    else if(inputArray.type() == CV_8UC1)
    {
        cv::Mat&& result = cv::Mat(100, 256, CV_8UC3, cv::Scalar(255,255,255));
        data.resize(1);
        data[0].resize(256, 0);
        #pragma omp parallel for private(i)
        for(int j=0; j < inputArray.size().height;j++)
        {
            for(i = 0; i < inputArray.size().width;i++)
            {
                data[0][inputArray.at<uchar>(j,i)]++;
            }
        }

        for (i = 0; i < 256; i++)
        {
            if (data[0][i] > largestCount)
            {
                largestCount = data[0][i];
            }
        }

        for (i = 0; i < 256; i ++)
        {
            for (int j = 0; j < 100-(int)(((double)data[0][i] / (double)largestCount) * 100); j++)
            {
                    result.at<uchar>(j,i) = 0;
            }
        }
        myCvtColor(result, result, GRAY2GBR);
        histogram.release();
        histogram = result.clone();
    }


}

imageProcessing::imageProcessing()
{
}


