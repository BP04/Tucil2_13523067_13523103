#ifndef METRICS_HPP
#define METRICS_HPP

#include "QuadTree.hpp"
#include <algorithm>

double CalculateVariance(const std::vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int &imageWidth);
double CalculateMeanAbsoluteDeviation(const std::vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int &imageWidth);
double CalculateMaxPixelDifference(const std::vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int &imageWidth);
double CalculateEntropy(const std::vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int &imageWidth);
double CalculateSSIM(const std::vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int &imageWidth);

#endif