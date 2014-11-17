#include "fuzzylogic.h"
using namespace myCV;

fuzzyLogic::fuzzyLogic()
{

}

void fuzzyLogic::getBoundaries(cv::Mat &inputArray, cv::Mat &outputArray)
{
    cv::Mat tmp;
    if(inputArray.type() == CV_8UC3) {myCvtColor(inputArray, tmp, BGR2GRAY);}
    else {tmp = inputArray.clone();}
    cv::Mat&& dest = cv::Mat::zeros(tmp.size().height, tmp.size().width, CV_8UC1);

    if(fuzzy_func_type == BELLCURVE)
    {
        int&& lThresh = Bell.mean - Bell.b;
        int&& rThresh = Bell.mean + Bell.b;
        int i;
        double memfunc;
        #pragma omp parallel for private(i) firstprivate(memfunc)
        for(int j = 0; j < tmp.size().height; j++)
            for(i = 0; i < tmp.size().width; i++)
            {
                memfunc = 0;
                int&& tjplus  = (j + 1) >= tmp.size().height ? j : j+1;
                int&& tjminus = (j - 1) < 0 ? j : j-1;
                int&& tiplus  = (i + 1) >= tmp.size().width ? i : i+1;
                int&& timinus = (i - 1) < 0 ? i : i-1;
                auto&& up_d   = tmp.at<uchar>(tjminus, i) - tmp.at<uchar>(j,i);
                auto&& down_d = tmp.at<uchar>(tjplus, i)  - tmp.at<uchar>(j,i);
                auto&& left_d = tmp.at<uchar>(j, timinus) - tmp.at<uchar>(j,i);
                auto&& right_d= tmp.at<uchar>(j, tiplus)  - tmp.at<uchar>(j,i);

                if(tjminus != j && up_d > (lThresh) && up_d < (rThresh))
                {
                    if(timinus != i && left_d > (lThresh) && left_d < (rThresh))
                    {
                        memfunc += bellCurve(up_d, Bell.b, Bell.mean);
                        memfunc += bellCurve(left_d, Bell.b, Bell.mean);
                        dest.at<uchar>(j,i) = (inverseTriangWH(memfunc/2)*(memfunc/2) + inverseTriangBL(1.0-memfunc/2)*(1.0-memfunc/2));
                    }
                    else if(tiplus != i && right_d > (lThresh) && right_d < (rThresh))
                    {
                        memfunc += bellCurve(up_d, Bell.b, Bell.mean);
                        memfunc += bellCurve(right_d, Bell.b, Bell.mean);
                        dest.at<uchar>(j,i) = (inverseTriangWH(memfunc/2)*(memfunc/2) + inverseTriangBL(1.0-memfunc/2)*(1.0-memfunc/2));
                    }
                }
                else if(tjplus !=j && down_d > (lThresh) && down_d < (rThresh))
                {
                    if(timinus != i && left_d > (lThresh) && left_d < (rThresh))
                    {
                        memfunc += bellCurve(down_d, Bell.b, Bell.mean);
                        memfunc += bellCurve(left_d, Bell.b, Bell.mean);
                        dest.at<uchar>(j,i) = (inverseTriangWH(memfunc/2)*(memfunc/2) + inverseTriangBL(1.0-memfunc/2)*(1.0-memfunc/2));
                    }
                    else if(tiplus != i && right_d > (lThresh) && right_d < (rThresh))
                    {
                        memfunc += bellCurve(down_d, Bell.b, Bell.mean);
                        memfunc += bellCurve(right_d, Bell.b, Bell.mean);
                        dest.at<uchar>(j,i) = (inverseTriangWH(memfunc/2)*(memfunc/2) + inverseTriangBL(1.0-memfunc/2)*(1.0-memfunc/2));
                    }
                }
                else
                {
                    memfunc += bellCurve(up_d, Bell.b, Bell.mean);
                    memfunc += bellCurve(right_d, Bell.b, Bell.mean);
                    memfunc += bellCurve(down_d, Bell.b, Bell.mean);
                    memfunc += bellCurve(left_d, Bell.b, Bell.mean);

                    dest.at<uchar>(j,i) = (inverseTriangBL(memfunc/4)*(memfunc/4)+inverseTriangWH(1.0-memfunc/4)*(1.0-memfunc/4));

                }
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
    else if(x >= a && x <= b)
    {
        return 2*pow((double)(x-a)/(double)(c-a),2);
    }
    else if(x > b && x <= c)
    {
        return 1-2*pow((double)(x-c)/(double)(c-a),2);
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
        return sShape(x, c-b, c-((double)b/2),c);
    }
    else
    {
        return 1 - sShape(x, c, c+((double)b/2), c+b);
    }
}

void fuzzyLogic::setTriangWHProperties(int center, int left, int right)
{
    TriangWH.center = center;
    TriangWH.left = center - left;
    TriangWH.right = center + right;
}

void fuzzyLogic::setTriangBLProperties(int center, int left, int right)
{
    TriangBL.center = center;
    TriangBL.left = center - left;
    TriangBL.right = center + right;
}

uchar fuzzyLogic::inverseTriangWH(double memf)
{
    int&& ans = memf * (255-TriangWH.left) + TriangWH.left;
    if(ans < 0)
        ans = 0;
    else if(ans > 255)
        ans = 255;

    return ans;
}

uchar fuzzyLogic::inverseTriangBL(double memf)
{
    int&& ans = memf * TriangBL.right;
    if(ans < 0)
        ans = 0;
    else if(ans > 255)
        ans = 255;

    return ans;
}
