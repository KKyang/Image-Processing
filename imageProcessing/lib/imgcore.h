#ifndef IMGCORE_H
#define IMGCORE_H
#include "opencv2/opencv.hpp"

namespace myCV{

void myResize(cv::Mat &inputArray, cv::Mat &outputArray, const unsigned int _width, unsigned int _height, const bool _aspectRatioFix = 1);

class imgCore
{

public:
    imgCore();


private:

};
}

#endif // IMGCORE_H
