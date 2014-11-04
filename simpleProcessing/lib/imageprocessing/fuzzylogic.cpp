#include "fuzzylogic.h"
using namespace myCV;

fuzzyLogic::fuzzyLogic()
{

}

void fuzzyLogic::getBoundaries(cv::Mat &inputArray, cv::Mat &outputArray)
{
    const int size = 3;
    cv::Mat tmp;
    if(inputArray.type() == CV_8UC3) {myCvtColor(inputArray, tmp, BGR2GRAY);}
    else {tmp = inputArray.clone();}
    cv::Mat&& dest = cv::Mat::zeros(tmp.size().height, tmp.size().width, CV_8UC1);

    if(fuzzy_func_type == BELLCURVE)
    {
        int i, memfunc;
        #pragma omp parallel for private(i) firstprivate(memfunc)
        for(int j = 0; j < tmp.size().height; j++)
            for(i = 0; i < tmp.size().width; i++)
            {
                if(tmp.at<uchar>(j-size / 2, i) > (Bell.mean - Bell.b) &&
                   tmp.at<uchar>(j-size / 2, i) < (Bell.mean + Bell.b))
                {
                    if(tmp.at<uchar>(j, i-size / 2) > (Bell.mean - Bell.b) &&
                       tmp.at<uchar>(j, i-size / 2) < (Bell.mean + Bell.b))
                    {
                        memfunc = bellCurve(tmp.at<uchar>(j, i), Bell.b, Bell.mean);
                    }
                    else if(tmp.at<uchar>(j, i+size / 2) > (Bell.mean - Bell.b) &&
                            tmp.at<uchar>(j, i+size / 2) < (Bell.mean + Bell.b))
                    {
                        memfunc = bellCurve(tmp.at<uchar>(j, i), Bell.b, Bell.mean);
                    }
                }
                else if(tmp.at<uchar>(j+size / 2, i) > (Bell.mean - Bell.b) &&
                        tmp.at<uchar>(j+size / 2, i) < (Bell.mean + Bell.b))
                {
                    if(tmp.at<uchar>(j, i-size / 2) > (Bell.mean - Bell.b) &&
                       tmp.at<uchar>(j, i-size / 2) < (Bell.mean + Bell.b))
                    {
                        memfunc = bellCurve(tmp.at<uchar>(j, i), Bell.b, Bell.mean);
                    }
                    else if(tmp.at<uchar>(j, i+size / 2) > (Bell.mean - Bell.b) &&
                            tmp.at<uchar>(j, i+size / 2) < (Bell.mean + Bell.b))
                    {
                        memfunc = bellCurve(tmp.at<uchar>(j, i), Bell.b, Bell.mean);
                    }
                }
                else
                {
                    memfunc = 0;
                }
                dest.at<uchar>(j,i) = inverseTriang(memfunc);
            }
        outputArray.release();
        outputArray = dest.clone();
        tmp.release();
        dest.release();
    }
}

double fuzzyLogic::sShape(int x, int a, int b, int c)
{
    if(x < a)
    {
        return 0;
    }
    else if(x <= b)
    {
        return 2*pow((x-a)/(c-a),2);
    }
    else if(x <= c)
    {
        return (1-2*pow((x-c)/(c-a),2));
    }
    else
        return 1;
}

void fuzzyLogic::setBellCurveProperties(double mean, double b)
{
    Bell.mean = mean;
    Bell.b = b;
}

double fuzzyLogic::bellCurve(int x, int b, int c)
{
    if(x <= c)
    {
        return sShape(x, c-b, c-(b/2),c);
    }
    else
    {
        return 1 - sShape(x, c, c+(b/2), c+b);
    }
}

void fuzzyLogic::setTriangProperties(int center, int left, int right)
{
    Triang.center = center;
    Triang.left = left;
    Triang.right = right;
}

uchar fuzzyLogic::inverseTriang(int memf)
{
    int&& ans = (1 - memf) * Triang.right;
    if(ans < 0)
        ans = 0;

    return ans;
}
