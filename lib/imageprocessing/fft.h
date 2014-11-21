#ifndef FFT_H
#define FFT_H
#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <complex>
#include <math.h>
#include "imageprocessing.h"
#include "imgcore.h"
#include "opencv2/opencv.hpp"
#include <vector>
#include <iostream>

namespace myCV{
    void FFT2D(cv::Mat &inputArray, cv::Mat &outputArray_real, cv::Mat &outputArray_imag);
    void iFFT2D(cv::Mat &inputArray_real, cv::Mat &inputArray_imag, cv::Mat &outputArray, const int width, const int height);
    void iFFT2DHomo(cv::Mat &inputArray_real, cv::Mat &inputArray_imag, cv::Mat &outputArray, const int width, const int height);
    template <typename T>
    void FFT(std::vector<T> &real, std::vector<T> &imag, int power, bool reverse=false);
}

template <typename T>
void myCV::FFT(std::vector<T> &real, std::vector<T> &imag, int power, bool reverse)
{
    int ifft = reverse ? 1 : -1;
    int n = pow(2,power);

    //Bit reverse
    int half_of_n;
    int swap_index = 0;

    for (int i = 0; i < n - 1; i++)
    {
        if (i < swap_index)
        {
            auto tempR = real[i];
            auto tempI = imag[i];
            real[i] = real[swap_index];
            imag[i] = imag[swap_index];
            real[swap_index] = tempR;
            imag[swap_index] = tempI;
        }
        half_of_n = n >> 1;
        while (half_of_n <= swap_index)
        {
            swap_index -= half_of_n;
            half_of_n >>= 1;
        }
        swap_index += half_of_n;
    }


    int step = 1, max = 1;
    double r_coff, i_coff;
    double cosarg = -1.0, sinarg = 0.0;

    for (int m = 0; m < power; m++)
    {
        max = step;
        step <<= 1;
        r_coff = 1.0;
        i_coff = 0.0;
        for (int j = 0; j < max; j++)
        {
            for (int i = j; i < n; i += step)
            {
                int&& mirror_index = i + max;
                double&& tempR = r_coff*real[mirror_index] - i_coff*imag[mirror_index];
                double&& tempI = r_coff*imag[mirror_index] + i_coff*real[mirror_index];

                real[mirror_index] = real[i] - tempR;
                imag[mirror_index] = imag[i] - tempI;
                real[i] += tempR;
                imag[i] += tempI;
            }
            double r_coff_temp;
            r_coff = (r_coff_temp = r_coff) * cosarg - i_coff * sinarg;
            i_coff = i_coff * cosarg + r_coff_temp * sinarg;
        }
        sinarg = ifft*sqrt((1.0 - cosarg) / 2.0);
        cosarg = sqrt((1.0 + cosarg) / 2.0);
    }

    if (!reverse)
    {
        #pragma omp parallel for
        for (int i = 0; i<n; i++) {
            real[i] /= (double)n;
            imag[i] /= (double)n;
        }
    }

}



#endif // FFT_H
