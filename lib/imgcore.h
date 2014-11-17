#ifndef IMGCORE_H
#define IMGCORE_H
#include "opencv2/opencv.hpp"

namespace myCV{
enum{
    NEAREST = 1,
    BILINEAR
};
//Resize image.
void myResize(cv::Mat &inputArray, cv::Mat &outputArray, const unsigned int _width, unsigned int _height,
              const bool _aspectRatioFix = 1, const int method = 2);

class imgCore
{

public:
    imgCore();


private:

};
}

#endif // IMGCORE_H
