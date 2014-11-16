#ifndef SPECTRALFILTERING_H
#define SPECTRALFILTERING_H
#include "opencv2/opencv.hpp"
#include "../imageprocessing.h"
#include "../imgcore.h"
#include "fft.h"


namespace myCV{

class spectralFiltering
{
public:
    spectralFiltering(cv::Mat &img = cv::Mat());
    void feedImage(cv::Mat &img);

    cv::Mat getOriginSpectralReal(){return originSpectralReal;}
    cv::Mat getOriginSpectralImag(){return originSpectralImag;}
    cv::Mat getSpectralReal(){return spectralReal;}
    cv::Mat getSpectralImag(){return spectralImag;}
    void getResult(cv::Mat &img);
private:
    void moveSpectral2Center(cv::Mat &img);
    cv::Mat originImg;
    cv::Mat originSpectralReal, originSpectralImag;
    cv::Mat spectralReal, spectralImag;
};
}

#endif // SPECTRALFILTERING_H
