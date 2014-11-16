#ifndef SPECTRALFILTERING_H
#define SPECTRALFILTERING_H
#define _USE_MATH_DEFINES
#include <math.h>
#include "opencv2/opencv.hpp"
#include "../imageprocessing.h"
#include "../imgcore.h"
#include "fft.h"


namespace myCV{

enum FilterAlgorithm{
    ideal = 0,
    butterworth,
    gaussian
};
class spectralFiltering
{
public:
    spectralFiltering(cv::Mat &img = cv::Mat());
    void feedImage(cv::Mat &img);
    void computeFFT();

    cv::Mat getFilter(){return filter;}
    cv::Mat getSpectralReal(){return spectral.real;}
    cv::Mat getSpectralImag(){return spectral.imag;}
    void getResult(cv::Mat &img);


    //filters
    void noFilter();
    void genLowPassFilter(int filter_algorithm, const int threshold);
    void genHighPassFilter(int filter_algorithm, const int threshold);

    //set algorithm properties
    void setButterworth(int n){butterworth = n;}
private:
    void moveSpectral2Center(cv::Mat &img);
    cv::Mat originImg;
    struct{
        cv::Mat real;
        cv::Mat imag;
    }originSpectral, spectral;
    cv::Mat filter;

    //algorithm properties
    int butterworth = 2;
};
}

#endif // SPECTRALFILTERING_H
