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
        tmp.release();
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
        tmp.release();
    }

}

void myCV::myThreshold(cv::Mat &inputArray, cv::Mat &outputArray, unsigned int _threshold, unsigned _min, unsigned _max)
{
    cv::Mat tmp;
    cv::Mat&& dest = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC1);
    if(_min < 0)
    {
        _min = 0;
    }
    if(_max > 255)
    {
        _max = 255;
    }
    if(_threshold < 0 || _threshold > 255)
    {
        _threshold = 128;
    }
    if(inputArray.type() == CV_8UC3)
    {
        myCvtColor(inputArray, tmp, BGR2GRAY);
    }
    else
    {
        tmp = inputArray.clone();
    }

    int i = 0;
    #pragma omp parallel for private(i)
    for(int j=0; j < inputArray.size().height;j++)
    {
        for(i = 0; i < inputArray.size().width;i++)
        {
            if(tmp.at<uchar>(j,i) >= _threshold)
            {
                dest.at<uchar>(j,i) = _max;
            }
            else
            {
                dest.at<uchar>(j,i) = _min;
            }
        }
    }

    outputArray.release();
    outputArray = dest.clone();
    tmp.release();
    dest.release();
}

void myCV::histogramD(cv::Mat &inputArray, cv::Mat &hist, std::vector<std::vector<int>> &data)
{
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
                }
            }
        }

        int j = 0; int k = 0;
        #pragma omp parallel for private(j, k)
        for (i = 0; i < 256; i ++)
        {
            for(k = 0; k < 3 ; k++)
            {
                for (j = 0; j < 100-(int)(((double)data[k][i] / (double)largestCount) * 100); j++)
                {
                    result.at<cv::Vec3b>(j,i)[k] = 0;
                }
            }
        }
        hist.release();
        hist = result.clone();
        result.release();
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

        int j = 0;
        #pragma omp parallel for private(j)
        for (i = 0; i < 256; i ++)
        {
            for (j = 0; j < 100-(int)(((double)data[0][i] / (double)largestCount) * 100); j++)
            {
                    result.at<uchar>(j,i) = 0;
            }
        }
        myCvtColor(result, result, GRAY2GBR);
        hist.release();
        hist = result.clone();
        result.release();
    }
}

void myCV::histogram(cv::Mat &inputArray, cv::Mat &hist)
{
    std::vector<std::vector<int>> data;
    histogramD(inputArray, hist, data);
}

void myCV::EqualizeHist(cv::Mat &inputArray, cv::Mat &outputArray)
{
    std::vector<std::vector<int>> data;
    std::vector<int> Transform(256, 0);

    int sum = 0;
    int sig = 0;

    if(inputArray.type() == CV_8UC3)
    {
        cv::Mat&& YCrCb = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC3);
        cv::Mat&& Y = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC1);

        int i = 0;

        //RGB to YCbCr (YUV)
        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width; i++)
            {
                Y.at<uchar>(j,i) =            inputArray.at<cv::Vec3b>(j,i)[0] * 0.114 +
                                              inputArray.at<cv::Vec3b>(j,i)[1] * 0.587 +
                                              inputArray.at<cv::Vec3b>(j,i)[2] * 0.299;
                YCrCb.at<cv::Vec3b>(j,i)[1] = inputArray.at<cv::Vec3b>(j,i)[0] * 0.5 -
                                              inputArray.at<cv::Vec3b>(j,i)[1] * 0.331264 -
                                              inputArray.at<cv::Vec3b>(j,i)[2] * 0.168736 + 128;
                YCrCb.at<cv::Vec3b>(j,i)[2] = -(inputArray.at<cv::Vec3b>(j,i)[0] * 0.081312) -
                                              inputArray.at<cv::Vec3b>(j,i)[1] * 0.418688 +
                                              inputArray.at<cv::Vec3b>(j,i)[2] * 0.5 + 128;
            }

        histogramD(Y, cv::Mat(), data);

        for(int i = 0; i < 256; i++)
        {
            sum += data[0][i];
        }

        for(int i = 0; i < 256; i++)
        {
            sig += data[0][i];
            Transform[i] = cvRound(255 * (double)sig/(double)sum);
        }

        cv::Mat&& dest = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC3);

        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width;i++)
            {
                YCrCb.at<cv::Vec3b>(j,i)[0] =  Transform[Y.at<uchar>(j,i)];


                if((YCrCb.at<cv::Vec3b>(j,i)[0] +
                    1.402   * (YCrCb.at<cv::Vec3b>(j,i)[2] - 128)) > 255)
                    dest.at<cv::Vec3b>(j,i)[2] = 255;
                else if((YCrCb.at<cv::Vec3b>(j,i)[0] +
                         1.402   * (YCrCb.at<cv::Vec3b>(j,i)[2] - 128)) < 0)
                         dest.at<cv::Vec3b>(j,i)[2] = 0;
                else
                    dest.at<cv::Vec3b>(j,i)[2] = YCrCb.at<cv::Vec3b>(j,i)[0] +
                    1.402   * (YCrCb.at<cv::Vec3b>(j,i)[2] - 128);

                if((YCrCb.at<cv::Vec3b>(j,i)[0] -
                    0.34414 * (YCrCb.at<cv::Vec3b>(j,i)[1] - 128) -
                    0.71414 * (YCrCb.at<cv::Vec3b>(j,i)[2] - 128)) > 255)
                    dest.at<cv::Vec3b>(j,i)[1] = 255;
                else if((YCrCb.at<cv::Vec3b>(j,i)[0] -
                         0.34414 * (YCrCb.at<cv::Vec3b>(j,i)[1] - 128) -
                         0.71414 * (YCrCb.at<cv::Vec3b>(j,i)[2] - 128)) < 0)
                         dest.at<cv::Vec3b>(j,i)[1] = 0;
                else
                    dest.at<cv::Vec3b>(j,i)[1] = YCrCb.at<cv::Vec3b>(j,i)[0] -
                    0.34414 * (YCrCb.at<cv::Vec3b>(j,i)[1] - 128) -
                    0.71414 * (YCrCb.at<cv::Vec3b>(j,i)[2] - 128);

                if((YCrCb.at<cv::Vec3b>(j,i)[0] +
                    1.772   * (YCrCb.at<cv::Vec3b>(j,i)[1] - 128)) > 255)
                    dest.at<cv::Vec3b>(j,i)[0] = 255;
                else if((YCrCb.at<cv::Vec3b>(j,i)[0] +
                         1.772   * (YCrCb.at<cv::Vec3b>(j,i)[1] - 128)) < 0)
                         dest.at<cv::Vec3b>(j,i)[0] = 0;
                else
                    dest.at<cv::Vec3b>(j,i)[0] = YCrCb.at<cv::Vec3b>(j,i)[0] +
                    1.772   * (YCrCb.at<cv::Vec3b>(j,i)[1] - 128);

            }

        outputArray.release();
        outputArray = dest.clone();
        YCrCb.release();
        dest.release();
    }
    else if(inputArray.type() == CV_8UC1)
    {
        histogramD(inputArray, cv::Mat(), data);

        for(int i = 0; i < 256; i++)
        {
            sum += data[0][i];
        }

        for(int i = 0; i < 256; i++)
        {
            sig += data[0][i];
            Transform[i] = cvRound(255 * (double)sig/(double)sum);
        }

        cv::Mat&& dest = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC1);
        int i = 0;
        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width;i++)
            {
                dest.at<uchar>(j,i) =  Transform[inputArray.at<uchar>(j,i)];
            }
        outputArray.release();
        outputArray = dest.clone();
        dest.release();
    }
}


imageProcessing::imageProcessing()
{
}


