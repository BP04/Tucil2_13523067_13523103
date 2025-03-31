#include "Metrics.hpp"

double CalculateVariance(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int imageWidth)
{
    double variance = 0.0;
    for (int i = y; i < y + height; i++)
    {
        for (int j = x; j < x + width; j++)
        {
            int idx = i * imageWidth + j;
            variance += pow(image[idx].r - avgColor.r, 2) +
                        pow(image[idx].g - avgColor.g, 2) +
                        pow(image[idx].b - avgColor.b, 2);
        }
    }
    return variance / (3 * width * height);
}

double MeanAbsoluteDeviation(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int imageWidth)
{
    double mad = 0.0;
    for (int i = y; i < y + height; i++)
    {
        for (int j = x; j < x + width; j++)
        {
            int idx = i * imageWidth + j;
            mad += abs(image[idx].r - avgColor.r) +
                   abs(image[idx].g - avgColor.g) +
                   abs(image[idx].b - avgColor.b);
        }
    }
    return mad / (3 * width * height);
}

double MaxPixelDifference(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int imageWidth)
{

    uint8_t minR = 255, maxR = 0;
    uint8_t minG = 255, maxG = 0;
    uint8_t minB = 255, maxB = 0;

    for (int i = y; i < y + height; i++)
    {
        for (int j = x; j < x + width; j++)
        {
            int idx = i * imageWidth + j;
            const auto &pix = image[idx];

            // Update min & max R
            if (pix.r < minR)
                minR = pix.r;
            if (pix.r > maxR)
                maxR = pix.r;

            // Update min & max G
            if (pix.g < minG)
                minG = pix.g;
            if (pix.g > maxG)
                maxG = pix.g;

            // Update min & max B
            if (pix.b < minB)
                minB = pix.b;
            if (pix.b > maxB)
                maxB = pix.b;
        }
    }

    double dR = static_cast<double>(maxR) - static_cast<double>(minR);
    double dG = static_cast<double>(maxG) - static_cast<double>(minG);
    double dB = static_cast<double>(maxB) - static_cast<double>(minB);
    double dRGB = (dR + dG + dB) / 3.0;
    return dRGB;
}

double CalculateEntropy(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int imageWidth)
{
    return 0.0;
}
double CalculateSSIM(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int imageWidth)
{
    return 0.0;
}