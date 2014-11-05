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
                        memfunc = bellCurve(left_d, Bell.b, Bell.mean);
                    }
                    else if(tiplus != i && right_d > (lThresh) && right_d < (rThresh))
                    {
                        memfunc = bellCurve(right_d, Bell.b, Bell.mean);
                    }
                }
                else if(tjplus !=j && down_d > (lThresh) && down_d < (rThresh))
                {
                    if(timinus != i && left_d > (lThresh) && left_d < (rThresh))
                    {
                        memfunc = bellCurve(left_d, Bell.b, Bell.mean);
                    }
                    else if(tiplus != i && right_d > (lThresh) && right_d < (rThresh))
                    {
                        memfunc = bellCurve(right_d, Bell.b, Bell.mean);
                    }
                }
                else
                {
                    memfunc = 0;
                }

                if(memfunc == 0)
                    dest.at<uchar>(j,i) = 200;
                else
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

void fuzzyLogic::setTriangProperties(int center, int left, int right)
{
    Triang.center = center;
    Triang.left = center - left;
    Triang.right = center + right;
}

uchar fuzzyLogic::inverseTriang(double memf)
{
    int&& ans = memf * (255-Triang.left) + Triang.left;
    if(ans < 0)
        ans = 0;

    return ans;
}
