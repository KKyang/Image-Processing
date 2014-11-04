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
        int i, memfunc;
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
                        memfunc = 255;
                    }
                    else if(tiplus != i && right_d > (lThresh) && right_d < (rThresh))
                    {
                        memfunc = 255;
                    }
                }
                else if(tjplus !=j && down_d > (lThresh) && down_d < (rThresh))
                {
                    if(timinus != i && left_d > (lThresh) && left_d < (rThresh))
                    {
                        memfunc = 255;
                    }
                    else if(tiplus != i && right_d > (lThresh) && right_d < (rThresh))
                    {
                        memfunc = 255;
                    }
                }
                else
                {
                    memfunc = 0;
                }

                dest.at<uchar>(j,i) = memfunc;
                //dest.at<uchar>(j,i) = inverseTriang(memfunc);
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
