#ifndef LINEMATH_H
#define LINEMATH_H
#include "opencv2/opencv.hpp"

inline double Dot(const cv::Point& a, const cv::Point& b){ return (a.x*b.x) + (a.y*b.y); }
inline double PerpDot(const cv::Point& a, const cv::Point& b){ return (a.y*b.x) - (a.x*b.y); }

bool LineCollision(const cv::Point&  A1, const cv::Point& A2,
    const cv::Point& B1, const cv::Point& B2,
    cv::Point &result)
{
    cv::Point a(A2 - A1);
    cv::Point b(B2 - B1);

	double f = PerpDot(a, b);
	if (!f)      // lines are parallel
		return false;

    cv::Point c(B2 - A2);
	double aa = PerpDot(a, c);
	double bb = PerpDot(b, c);

	if (f < 0)
	{
		if (aa > 0)     return false;
		if (bb > 0)     return false;
		if (aa < f)     return false;
		if (bb < f)     return false;
	}
	else
	{
		if (aa < 0)     return false;
		if (bb < 0)     return false;
		if (aa > f)     return false;
		if (bb > f)     return false;
	}

    result = ((B2 - B1) * (1.0 - (aa / f)) + B1);
	return true;
}

#endif
