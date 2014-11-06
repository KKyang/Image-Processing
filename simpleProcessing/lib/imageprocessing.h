#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H
#define _USE_MATH_DEFINES
#include <cmath>
#include "opencv2/opencv.hpp"

namespace myCV{

//This enum stores cvtColor types
enum{
    BGR2GRAY = 1,
    GRAY2GBR,
    BGR2YCbCr,
    YCbCr2BGR
};
//This enum stores gray cvtColor types.
enum{
    GRAY_AVG = 1,
    GRAY_LUMI
};

void customFilter(cv::Mat &inputArray, cv::Mat &outputArray, int mask_w, int mask_h, std::vector<int> &mask);

//Find the pixel appears the most.
int findHistLargestPos(std::vector<std::vector<int>> &data);

void grayResolutionScale(cv::Mat &inputArray, cv::Mat &outputArray, int scaleLevel);

void laplacianFilter(cv::Mat &inputArray, cv::Mat &outputArray, const int size = 3);

void medianFilter(cv::Mat &inputArray, cv::Mat &outputArray, const int size);

//Scale an image contrast by using stretch-based method.
void myContrast(cv::Mat &inputArray, cv::Mat &outputArray, int min, int max, bool ifauto = false);

//Chnage images color plane.
void myCvtColor(cv::Mat &inputArray, cv::Mat &outputArray, int colorType, int grayType=2);

//Simple Threshold
void myThreshold(cv::Mat &inputArray, cv::Mat &outputArray);

void myThreshold(cv::Mat &inputArray, cv::Mat &outputArray, unsigned int _threshold, unsigned _min, unsigned _max);

//find histogram
void histogram(cv::Mat &inputArray, cv::Mat &histogram);

void histogram(cv::Mat &inputArray, cv::Mat &histogram, std::vector<std::vector<int>> &data);

// Histogram equalization.
void EqualizeHist(cv::Mat &inputArray, cv::Mat &outputArray);

void sobelFilter(cv::Mat &inputArray, cv::Mat &outputArray);

class Blur
{
public:
    static void simple(cv::Mat &inputArray, cv::Mat &outputArray, const int _ksize);
    static void Gaussian(cv::Mat &inputArray, cv::Mat &outputArray, const int _ksize, const float sigmaX, float sigmaY);
};

}



#endif // IMAGEPROCESSING_H
