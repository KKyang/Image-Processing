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

void myCV::normalize(cv::Mat &inputArray, cv::Mat &outputArray, int min, int max, int type)
{
    //still broken
    if(inputArray.type() == CV_32FC3 && type == normType::MINMAX)
    {
        cv::Mat dst(inputArray.rows, inputArray.cols, inputArray.type());
        int channels = inputArray.channels();
        float little, big;
        int j, i;

        findMinMax(inputArray, little, big);


        #pragma omp parallel for private(i)
        for (j = 0; j < inputArray.rows; j++)
        {
            for (i = 0; i < inputArray.cols * channels; i++)
            {
                dst.ptr<float>(j)[i] = ((inputArray.ptr<float>(j)[i] - little) / (big - little)) * (float)(max - min) + min;
            }
        }
        outputArray.release();
        outputArray = dst.clone();
        dst.release();
        return;
    }
    else if(inputArray.type() == CV_32FC1 && type == normType::MINMAX)
    {
        cv::Mat dst(inputArray.rows, inputArray.cols, inputArray.type());
        int channels = inputArray.channels();
        float little, big;
        int j, i;

        findMinMax(inputArray, little, big);


        #pragma omp parallel for private(i)
        for (j = 0; j < inputArray.rows; j++)
        {
            for (i = 0; i < inputArray.cols * channels; i++)
            {
                dst.ptr<float>(j)[i] = ((inputArray.ptr<float>(j)[i] - little) / (big - little)) * (float)(max - min) + min;
            }
        }
        outputArray.release();
        outputArray = dst.clone();
        dst.release();
        return;
    }
}

//Still broken
void myCV::myLine(cv::Mat &inputArray, cv::Point &point1, cv::Point &point2, cv::Scalar color, int thickness, int method)
{
    cv::Mat dst;
    if(inputArray.type()== CV_8UC1)
    {
        myCvtColor(inputArray, dst, GRAY2GBR);
    }
    else{dst = inputArray.clone();}

    if(point1.x > point2.x){
        cv::Point tmp;
        tmp = point2;
        point2 = point1;
        point1 = tmp;
    }
    if(thickness <= 0){thickness = 1;}
    thickness = thickness % 2 == 0 ? (thickness) / 2 : (thickness - 1) / 2;

    //Bresenham alogorithm
    if(method == line::BRESENHAM)
    {
        int&& delta_x = point2.x - point1.x;
        int&& delta_y = point2.y - point1.y;

        double error = 0;
        double delta_error = abs((double)delta_y / delta_x);
        const int sign_num = delta_y < 0 ? -1 : 1;

        for(int t = - thickness; t <= thickness; t++)
        {
            double j = point1.y;
            error = 0;

            for(int i = point1.x + t; i <= point2.x + t ; i++)
            {
                if(i >= 0 && i < dst.cols && j >= 0 && j < dst.rows)
                {
                    dst.at<cv::Vec3b>(j, i)[0] = color[0];
                    dst.at<cv::Vec3b>(j, i)[1] = color[1];
                    dst.at<cv::Vec3b>(j, i)[2] = color[2];
                }

                error += delta_error;
                while(error >= 0.5)
                {
                    if(i >= 0 && i < dst.cols && j >= 0 && j < dst.rows)
                    {
                        dst.at<cv::Vec3b>(j, i)[0] = color[0];
                        dst.at<cv::Vec3b>(j, i)[1] = color[1];
                        dst.at<cv::Vec3b>(j, i)[2] = color[2];
                    }
                    j += sign_num;
                    error -= 1.0;
                }
            }
        }
    }

    inputArray.release();
    inputArray = dst.clone();
    dst.release();
}

imgCore::imgCore()
{
}
