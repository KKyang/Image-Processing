#ifndef FUZZYLOGIC_H
#define FUZZYLOGIC_H
#define _USE_MATH_DEFINES
#include <cmath>
#include "../imageprocessing.h"
#include "opencv2/opencv.hpp"
#include <vector>

namespace myCV{
class fuzzyLogic
{
public:
    enum{
        BELLCURVE = 1
    };
    fuzzyLogic();
    void setFuzzyFunctionType(int type = 1){fuzzy_func_type = type;}
    void setOutput(std::vector<int> &output){fuzzy_logic_thresh = output;}
    void getBoundaries(cv::Mat &inputArray, cv::Mat &outputArray);


    double sShape(int x, int a, int b, int c);
    void setBellCurveProperties(double mean, double b);
    double bellCurve(int x, int b, int c);

    void setTriangWHProperties(int center, int left, int right);
    void setTriangBLProperties(int center, int left, int right);
    inline double triangular(int x){}
    uchar inverseTriangWH(double memf);
    uchar inverseTriangBL(double memf);
private:
    int fuzzy_func_type = 1;
    std::vector<int> fuzzy_logic_thresh;

    struct line
    {
        double center;
        double left;
        double right;
    }TriangWH, TriangBL;

    struct curve
    {
        double mean;
        double b;
    }Bell;
};
}

#endif // FUZZYLOGIC_H
