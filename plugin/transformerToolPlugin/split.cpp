#include "split.h"
#include "iostream"
void split3(cv::Mat &img, int colorType, std::vector<cv::Mat> &img_set)
{
    if(img.type() == CV_8UC1)
    {
        img_set.resize(1);
        return;
    }

    img_set.resize(4);
    for(int i = 0; i < 4; i++)
    {
        img_set[i] = cv::Mat(img.rows, img.cols, CV_8UC3, cv::Scalar(0,0,0)).clone();
    }
    if(colorType == myCV::RGB || colorType == myCV::CMY)
    {
        int i, j;
#pragma omp parallel for private(i)
        for(j = 0; j < img.rows; j++)
        {
            for(i = 0; i < img.cols; i++)
            {
                img_set[1].at<cv::Vec3b>(j, i)[2] = img.at<cv::Vec3b>(j, i)[0];
                img_set[2].at<cv::Vec3b>(j, i)[1] = img.at<cv::Vec3b>(j, i)[1];
                img_set[3].at<cv::Vec3b>(j, i)[0] = img.at<cv::Vec3b>(j, i)[2];
            }
        }
        return;
    }
    else if(colorType == myCV::HSV || colorType == myCV::XYZ || colorType == myCV::Lab)
    {
        std::vector<cv::Mat> splitted;
        cv::split(img, splitted);

        for(int i = 0; i < 3; i++)
        {
            myCV::normalize(splitted[i], splitted[i], 0, 255, myCV::MINMAX);
        }
        int i, j, k;
#pragma omp parallel for private(i, k)
        for(j = 0; j < img.rows; j++)
        {
            for(i = 0; i < img.cols; i++)
            {
                for( k = 0; k < 3; k++)
                {
                     img_set[1].at<cv::Vec3b>(j, i)[k] = splitted[0].at<float>(j, i);
                     img_set[2].at<cv::Vec3b>(j, i)[k] = splitted[1].at<float>(j, i);
                     img_set[3].at<cv::Vec3b>(j, i)[k] = splitted[2].at<float>(j, i);
                }
            }
        }
        return;
    }
    /*
    else if(colorType == myCV::XYZ || colorType == myCV::Lab)
    {

        std::vector<cv::Mat> splitted;
        cv::split(img, splitted);

        for(int i = 0; i < 3; i++)
        {
            myCV::normalize(splitted[i], splitted[i], 0, 255, myCV::MINMAX);
        }
        int i, j;
#pragma omp parallel for private(i)
        for(j = 0; j < img.rows; j++)
        {
            for(i = 0; i < img.cols; i++)
            {

                  img_set[1].at<cv::Vec3b>(j, i)[2] = splitted[0].at<float>(j, i);
                  img_set[2].at<cv::Vec3b>(j, i)[1] = splitted[1].at<float>(j, i);
                  img_set[3].at<cv::Vec3b>(j, i)[0] = splitted[2].at<float>(j, i);

            }
        }
        return;
    }*/
    else if(colorType == myCV::YUV)
    {
        int i, j, k;
#pragma omp parallel for private(i, k)
        for(j = 0; j < img.rows; j++)
        {
            for(i = 0; i < img.cols; i++)
            {
                for(k = 0; k < 3; k++)
                    img_set[1].at<cv::Vec3b>(j, i)[k] = img.at<cv::Vec3b>(j, i)[0];

                img_set[2].at<cv::Vec3b>(j, i)[1] = img.at<cv::Vec3b>(j, i)[1];
                img_set[2].at<cv::Vec3b>(j, i)[0] = img.at<cv::Vec3b>(j, i)[1];

                img_set[3].at<cv::Vec3b>(j, i)[2] = img.at<cv::Vec3b>(j, i)[2];
                img_set[3].at<cv::Vec3b>(j, i)[1] = img.at<cv::Vec3b>(j, i)[2];
            }
        }
        return;
    }
}
