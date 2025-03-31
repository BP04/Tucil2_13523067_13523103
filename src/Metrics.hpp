#ifndef METRICS_HPP
#define METRICS_HPP

#include "QuadTree.hpp"

double CalculateVariance(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int imageWidth);
double CalculateMeanAbsoluteDeviation(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int imageWidth);
double CalculateMaxPixelDifference(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int imageWidth);
double CalculateEntropy(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int imageWidth);
double CalculateSSIM(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int imageWidth);
#endif