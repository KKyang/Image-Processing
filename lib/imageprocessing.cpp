#include "imageprocessing.h"
#include <omp.h>
#include <cmath>
#include <iostream>
using namespace myCV;
float ffff(float x){return (x > 0.008856 ? pow(x, 1.0/3.0) : 7.787 * x + 16.0 / 116.0);}

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

//Threshold that auto find image's peak. (OTSU)
void myCV::myThreshold(cv::Mat &inputArray, cv::Mat &outputArray)
{
    cv::Mat tmp;

    if(inputArray.type() == CV_8UC3){myCvtColor(inputArray, tmp, BGR2GRAY);}
    else{tmp = inputArray.clone();}

    std::vector<std::vector<int>> data;
    histogram(tmp, cv::Mat(), data);

    double&& sum = 0;
    double&& weight_sum = 0;
    for(int j = 0; j < 256; j++)
    {
            sum += data[0][j];
            weight_sum += (j + 1) * data[0][j];
    }

    double a, w_a, u, v, g[256];
    for(int j = 0; j < 256; j++)
    {
        a=w_a=0;
        for(int i = 0; i <= j; i++)
        {
            a+= data[0][i];
            w_a+= data[0][i] * (i + 1);
        }
        if(a){u = w_a / a;}
        else u = 0;
        if(sum - a){v = (weight_sum-w_a)/(sum-a);}
        else v = 0;
        g[j]= a *(sum - a)*(u - v) * (u - v);
    }
    double max, min;
    int threshold_value;

    max = min = g[0];

    for(int j = 0; j < 256; j++)
    {
        if(g[j] > max)
        {
            max = g[j];
            threshold_value = j;
        }
        else if(g[j] < min){min = g[j];}
    }
#ifdef _DEBUG
    std::cout << threshold_value << std::endl;
#endif
    myThreshold(inputArray, outputArray, threshold_value, 0, 255);
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

void myCV::BBHE(cv::Mat &inputArray, cv::Mat &outputArray)
{
    if(inputArray.type() != CV_8UC1)
        return;

    if(inputArray.type() == CV_8UC1)
    {
        //black only
        std::vector<int> dataL, dataH;
        std::vector<std::vector<int>> data;
        std::vector<int> Transform(256, 0);
        int avg;

        histogram(inputArray, cv::Mat(), data);

        for(int i = 0; i < 256; i++)
        {
            avg += data[0][i];
        }
        avg = avg / 2;
        int sumL = 0, sumH = 0;
        for(int i = 0; i < 256; i++)
        {
            if(sumL <= avg)
            {
                dataL.push_back(data[0][i]);
                sumL += data[0][i];
            }
            else
                break;
        }

        for(int i = dataL.size(); i < 256; i++)
        {
            dataH.push_back(data[0][i]);
            sumH += data[0][i];
        }

        int sig = 0;
        for(int i = 0; i < dataL.size(); i++)
        {
            sig += dataL[i];
            Transform[i] = cvRound((double)(dataL.size()-1) * (double)sig/(double)sumL);
        }
        std::cout << dataL.size() << std::endl;
        sig = 0;
        for(int i = 0; i < dataH.size(); i++)
        {
            sig += dataH[i];
            Transform[i + dataL.size()] = cvRound((double)(255 - dataL.size())*(double)sig/(double)sumH) + dataL.size();
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

void myCV::pseudoColor(cv::Mat &inputArray, cv::Mat &outputArray, float start_angle, bool outputPseudoMap, cv::Mat &pseudoMap, cv::Size bar_size)
{
    if(inputArray.type() != CV_8UC1)
        return;
    int sa;
    if(start_angle >= 360){sa = 359;}
    else if(start_angle < 0){sa = 0;}
    else{sa = start_angle;}
    float s = 1.0;
    float v = 255.0;

    float one_step = 360.0 / 255.0;
    std::vector<std::vector<int>> pseudo_array(256, std::vector<int>(3, 0));

    int i = 0;
#pragma omp parallel for
    for(i = 0; i < 256; i++)
    {
        float&& h = sa + one_step * i;
        while(h >= 360){h -= 360;}
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

    if(outputPseudoMap)
    {
        if(pseudoMap.empty())
            pseudoMap.release();
        pseudoMap = cv::Mat(bar_size, CV_8UC3).clone();

        #pragma omp parallel for private(i, k)
        for(int j = 0; j < bar_size.height; j++)
            for(i = 0; i < bar_size.width;i++)
            {
                for(k = 0; k < 3; k++)
                {
                    pseudoMap.at<cv::Vec3b>(j,i)[k] = pseudo_array[(int)((i * 255.0) / bar_size.width)][k];
                }
            }
    }
    pseudo_array.clear();

}

void myCV::getPseudoBar(float start_angle, cv::Mat &pseudoMap, cv::Size bar_size)
{
    float sa;
    if(start_angle >= 360){sa = 359;}
    else if(start_angle < 0){sa = 0;}
    else{sa = start_angle;}
    float s = 1.0;
    float v = 255.0;

    float one_step = 360.0 / 255.0;
    std::vector<std::vector<int>> pseudo_array(256, std::vector<int>(3, 0));

    int i = 0;
#pragma omp parallel for
    for(i = 0; i < 256; i++)
    {
        float&& h = sa + one_step * i;       
        while(h >= 360){h -= 360;}
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

    if(pseudoMap.empty())
        pseudoMap.release();
    pseudoMap = cv::Mat(bar_size, CV_8UC3).clone();

    int k;
    #pragma omp parallel for private(i, k)
    for(int j = 0; j < bar_size.height; j++)
        for(i = 0; i < bar_size.width;i++)
        {
            for(k = 0; k < 3; k++)
            {
                pseudoMap.at<cv::Vec3b>(j,i)[k] = pseudo_array[(int)((i * 255.0) / bar_size.width)][k];
            }
        }

    pseudo_array.clear();
}

//It's broken
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
/*Transformation*/
//Bilinear tilt transformation
void myCV::tilt(cv::Mat &inputArray, cv::Mat &outputArray, int tilt_degree, int direction, int center_axis_pos)
{
    const int new_length = direction == 0 ? (inputArray.rows * cos((tilt_degree/180.0)*M_PI)) : (inputArray.cols * cos((tilt_degree/180.0)*M_PI));
    cv::Mat&& dst = direction == 0 ? cv::Mat(new_length, inputArray.cols, CV_8UC3, cv::Scalar(0,0,0)).clone() : cv::Mat(inputArray.rows, new_length, CV_8UC3, cv::Scalar(0,0,0)).clone();
    double padded_input;
    int i, k, padded;

    if(direction == 0)
    {
        #pragma omp parallel for private(i, k) firstprivate(padded, padded_input)
        for(int j = 0; j < dst.rows; j++)
        {
            padded = (j * sin((tilt_degree/180.0)*M_PI))/2;
            padded_input = dst.cols - (2 * padded);
            for(i = padded; i < dst.cols - padded; i++)
            {
                int&& ceilValW = std::ceil((i - padded) * dst.cols / padded_input) > inputArray.size().width-1  ? inputArray.size().width-1  : std::ceil((i - padded) * dst.cols / padded_input);
                int&& ceilValH = std::ceil(j / cos((45/180.0)*M_PI)) > inputArray.size().height-1 ? inputArray.size().height-1 : std::ceil(j / cos((45/180.0)*M_PI));
                int&& floorValW = std::floor((i - padded) * dst.cols / padded_input);
                int&& floorValH = std::floor(j / cos((45/180.0)*M_PI));
                double&& alpha = ceilValW >= inputArray.size().width-1  ? 0 : (double)((double)((i - padded) * dst.cols / padded_input) - floorValW);
                double&& beta  = ceilValH >= inputArray.size().height-1 ? 0 : (double)((double)(j / cos((45/180.0)*M_PI)) - floorValH);
                for(k = 0; k < 3; k++)
                {
                    dst.at<cv::Vec3b>(j, i)[k] = inputArray.at<cv::Vec3b>(floorValH, floorValW)[k]*(1-alpha)*(1-beta) +
                                                 inputArray.at<cv::Vec3b>(floorValH, ceilValW)[k] *(alpha)  *(1-beta) +
                                                 inputArray.at<cv::Vec3b>(ceilValH,  floorValW)[k]*(1-alpha)*(beta)   +
                                                 inputArray.at<cv::Vec3b>(ceilValH,  ceilValW)[k] *(alpha)  *(beta);
                }
            }
        }
    }

    outputArray.release();
    outputArray = dst.clone();
    dst.release();
}

void myCV::wave(cv::Mat &inputArray, cv::Mat &outputArray, double frequency, int wave_magnitude)
{
    const int padded = wave_magnitude;
    int i, k;

    cv::Mat&& temp = cv::Mat(inputArray.rows + wave_magnitude * 2, inputArray.cols, CV_8UC3, cv::Scalar(0,0,0)).clone();
    #pragma omp parallel for private(i, k)
    for(int j = 0; j < inputArray.rows; j++)
    {

        for(i = 0; i < inputArray.cols; i++)
        {
            int floorValH = wave_magnitude * sin(M_PI*i * frequency / 180.0);
            for(k = 0; k < 3; k++)
            {
                temp.at<cv::Vec3b>(j + floorValH + padded, i)[k] = inputArray.at<cv::Vec3b>(j, i)[k];
            }
        }
    }
    cv::Mat&& dst = cv::Mat(inputArray.rows + wave_magnitude * 2, inputArray.cols + wave_magnitude * 2, CV_8UC3, cv::Scalar(0,0,0)).clone();
    #pragma omp parallel for private(i, k)
    for(int j = 0; j < temp.rows; j++)
    {

        for(i = 0; i < temp.cols; i++)
        {
            int floorValW = wave_magnitude * sin(M_PI*j * frequency / 180.0);
            for(k = 0; k < 3; k++)
            {
                dst.at<cv::Vec3b>(j, i + floorValW + padded)[k] = temp.at<cv::Vec3b>(j, i)[k];
            }
        }
    }

    outputArray.release();
    outputArray = dst.clone();
    dst.release();
}

void myCV::fisheye(cv::Mat &inputArray, cv::Mat &outputArray)
{
    double diam = inputArray.rows > inputArray.cols ? inputArray.cols : inputArray.rows;
    int i, k;

    std::vector<std::vector<int>> circle(inputArray.rows, std::vector<int>(inputArray.cols, 0));
    double center[2] = {inputArray.cols/2.0, inputArray.rows/2.0};
    #pragma omp parallel for private(i)
    for(int j = 0; j < inputArray.rows; j++)
    {

        for(i = 0; i < inputArray.cols; i++)
        {
           if(sqrt(pow(i - center[0],2) + pow(j - center[1],2)) <= diam/2.0)
           {
               circle[j][i] = 1;
           }
        }
    }

    cv::Mat&& dst = cv::Mat(inputArray.rows, inputArray.cols, CV_8UC3, cv::Scalar(0,0,0)).clone();
    bool isInitial = false;
    double multiW;
    #pragma omp parallel for private(i, k) firstprivate(isInitial, multiW)
    for(int j = 0; j < inputArray.rows; j++)
    {
        isInitial = false;
        for(i = 0; i < inputArray.cols; i++)
        {
            if(circle[j][i] == 1)
            {
                if(!isInitial)
                {
                    multiW = center[0] - i == 0 ? 0 : (center[0]-1.0) / abs(center[0] - i);
                    isInitial = true;
                }
                int&& floorValW = center[0] + std::floor((i - center[0]) * multiW);
                for(k = 0; k < 3; k++)
                {
                    dst.at<cv::Vec3b>(j, i)[k] = inputArray.at<cv::Vec3b>(j, floorValW)[k];
                }
            }
        }
    }

    outputArray.release();
    outputArray = dst.clone();
    dst.release();
}

void myCV::HoughLineDetection(cv::Mat &inputArray, cv::Mat &outputArray, int line_threshold)
{
    cv::Mat edge;

    //Use OTSU to find threshold value
    myThreshold(inputArray, edge);
#ifdef _DEBUG
    cv::imshow("otsu threshold", edge);
#endif
    //Do laplacian
    laplacianFilter(edge, edge);

    //Create a line map with possible max distance
    int D = sqrt(pow(inputArray.rows, 2) + pow(inputArray.cols, 2));
    cv::Mat line_map(D * 2, 180, CV_32FC1, cv::Scalar(0));
#ifdef _DEBUG
    cv::imshow("edge result", edge);
#endif
    //Counting weight map
    int k, i;
    for(int j = 0; j < edge.rows; j++)
    {

        for(i = 0; i < edge.cols; i++)
        {
           if(edge.at<uchar>(j, i) == 255)
           {
               for(k = 0; k < 180; k++)
               {
                   int magnitude = D + (double)(i) * cos((k-90.0) * M_PI / 180) + (double)(j) * sin((k-90.0) * M_PI / 180);
                   if(magnitude >= 0 && magnitude < line_map.rows)
                        line_map.at<float>(magnitude, k)++;
               }
           }
        }
    }

    //Find local maximun solutions.
    std::vector<cv::Point3i> lines(0);
    int a;
    bool flag = true;
    for(int j = 0; j < line_map.rows; j++)
    {

        for(i = 0; i < line_map.cols; i++)
        {
            if(line_map.at<float>(j, i) > line_threshold)
            {
                flag = true;
                if(lines.empty())
                {
                    lines.push_back(cv::Point3i(i, j, (int)line_map.at<float>(j, i)));
                }
                else
                {
                    for(a = 0; a < lines.size(); a++)
                    {
                        if((pow(lines[a].y - j, 2) + pow(lines[a].x - i, 2)) < 50)
                        {
                            if((int)line_map.at<float>(j, i) > lines[a].z)
                            {
                                lines[a].x = i;
                                lines[a].y = j;
                                lines[a].z = (int)line_map.at<float>(j, i);
                            }
                            flag = false;
                        }
                    }
                    if(flag)
                    {
                       lines.push_back(cv::Point3i(i, j, (int)line_map.at<float>(j, i)));
                    }
                }
            }
        }
    }

    cv::Mat&& dst = inputArray.clone();
    if(dst.type()== CV_8UC1){myCvtColor(dst, dst, GRAY2GBR);}

    //Draw lines with points
    for(i = 0; i < lines.size(); i++)
    {
        int&& height0 = ((double)(lines[i].y - D)) / sin((lines[i].x-90.0) * M_PI/180.0);
        int&& height = ((double)(lines[i].y - D) - ((dst.cols - 1) * cos((lines[i].x-90.0) * M_PI/180.0))) / sin((lines[i].x-90.0) * M_PI/180.0);
        cv::Point point1(0, height0), point2((dst.cols - 1), height);
        myLine(dst, point1, point2, cv::Scalar(0, 0, 255), 3);
    }

#ifdef _DEBUG
    cv::Mat line_map_8u;
    float min, max;
    findMinMax<float>(line_map, min, max);
    line_map.convertTo(line_map_8u, CV_8UC1, 255.0/(max-min));
    cv::imshow("map", line_map_8u);
    cv::imshow("result", dst);
#endif

    outputArray.release();
    outputArray = dst.clone();
    dst.release();
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
