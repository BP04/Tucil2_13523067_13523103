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

double CalculateMeanAbsoluteDeviation(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int imageWidth)
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

double CalculateMaxPixelDifference(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int imageWidth)
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
            if (image[idx].r < minR)
                minR = image[idx].r;
            if (image[idx].r > maxR)
                maxR = image[idx].r;

            // Update min & max G
            if (image[idx].g < minG)
                minG = image[idx].g;
            if (image[idx].g > maxG)
                maxG = image[idx].g;

            // Update min & max B
            if (image[idx].b < minB)
                minB = image[idx].b;
            if (image[idx].b > maxB)
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
    // hitung frekuensi pixel
    vector<double> freqR(256, 0), freqG(256, 0), freqB(256, 0);
    for (int i = y; i < y + height; i++)
    {
        for (int j = x; j < x + width; j++)
        {
            int idx = i * imageWidth + j;
            freqR[image[idx].r]++;
            freqG[image[idx].g]++;
            freqB[image[idx].b]++;
        }
    }
    // hitung probabilitas pixel
    int totalPixels = width * height;
    double H = 0.0;
    for (int i = 0; i < 256; i++)
    {
        if (freqR[i] > 0)
        {
            double p = freqR[i] / totalPixels;
            H -= p * log2(p);
        }
        if (freqG[i] > 0)
        {
            double p = freqG[i] / totalPixels;
            H -= p * log2(p);
        }
        if (freqB[i] > 0)
        {
            double p = freqB[i] / totalPixels;
            H -= p * log2(p);
        }
    }

    return H / 3.0;
}

double CalculateSSIM(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int imageWidth)
{
    
    return 0.0;
}
