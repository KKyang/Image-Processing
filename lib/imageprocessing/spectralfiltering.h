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
    spectralFiltering(cv::Mat &img = cv::Mat(), bool isColor = false);
    void feedImage(cv::Mat &img, bool isColor = false);
    void computeFFT();

    cv::Mat getFilter(){return filter;}
    cv::Mat getSpectralReal(){return spectral[0].real;}
    cv::Mat getSpectralImag(){return spectral[0].imag;}
    void getResult(cv::Mat &img);
    void changeColorMode(bool isColor);


    //filters
    void noFilter();
    void genLowPassFilter(int filter_algorithm, const int threshold);
    void genHighPassFilter(int filter_algorithm, const int threshold);

    //set algorithm properties
    void setHomomorphic(float high, float low);
    void setButterworth(int n){butterworth = n;}
private:
    void initial(bool isColor);
    void moveSpectral2Center(cv::Mat &img);
    cv::Mat originImg;
    cv::Mat HSV;
    struct eigen{
        cv::Mat real;
        cv::Mat imag;
    };
    std::vector<eigen> originSpectral;
    std::vector<eigen> spectral;

    cv::Mat filter;

    struct{
        float high;
        float low;
    }homomorphic;
    //algorithm properties
    int butterworth = 2;

    bool colorMode = false;
    int channel = 1;
};
}

#endif // SPECTRALFILTERING_H
