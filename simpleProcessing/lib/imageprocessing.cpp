#include "imageprocessing.h"
#include <omp.h>
#include <iostream>
using namespace myCV;

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
    if(colorType == 1)  //BGR2GRAY
    {
        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC1);

        if(grayType == 2)  //Using luminus
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
        else if(grayType == 1) //Using average
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
    else if(colorType == 2) //GRAY2BGR. Copy one channel three times.
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
    else if(colorType == 3) //BGR2YCbCr (YUV). Formula at http://en.wikipedia.org/wiki/YCbCr#JPEG_conversion
    {
        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC3);

        int i = 0;

        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width; i++)
            {
                tmp.at<cv::Vec3b>(j,i)[0] =   inputArray.at<cv::Vec3b>(j,i)[0] * 0.114 +
                                              inputArray.at<cv::Vec3b>(j,i)[1] * 0.587 +
                                              inputArray.at<cv::Vec3b>(j,i)[2] * 0.299;
                tmp.at<cv::Vec3b>(j,i)[1] =   inputArray.at<cv::Vec3b>(j,i)[0] * 0.5 -
                                              inputArray.at<cv::Vec3b>(j,i)[1] * 0.331264 -
                                              inputArray.at<cv::Vec3b>(j,i)[2] * 0.168736 + 128;
                tmp.at<cv::Vec3b>(j,i)[2] = -(inputArray.at<cv::Vec3b>(j,i)[0] * 0.081312) -
                                              inputArray.at<cv::Vec3b>(j,i)[1] * 0.418688 +
                                              inputArray.at<cv::Vec3b>(j,i)[2] * 0.5 + 128;
            }

        outputArray.release();
        outputArray = tmp.clone();
        tmp.release();
    }
    else if(colorType == 4) //YCbCr2BGR
    {
        cv::Mat&& tmp = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC3);

        int i = 0;
        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width;i++)
            {
                auto&& b = inputArray.at<cv::Vec3b>(j,i)[0] +
                           1.402   * (inputArray.at<cv::Vec3b>(j,i)[2] - 128);
                auto&& g = inputArray.at<cv::Vec3b>(j,i)[0] -
                           0.34414 * (inputArray.at<cv::Vec3b>(j,i)[1] - 128) -
                           0.71414 * (inputArray.at<cv::Vec3b>(j,i)[2] - 128);
                auto&& r = inputArray.at<cv::Vec3b>(j,i)[0] +
                           1.772   * (inputArray.at<cv::Vec3b>(j,i)[1] - 128);

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
        cv::Mat&& YCrCb = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC3);
        cv::Mat&& Y = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC1);

        int i = 0;
        //RGB to YCbCr (YUV). Speed up by skipping some steps.
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

        histogram(Y, cv::Mat(), data); //get histogram data.

        for(int i = 0; i < 256; i++)   //get sum.
        {
            sum += data[0][i];
        }

        for(int i = 0; i < 256; i++)   //histogram equalization. - find T(r)
        {
            sig += data[0][i];
            Transform[i] = cvRound(255 * (double)sig/(double)sum);
        }

        cv::Mat&& dest = cv::Mat::zeros(inputArray.size().height, inputArray.size().width, CV_8UC3);
        //Draw a new image by adapting T(r) to old image.
        #pragma omp parallel for private(i)
        for(int j = 0; j < inputArray.size().height; j++)
            for(i = 0; i < inputArray.size().width;i++)
            {
                YCrCb.at<cv::Vec3b>(j,i)[0] =  Transform[Y.at<uchar>(j,i)];

                auto&& b = YCrCb.at<cv::Vec3b>(j,i)[0] +
                        1.402   * (YCrCb.at<cv::Vec3b>(j,i)[2] - 128);
                auto&& g = YCrCb.at<cv::Vec3b>(j,i)[0] -
                        0.34414 * (YCrCb.at<cv::Vec3b>(j,i)[1] - 128) -
                        0.71414 * (YCrCb.at<cv::Vec3b>(j,i)[2] - 128);
                auto&& r = YCrCb.at<cv::Vec3b>(j,i)[0] +
                        1.772   * (YCrCb.at<cv::Vec3b>(j,i)[1] - 128);

                b = b < 0   ? 0   : b;
                b = b > 255 ? 255 : b;
                g = g < 0   ? 0   : g;
                g = g > 255 ? 255 : g;
                r = r < 0   ? 0   : r;
                r = r > 255 ? 255 : r;

                dest.at<cv::Vec3b>(j,i) = cv::Vec3b(r, g, b);
            }

        outputArray.release();
        outputArray = dest.clone();
        YCrCb.release();
        dest.release();
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

