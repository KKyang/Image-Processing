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
    spectralFiltering(cv::Mat &img, bool isColor,  bool isHomomorphic);
    void feedImage(cv::Mat &img, bool isColor = false, bool isHomomorphic = false);
    void computeFFT();

    cv::Mat getFilter(){return filter;}
    cv::Mat getSpectralIntensity();
    cv::Mat getSpectralReal(){return spectral[0].real;}
    cv::Mat getSpectralImag(){return spectral[0].imag;}
    void getResult(cv::Mat &img);
    void changeColorMode(bool isColor);
    void changeGHPFMode(bool status);


    //filters
    void noFilter();
    void genLowPassFilter(int filter_algorithm, const int threshold);
    void genHighPassFilter(int filter_algorithm, const int threshold);

    //set algorithm properties
    void setHomomorphic(float high, float low, float c);
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
        float high = 1.0;
        float low = 0.0;
        float c = 1.0;
    }homomorphic;
    struct{
        int threshold = 0;
        int type = 0;     //0 - no filter, 1 - low, 2 - high
    }filter_pro;
    //algorithm properties
    int butterworth = 2;

    bool colorMode = false;
    bool GHPF = false;
    int channel = 1;
};
}

#endif // SPECTRALFILTERING_H
