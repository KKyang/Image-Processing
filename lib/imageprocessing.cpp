#include "imageprocessing.h"
#include <omp.h>
#include <cmath>
#include <iostream>
#define ffff(x) x > 0.008856 ? pow(x, 1/3) : 7.787 * x + 16 / 116
using namespace myCV;

void myCV::customFilter(cv::Mat &inputArray, cv::Mat &outputArray, int mask_w, int mask_h, std::vector<int> &mask)
{
    //Initial Values
    int count = 0, total = 0;

    int type = inputArray.type() == CV_8UC3 ? CV_8UC3 : CV_8UC1;
    cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, type);

    //Check ksize is larger than image width/ height
    if (mask_w > (inputArray.size().width / 2) || mask_h > (inputArray.size().height / 2))
    {
        outputArray.release();
        outputArray = inputArray.clone();
        return;
    }

    if(type == CV_8UC1)
    {
        int i, y, x;
        #pragma omp parallel for private(i,y,x) firstprivate(total, count)
        for (int j = 0; j < inputArray.size().height; j++)
        {
            for (i = 0; i < inputArray.size().width; i++)
            {
                total = 0;
                count = 0;

                //Blur.
                for (y = -mask_h / 2; y <= mask_h / 2; y++)
                {
                    for (x = -mask_w / 2; x <= mask_w / 2; x++)
                    {
                        int&& tx = i + x;
                        int&& ty = j + y;
                        if (tx >= 0 && tx < inputArray.size().width && ty >= 0 && ty < inputArray.size().height)
                        {
                            total += mask[(y+(mask_h/2))*mask_w+(x+(mask_w/2))]*inputArray.at<uchar>(ty,tx);
                            count += mask[(y+(mask_h/2))*mask_w+(x+(mask_w/2))];
                        }
                    }
                }
                if(count == 0){count = 1;}
                int&& result = (total / count) > 255 ? 255 : (total / count);
                if(result < 0){result = 0;}
                tmp.at<uchar>(j,i) = result;
            }
        }
    }
    else if(type == CV_8UC3)
    {
        int i, k, y, x;
        #pragma omp parallel for private(i,k,y,x) firstprivate(total, count)
        for (int j = 0; j < inputArray.size().height; j++)
        {
            for (i = 0; i < inputArray.size().width; i++)
            {
                for(k = 0; k < 3; k++)
                {
                    total = 0;
                    count = 0;

                    //Blur.
                    for (y = -mask_h / 2; y <= mask_h / 2; y++)
                    {
                        for (x = -mask_w / 2; x <= mask_w / 2; x++)
                        {
                            int&& tx = i + x;
                            int&& ty = j + y;
                            if (tx >= 0 && tx < inputArray.size().width && ty >= 0 && ty < inputArray.size().height)
                            {
                                total += mask[(y+(mask_h/2))*mask_w+(x+(mask_w/2))]*inputArray.at<cv::Vec3b>(ty,tx)[k];
                                count += mask[(y+(mask_h/2))*mask_w+(x+(mask_w/2))];
                            }
                        }
                    }
                    if(count == 0){count = 1;}
                    int&& result = (total / count) > 255 ? 255 : (total / count);
                    if(result < 0){result = 0;}
                    tmp.at<cv::Vec3b>(j,i)[k] = result;
                }
            }
        }
    }
    outputArray.release();
    outputArray = tmp.clone();
    tmp.release();
}

int myCV::findHistLargestPos(std::vector<std::vector<int>> &data)
{
    int largestNum = 0;
    int Pos = 0;
    //find pixel appears most by for loop.
    for(int j = 0; j < data.size();j++)
        for(int i = 0; i < data[j].size();i++)
        {
            if(data[j][i] > largestNum)
            {
                largestNum = data[j][i];
                Pos = i;
            }
        }
    return Pos;
}

void myCV::grayResolutionScale(cv::Mat &inputArray, cv::Mat &outputArray, int scaleLevel)
{
    cv::Mat tmp;
    inputArray.type() == CV_8UC3 ? myCvtColor(inputArray, tmp, BGR2GRAY):
                                   tmp = inputArray.clone();
    double scale;
    if(scaleLevel < 0)
        scaleLevel = 0;
    else if(scaleLevel > 8)
    {
        scaleLevel = 8;
    }
    scale = pow(2,(8-scaleLevel));

    cv::Mat&& dest = cv::Mat::zeros(tmp.size().height, tmp.size().width, CV_8UC1);
    int i = 0;
    #pragma omp parallel for private(i)
    for(int j = 0; j < tmp.size().height;j++)
        for(i = 0; i < tmp.size().width;i++)
        {
            auto&& gray = cvRound((tmp.at<uchar>(j,i) + 1) / scale);
            gray = (gray * scale) -1;

            gray = gray > 255 ? 255 : gray;
            gray = gray < 0   ? 0   : gray;
            dest.at<uchar>(j,i) = gray;
        }
    outputArray.release();
    outputArray = dest.clone();
    tmp.release();
    dest.release();
}

void myCV::laplacianFilter(cv::Mat &inputArray, cv::Mat &outputArray, const int size)
{
    int mask[9] = { 0, 1, 0,
                    1,-4, 1,
                    0, 1, 0};
    cv::Mat tmp;
    if(inputArray.type()==CV_8UC3)
    {
        myCvtColor(inputArray, tmp, BGR2GRAY);
    }
    else
        tmp = inputArray.clone();
    cv::Mat&& dest = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC1);

    int i, x, y, sum;

    #pragma omp parallel for private(i, x, y) firstprivate(sum)
    for(int j = 0; j < tmp.size().height; j++)
        for(i = 0; i < tmp.size().width; i++)
        {
            sum = 0;
            for (y = -1; y <= 1; y++)
            {
                for (x = -1; x <= 1; x++)
                {
                    int&& tx = i + x;
                    int&& ty = j + y;
                    if (tx >= 0 && tx < tmp.size().width && ty >= 0 && ty < tmp.size().height)
                    {
                        sum+= tmp.at<uchar>(ty, tx) * mask[(y+1)*3 + (x+1)];
                    }

                }
            }
            sum = sum > 255 ? 255 : sum;
            sum = sum < 0   ? 0   : sum;
            dest.at<uchar>(j,i) = sum;
        }

    outputArray.release();
    outputArray = dest.clone();
    tmp.release();
    dest.release();
}

//Currently only grayscaled image works.
void myCV::medianFilter(cv::Mat &inputArray, cv::Mat &outputArray, const int size)
{
    cv::Mat tmp;
    if(inputArray.type()==CV_8UC3)
    {
        myCvtColor(inputArray, tmp, BGR2GRAY);
    }
    else
        tmp = inputArray.clone();
    cv::Mat&& dest = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC1);

    int i, x, y, k;

    std::vector<int> tmpArr;

    #pragma omp parallel for private(i,y,x) firstprivate(tmpArr, k)
    for(int j = 0; j < tmp.size().height; j++)
        for(i = 0; i < tmp.size().width; i++)
        {
            tmpArr.clear();
            tmpArr.resize(size*size,0);
            k =0;
            for (y = -size / 2; y <= size / 2; y++)
            {
                for (x = -size / 2; x <= size / 2; x++)
                {
                    int&& tx = i + x;
                    int&& ty = j + y;
                    if (tx >= 0 && tx < tmp.size().width && ty >= 0 && ty < tmp.size().height)
                    {
                        tmpArr[k]=tmp.at<uchar>(ty,tx);
                        k++;
                    }

                }
            }
            std::sort(tmpArr.begin(), tmpArr.begin() + k);
            dest.at<uchar>(j, i) = tmpArr[(k+1)/2];
        }

    outputArray.release();
    outputArray = dest.clone();
    tmp.release();
    dest.release();
    tmpArr.clear();
}

void myCV::myContrast(cv::Mat &inputArray, cv::Mat &outputArray, int min, int max, bool ifauto)
{
    int DEFMIN = 0, DEFMAX = 255; //define stratching goal.
    if(min > max)
    {
        std::cout << "Error! Min value is larger than max value!";
        return;
    }
    //Use type to check if the image is color or not.
    if(inputArray.type() == CV_8UC3)
    {
        if(ifauto)  //An user is able to let computer auto find min, max pixels.
        {
            for(int j = 0; j < inputArray.size().height; j++)
                for(int i = 0; i < inputArray.size().width;i++)
                {
                    auto&& val = (inputArray.at<cv::Vec3b>(j,i)[0]+
                                  inputArray.at<cv::Vec3b>(j,i)[1]+
                                  inputArray.at<cv::Vec3b>(j,i)[2])/3;

                    min = min < val ? min : val;
                    max = max > val ? max : val;
                }
        }

        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC3);
        int i = 0;
        //The stretch-based contrast algorithm. Formula at http://homepages.inf.ed.ac.uk/rbf/HIPR2/stretch.htm
        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width; i++)
            {
                auto&& b = (inputArray.at<cv::Vec3b>(j,i)[2] - min) * ((double)(DEFMAX-DEFMIN) / (double)(max - min)) + DEFMIN;
                auto&& g = (inputArray.at<cv::Vec3b>(j,i)[1] - min) * ((double)(DEFMAX-DEFMIN) / (double)(max - min)) + DEFMIN;
                auto&& r = (inputArray.at<cv::Vec3b>(j,i)[0] - min) * ((double)(DEFMAX-DEFMIN) / (double)(max - min)) + DEFMIN;
                //Check the boundary overflow.
                b = b < 0   ? 0   : b;
                b = b > 255 ? 255 : b;
                g = g < 0   ? 0   : g;
                g = g > 255 ? 255 : g;
                r = r < 0   ? 0   : r;
                r = r > 255 ? 255 : r;
                tmp.at<cv::Vec3b>(j,i) = cv::Vec3b(r, g, b);
            }
        //Copy the result to outputArray.
        outputArray.release();
        outputArray = tmp.clone();
        tmp.release();
    }
    else if(inputArray.type() == CV_8UC1)  //Grayscale mode.
    {
        if(ifauto)
        {
            for(int j = 0; j < inputArray.size().height; j++)
                for(int i = 0; i < inputArray.size().width;i++)
                {
                    auto&& val = inputArray.at<uchar>(j,i);

                    min = min < val ? min : val;
                    max = max > val ? max : val;
                }
        }

        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC1);
        int i = 0;
        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width; i++)
            {
                auto&& val = (inputArray.at<uchar>(j,i) - min) * ((double)(DEFMAX-DEFMIN) / (double)(max - min)) + DEFMIN;

                val = val < 0   ? 0   : val;
                val = val > 255 ? 255 : val;

                tmp.at<uchar>(j,i) = val;
            }
        outputArray.release();
        outputArray = tmp.clone();
        tmp.release();
    }
}

void myCV::myCvtColor(cv::Mat &inputArray, cv::Mat &outputArray, int colorType, int grayType)
{
    int i=0;
    if(colorType == BGR2GRAY)  //BGR2GRAY
    {
        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC1);

        if(grayType == GRAY_LUMI)  //Using luminus
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
        else if(grayType == GRAY_AVG) //Using average
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
    else if(colorType == GRAY2GBR) //GRAY2BGR. Copy one channel three times.
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
    else if(colorType == BGR2YCbCr) //BGR2YCbCr (YUV). Formula at http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html
    {
        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC3);

        int i = 0;

        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width; i++)
            {
                int&& y = inputArray.at<cv::Vec3b>(j,i)[0] * 0.114 +
                          inputArray.at<cv::Vec3b>(j,i)[1] * 0.587 +
                          inputArray.at<cv::Vec3b>(j,i)[2] * 0.299;
                tmp.at<cv::Vec3b>(j,i)[0] =   y;
                tmp.at<cv::Vec3b>(j,i)[1] =   ((int)inputArray.at<cv::Vec3b>(j,i)[2] - y) * 0.713 + 128;
                tmp.at<cv::Vec3b>(j,i)[2] =   ((int)inputArray.at<cv::Vec3b>(j,i)[0] - y) * 0.564 + 128;
            }

        outputArray.release();
        outputArray = tmp.clone();
        tmp.release();
    }
    else if(colorType == YCbCr2BGR) //YCbCr2BGR
    {
        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC3);

        int i = 0;
        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width;i++)
            {
                auto&& r = inputArray.at<cv::Vec3b>(j,i)[0] +
                           1.403   * (inputArray.at<cv::Vec3b>(j,i)[2] - 128);
                auto&& g = inputArray.at<cv::Vec3b>(j,i)[0] -
                           0.344 * (inputArray.at<cv::Vec3b>(j,i)[1] - 128) -
                           0.714 * (inputArray.at<cv::Vec3b>(j,i)[2] - 128);
                auto&& b = inputArray.at<cv::Vec3b>(j,i)[0] +
                           1.773   * (inputArray.at<cv::Vec3b>(j,i)[1] - 128);

                b = b < 0   ? 0   : b;
                b = b > 255 ? 255 : b;
                g = g < 0   ? 0   : g;
                g = g > 255 ? 255 : g;
                r = r < 0   ? 0   : r;
                r = r > 255 ? 255 : r;

                tmp.at<cv::Vec3b>(j,i) = cv::Vec3b(r, g, b);
            }

        outputArray.release();
        outputArray = tmp.clone();
        tmp.release();
    }
    else if(colorType == BGR2HSV)
    {
        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_32FC3);
        int i = 0;
        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width; i++)
            {
                auto&& b = inputArray.at<cv::Vec3b>(j,i)[0];
                auto&& g = inputArray.at<cv::Vec3b>(j,i)[1];
                auto&& r = inputArray.at<cv::Vec3b>(j,i)[2];
                int max = b > g   ? b : g ;
                    max = r > max ? r : max ;
                int min = b < g   ? b : g ;
                    min = r < min ? r : min ;

                tmp.at<cv::Vec3f>(j,i)[2] = max;
                if(max == 0){tmp.at<cv::Vec3f>(j,i)[1] = 0;}
                else        {tmp.at<cv::Vec3f>(j,i)[1] = 1 - ((double)min / (double)max);}
                if(max == min)             {tmp.at<cv::Vec3f>(j,i)[0] = 0;}
                else if(max == r && g >= b){tmp.at<cv::Vec3f>(j,i)[0] = 60 * (double)(g - b)/(double)(max - min);}
                else if(max == r && g <  b){tmp.at<cv::Vec3f>(j,i)[0] = 60 * (double)(g - b)/(double)(max - min) + 360;}
                else if(max == g)          {tmp.at<cv::Vec3f>(j,i)[0] = 60 * (double)(b - r)/(double)(max - min) + 120;}
                else if(max == b)          {tmp.at<cv::Vec3f>(j,i)[0] = 60 * (double)(r - g)/(double)(max - min) + 240;}
            }

        outputArray.release();
        outputArray = tmp.clone();
        tmp.release();
    }
    else if(colorType == HSV2BGR)
    {
        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC3);
        int i = 0;
        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width; i++)
            {
                auto&& h = inputArray.at<cv::Vec3f>(j, i)[0];
                auto&& s = inputArray.at<cv::Vec3f>(j, i)[1];
                auto&& v = inputArray.at<cv::Vec3f>(j, i)[2];
                int hh = (int)(h / 60) % 6;
                float ff = (h / 60) - hh;
                int p = v * (1.0 - s);
                int q = v * (1.0 - ff * s);
                int t = v * (1.0 - (1.0 - ff) * s);

                switch (hh) {
                case 0:
                    tmp.at<cv::Vec3b>(j, i)[0] = p;
                    tmp.at<cv::Vec3b>(j, i)[1] = t;
                    tmp.at<cv::Vec3b>(j, i)[2] = v;
                    break;
                case 1:
                    tmp.at<cv::Vec3b>(j, i)[0] = p;
                    tmp.at<cv::Vec3b>(j, i)[1] = v;
                    tmp.at<cv::Vec3b>(j, i)[2] = q;
                    break;
                case 2:
                    tmp.at<cv::Vec3b>(j, i)[0] = t;
                    tmp.at<cv::Vec3b>(j, i)[1] = v;
                    tmp.at<cv::Vec3b>(j, i)[2] = p;
                    break;
                case 3:
                    tmp.at<cv::Vec3b>(j, i)[0] = v;
                    tmp.at<cv::Vec3b>(j, i)[1] = q;
                    tmp.at<cv::Vec3b>(j, i)[2] = p;
                    break;
                case 4:
                    tmp.at<cv::Vec3b>(j, i)[0] = v;
                    tmp.at<cv::Vec3b>(j, i)[1] = p;
                    tmp.at<cv::Vec3b>(j, i)[2] = t;
                    break;
                case 5:
                    tmp.at<cv::Vec3b>(j, i)[0] = q;
                    tmp.at<cv::Vec3b>(j, i)[1] = p;
                    tmp.at<cv::Vec3b>(j, i)[2] = v;
                    break;
                }
            }

        outputArray.release();
        outputArray = tmp.clone();
        tmp.release();
    }
    else if(colorType == BGR2CMY)
    {
        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC3);
        int i = 0;
        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width; i++)
            {
                tmp.at<cv::Vec3b>(j, i)[0] = 255-inputArray.at<cv::Vec3b>(j, i)[0];
                tmp.at<cv::Vec3b>(j, i)[1] = 255-inputArray.at<cv::Vec3b>(j, i)[1];
                tmp.at<cv::Vec3b>(j, i)[2] = 255-inputArray.at<cv::Vec3b>(j, i)[2];
            }

        outputArray.release();
        outputArray = tmp.clone();
        tmp.release();
    }
    else if(colorType == BGR2XYZ)
    {
        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_32FC3);
        int i = 0;
        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width; i++)
            {
                tmp.at<cv::Vec3f>(j, i)[0] = 0.412453 * ((float)inputArray.at<cv::Vec3b>(j, i)[2] / 255.0) +
                                             0.357580 * ((float)inputArray.at<cv::Vec3b>(j, i)[1] / 255.0) +
                                             0.180423 * ((float)inputArray.at<cv::Vec3b>(j, i)[0] / 255.0);
                tmp.at<cv::Vec3f>(j, i)[1] = 0.212671 * ((float)inputArray.at<cv::Vec3b>(j, i)[2] / 255.0) +
                                             0.715160 * ((float)inputArray.at<cv::Vec3b>(j, i)[1] / 255.0) +
                                             0.072169 * ((float)inputArray.at<cv::Vec3b>(j, i)[0] / 255.0);
                tmp.at<cv::Vec3f>(j, i)[2] = 0.019334 * ((float)inputArray.at<cv::Vec3b>(j, i)[2] / 255.0) +
                                             0.119193 * ((float)inputArray.at<cv::Vec3b>(j, i)[1] / 255.0) +
                                             0.950227 * ((float)inputArray.at<cv::Vec3b>(j, i)[0] / 255.0);
            }

        outputArray.release();
        outputArray = tmp.clone();
        tmp.release();
    }
    else if(colorType == XYZ2Lab)
    {
        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_32FC3);
        const float Xn = 0.33333, Yn = 0.33333, Zn = 0.33333;
        int i = 0;
        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width; i++)
            {
                 float X = inputArray.at<cv::Vec3f>(j, i)[0];
                 float Y = inputArray.at<cv::Vec3f>(j, i)[1];
                 float Z = inputArray.at<cv::Vec3f>(j, i)[2];

                 tmp.at<cv::Vec3f>(j, i)[0] = (Y / Yn) > 0.008856 ? 116.0 * pow((Y / Yn),0.33333) - 16.0 : 903.3 * (Y / Yn);
                 tmp.at<cv::Vec3f>(j, i)[1] = 500.0 * (ffff(X / Xn) - ffff(Y / Yn));
                 tmp.at<cv::Vec3f>(j, i)[2] = 200.0 * (ffff(Y / Yn) - ffff(Z / Zn));
            }
        outputArray.release();
        outputArray = tmp.clone();
        tmp.release();
    }
    else if(colorType == BGR2YUV)
    {
        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC3);
        int i = 0;
        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width; i++)
            {
                 tmp.at<cv::Vec3b>(j, i)[0] =  0.299 * (float)inputArray.at<cv::Vec3b>(j, i)[2]
                                              +0.587 * (float)inputArray.at<cv::Vec3b>(j, i)[1]
                                              +0.114 * (float)inputArray.at<cv::Vec3b>(j, i)[0];
                 tmp.at<cv::Vec3b>(j, i)[1] = -0.169 * (float)inputArray.at<cv::Vec3b>(j, i)[2]
                                              -0.331 * (float)inputArray.at<cv::Vec3b>(j, i)[1]
                                              +0.5   * (float)inputArray.at<cv::Vec3b>(j, i)[0] + 128.0;
                 tmp.at<cv::Vec3b>(j, i)[2] =  0.5   * (float)inputArray.at<cv::Vec3b>(j, i)[2]
                                              -0.419 * (float)inputArray.at<cv::Vec3b>(j, i)[1]
                                              -0.081 * (float)inputArray.at<cv::Vec3b>(j, i)[0] + 128.0;
            }
        outputArray.release();
        outputArray = tmp.clone();
        tmp.release();
    }

}

//Threshold that auto find image's peak.
void myCV::myThreshold(cv::Mat &inputArray, cv::Mat &outputArray)
{
    std::vector<std::vector<int>> data;
    histogram(inputArray, cv::Mat(), data);
    myThreshold(inputArray, outputArray, findHistLargestPos(data),0 , 255);
}

//Manual threshold
void myCV::myThreshold(cv::Mat &inputArray, cv::Mat &outputArray, unsigned int _threshold, unsigned _min, unsigned _max)
{
    cv::Mat tmp;
    cv::Mat&& dest = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC1);
    if(_min < 0  ){_min  = 0;}
    if(_max > 255){ _max = 255;}

    if(_threshold < 0 || _threshold > 255){_threshold = 128;}

    if(inputArray.type() == CV_8UC3){myCvtColor(inputArray, tmp, BGR2GRAY);}
    else{tmp = inputArray.clone();}

    int i = 0;
    #pragma omp parallel for private(i)
    for(int j=0; j < inputArray.size().height;j++)
    {
        for(i = 0; i < inputArray.size().width;i++)
        {
            //Make value larger than threshold _max, vice versa.
            if(tmp.at<uchar>(j,i) >= _threshold){dest.at<uchar>(j,i) = _max;}
            else{dest.at<uchar>(j,i) = _min;}
        }
    }

    outputArray.release();
    outputArray = dest.clone();
    tmp.release();
    dest.release();
}

//find histogram
void myCV::histogram(cv::Mat &inputArray, cv::Mat &hist)
{
    std::vector<std::vector<int>> data;
    histogram(inputArray, hist, data);
}

//find histogram. Also return data results.
void myCV::histogram(cv::Mat &inputArray, cv::Mat &hist, std::vector<std::vector<int>> &data)
{
    int i=0;
    int largestCount = 0;
    //type check
    if(inputArray.type() == CV_8UC3) //color
    {
        cv::Mat&& result = cv::Mat(100, 256, CV_8UC3, cv::Scalar(255,255,255));
        data.resize(3); //resize vector size to 3 channels
        data[0].resize(256, 0); //each 256 blocks
        data[1].resize(256, 0);
        data[2].resize(256, 0);
        //Calculate histogram
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
        //Normalization
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
        //Draw histogram results to mat.
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
    else if(inputArray.type() == CV_8UC1) //gray
    {
        cv::Mat&& result = cv::Mat(100, 256, CV_8UC3, cv::Scalar(255,255,255));
        data.resize(1); //resize vector
        data[0].resize(256, 0);
        //Calculate histogram/
        #pragma omp parallel for private(i)
        for(int j=0; j < inputArray.size().height;j++)
        {
            for(i = 0; i < inputArray.size().width;i++)
            {
                data[0][inputArray.at<uchar>(j,i)]++;
            }
        }
        //Normalization
        for (i = 0; i < 256; i++)
        {
            if (data[0][i] > largestCount)
            {
                largestCount = data[0][i];
            }
        }
        //Draw histogram
        int j = 0;
        #pragma omp parallel for private(j)
        for (i = 0; i < 256; i ++)
        {
            for (j = 0; j < 100-(int)(((double)data[0][i] / (double)largestCount) * 100); j++)
            {
                    result.at<uchar>(j,i) = 0;
            }
        }
        //Output as BGR format. (Black-n-white image)
        myCvtColor(result, result, GRAY2GBR);
        hist.release();
        hist = result.clone();
        result.release();
    }
}

void myCV::EqualizeHist(cv::Mat &inputArray, cv::Mat &outputArray)
{
    std::vector<std::vector<int>> data;
    std::vector<int> Transform(256, 0);

    int sum = 0;
    int sig = 0;
    //type check
    if(inputArray.type() == CV_8UC3) //color version use Y (luma) to do equalization.
    {
        cv::Mat HSV;
        std::vector<cv::Mat> mats;
        cv::Mat&& v = cv::Mat::zeros(inputArray.rows, inputArray.cols, CV_8UC1);

        myCV::myCvtColor(inputArray, HSV, BGR2HSV);
        cv::split(HSV, mats);

        int i = 0;
        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width;i++)
            {
                v.at<uchar>(j,i) =  (uchar)mats[2].at<float>(j,i);
            }

        histogram(v, cv::Mat(), data);

        for(i = 0; i < 256; i++)
        {
            sum += data[0][i];
        }

        for(i = 0; i < 256; i++)
        {
            sig += data[0][i];
            Transform[i] = cvRound(255 * (double)sig/(double)sum);
        }


        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width;i++)
            {
                mats[2].at<float>(j,i) =  Transform[(int)v.at<uchar>(j,i)];
            }

        cv::merge(mats, HSV);
        myCvtColor(HSV, outputArray, HSV2BGR);
        HSV.release();
    }
    else if(inputArray.type() == CV_8UC1)  //gray version of equalization
    {
        histogram(inputArray, cv::Mat(), data);

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

void myCV::pseudoColor(cv::Mat &inputArray, cv::Mat &outputArray, float start_angle)
{
    if(inputArray.type() != CV_8UC1)
        return;
    int sa;
    if(start_angle > 360){sa = 360;}
    else if(start_angle < 0){sa = 0;}
    else{sa = start_angle;}
    float s = 1.0;
    float v = 255.0;

    float one_step = 255.0 / 360.0;
    std::vector<std::vector<int>> pseudo_array(256, std::vector<int>(3, 0));

    int i = 0;
#pragma omp parallel for
    for(i = 0; i < 256; i++)
    {
        int h = start_angle + one_step * i;
        int hh = (int)(h / 60) % 6;
        float ff = (h / 60) - hh;
        int p = v * (1.0 - s);
        int q = v * (1.0 - ff * s);
        int t = v * (1.0 - (1.0 - ff) * s);

        switch (hh) {
        case 0:
            pseudo_array[i][0] = p;
            pseudo_array[i][1] = t;
            pseudo_array[i][2] = v;
            break;
        case 1:
            pseudo_array[i][0] = p;
            pseudo_array[i][1] = v;
            pseudo_array[i][2] = q;
            break;
        case 2:
            pseudo_array[i][0] = t;
            pseudo_array[i][1] = v;
            pseudo_array[i][2] = p;
            break;
        case 3:
            pseudo_array[i][0] = v;
            pseudo_array[i][1] = q;
            pseudo_array[i][2] = p;
            break;
        case 4:
            pseudo_array[i][0] = v;
            pseudo_array[i][1] = p;
            pseudo_array[i][2] = t;
            break;
        case 5:
            pseudo_array[i][0] = q;
            pseudo_array[i][1] = p;
            pseudo_array[i][2] = v;
            break;
        }

    }

    cv::Mat&& dest = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC3);
    int k;

    #pragma omp parallel for private(i, k)
    for(int j = 0; j < inputArray.size().height; j++)
        for(i = 0; i < inputArray.size().width;i++)
        {
            for(k = 0; k < 3; k++)
            {
                dest.at<cv::Vec3b>(j,i)[k] = pseudo_array[(int)inputArray.at<uchar>(j,i)][k];
            }
        }

    outputArray.release();
    outputArray = dest.clone();
    dest.release();

}

void myCV::sobelFilter(cv::Mat &inputArray, cv::Mat &outputArray)
{
    std::vector<int> mask1={-1,-2,-1,
                             0, 0, 0,
                             1, 2, 1};
    std::vector<int> mask2={-1, 0, 1,
                            -2, 0, 2,
                            -1, 0, 1};

    cv::Mat tmp1, tmp2;
    customFilter(inputArray, tmp1, 3, 3, mask1);
    customFilter(inputArray, tmp2, 3, 3, mask2);

    cv::Mat&& dest = (tmp1/2) + (tmp2/2);
    outputArray.release();
    outputArray = dest.clone();
    tmp1.release();
    tmp2.release();
    dest.release();
}

void Blur::simple(cv::Mat &inputArray, cv::Mat &outputArray, const int _ksize)
{
    //Initial Values
    int ksize = 0;
    int count = 0, total = 0;

    int type = inputArray.type() == CV_8UC3 ? CV_8UC3 : CV_8UC1;
    cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, type);

    //Check ksize is larger than image width/ height
    if (_ksize > (inputArray.size().width / 2) || _ksize > (inputArray.size().height / 2))
    {
        ksize = inputArray.size().width > inputArray.size().height ? (inputArray.size().height / 2) : (inputArray.size().width / 2);
    }
    else
    {
        ksize = _ksize;
    }

    if (ksize % 2 == 0){ksize = ksize - 1;}

    if(type == CV_8UC1)
    {
        int i, y, x;
        #pragma omp parallel for private(i,y,x) firstprivate(total, count)
        for (int j = 0; j < inputArray.size().height; j++)
        {
            for (i = 0; i < inputArray.size().width; i++)
            {
                total = 0;
                count = 0;

                //Blur.
                for (y = -ksize / 2; y <= ksize / 2; y++)
                {
                    for (x = -ksize / 2; x <= ksize / 2; x++)
                    {
                        int&& tx = i + x;
                        int&& ty = j + y;
                        if (tx >= 0 && tx < inputArray.size().width && ty >= 0 && ty < inputArray.size().height)
                        {
                            total += inputArray.at<uchar>(ty,tx);
                            count++;
                        }
                    }
                }
                tmp.at<uchar>(j,i) = (total / count);
            }
        }
    }
    else if(type == CV_8UC3)
    {
        int i, k, y, x;
        #pragma omp parallel for private(i,k,y,x) firstprivate(total, count)
        for (int j = 0; j < inputArray.size().height; j++)
        {
            for (i = 0; i < inputArray.size().width; i++)
            {
                for(k = 0; k < 3; k++)
                {
                    total = 0;
                    count = 0;

                    //Blur.
                    for (y = -ksize / 2; y <= ksize / 2; y++)
                    {
                        for (x = -ksize / 2; x <= ksize / 2; x++)
                        {
                            int&& tx = i + x;
                            int&& ty = j + y;
                            if (tx >= 0 && tx < inputArray.size().width && ty >= 0 && ty < inputArray.size().height)
                            {
                                total += inputArray.at<cv::Vec3b>(ty,tx)[k];
                                count++;
                            }
                        }
                    }
                    tmp.at<cv::Vec3b>(j,i)[k] = (total / count);
                }
            }
        }
    }
    outputArray.release();
    outputArray = tmp.clone();
    tmp.release();
}

void Blur::Gaussian(cv::Mat &inputArray, cv::Mat &outputArray, const int _ksize, const float sigmaX, float sigmaY)
{
    //Initial Values
    if(sigmaY == 0){sigmaY = sigmaX;}
    float total = 0;
    int ksize;      //mask size.

    int type = inputArray.type() == CV_8UC3 ? CV_8UC3 : CV_8UC1;
    cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, type);

    //Check ksize is larger than image width/ height
    if (_ksize > (inputArray.size().width / 2) || _ksize > (inputArray.size().height / 2))
    {
        ksize = inputArray.size().width > inputArray.size().height ? (inputArray.size().height / 2) : (inputArray.size().width / 2);
    }
    else
    {
        ksize = _ksize;
    }

    if (ksize % 2 == 0){ksize = ksize - 1;}

    std::vector<std::vector<float>> mask(ksize, std::vector<float>(ksize, 0));

    float wsum = 0;   //mask weight sum

    //mask calculation
    int i = 0, y = 0, x = 0;
    #pragma omp parallel for private(i)
    for(int j =0 ; j < ksize; j++)
    {
        for(i=0; i < ksize; i++)
        {
            mask[i][j] = (float)(1 / (2 * M_PI * sigmaX * sigmaX))
                                 * exp(-(((i - (int)(ksize / 2)) * (i - (int)(ksize / 2)) +
                                (j - (int)(ksize / 2)) * (j - (int)(ksize / 2))) / (2 * sigmaX * sigmaX)));
        }
    }

    if(type == CV_8UC1)
    {
        #pragma omp parallel for private(i,y,x) firstprivate(total,wsum)
        for (int j = 0; j < inputArray.size().height; j++)
        {
            for (i = 0; i < inputArray.size().width; i++)
            {
                total = 0;
                wsum = 0;
                //Blur.
                for (y = -ksize / 2; y <= ksize / 2; y++)
                {
                    for (x = -ksize / 2; x <= ksize / 2; x++)
                    {
                        int&& tx = i + x;
                        int&& ty = j + y;
                        if (tx >= 0 && tx < inputArray.size().width && ty >= 0 && ty < inputArray.size().height)
                        {
                            total += (float)(inputArray.at<uchar>(ty,tx)) * (float)mask[x + (ksize / 2)][y + (ksize / 2)];
                            wsum += mask[x + (ksize / 2)][y + (ksize / 2)];
                        }
                    }
                }
                tmp.at<uchar>(j,i) = cvRound(total / wsum);
            }
        }
    }
    else if(type == CV_8UC3)
    {
        int k = 0;
        #pragma omp parallel for private(i,k,y,x) firstprivate(total,wsum)
        for (int j = 0; j < inputArray.size().height; j++)
        {
            for (i = 0; i < inputArray.size().width; i++)
            {
                for(k = 0; k < 3; k++)
                {
                    total = 0;
                    wsum = 0;
                    //Blur.
                    for (y = -ksize / 2; y <= ksize / 2; y++)
                    {
                        for (x = -ksize / 2; x <= ksize / 2; x++)
                        {
                            int&& tx = i + x;
                            int&& ty = j + y;
                            if (tx >= 0 && tx < inputArray.size().width && ty >= 0 && ty < inputArray.size().height)
                            {
                                total += (float)(inputArray.at<cv::Vec3b>(ty,tx))[k] * (float)mask[x + (ksize / 2)][y + (ksize / 2)];
                                wsum += mask[x + (ksize / 2)][y + (ksize / 2)];
                            }
                        }
                    }
                    tmp.at<cv::Vec3b>(j,i)[k] = cvRound(total / wsum);
                }
            }
        }
    }
    outputArray.release();
    outputArray = tmp.clone();
    tmp.release();
}
