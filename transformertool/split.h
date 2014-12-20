#ifndef SPLIT_H
#define SPLIT_H
#include "opencv2/opencv.hpp"
#include "imageprocessing.h"
#include "imgcore.h"

void split3(cv::Mat &img, int colorType, std::vector<cv::Mat> &img_set);

#endif // SPLIT_H
