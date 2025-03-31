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

    double minR = 255, maxR = 0;
    double minG = 255, maxG = 0;
    double minB = 255, maxB = 0;

    for (int i = y; i < y + height; i++)
    {
        for (int j = x; j < x + width; j++)
        {
            int idx = i * imageWidth + j;

            // Update min & max R
            if (pix.r < minR)
                minR = image[idx].r;
            if (pix.r > maxR)
                maxR = image[idx].r;

            // Update min & max G
            if (pix.g < minG)
                minG = image[idx].g;
            if (pix.g > maxG)
                maxG = image[idx].g;

            // Update min & max B
            if (pix.b < minB)
                minB = image[idx].b;
            if (pix.b > maxB)
                maxB = image[idx].b;
        }
    }

    double diffR = maxR - minR;
    double diffG = maxG - minG;
    double diffB = maxB - minB;
    double diffRGB = (diffR + diffG + diffB) / 3.0;
    return diffRGB;
}

double CalculateEntropy(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int imageWidth)
{
    return 0.0;
}
double CalculateSSIM(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int imageWidth)
{
    return 0.0;
}
