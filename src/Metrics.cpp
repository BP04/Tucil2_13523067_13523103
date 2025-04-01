#include "Metrics.hpp"

double CalculateVariance(const std::vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int &imageWidth) {
    double variance = 0.0;

    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            int idx = i * imageWidth + j;
            variance += (image[idx].r - avgColor.r) * (image[idx].r - avgColor.r) +
                        (image[idx].r - avgColor.g) * (image[idx].r - avgColor.g) +
                        (image[idx].r - avgColor.b) * (image[idx].r - avgColor.b);
        }
    }

    return variance / (double)(3 * width * height);
}

double CalculateMeanAbsoluteDeviation(const std::vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int &imageWidth) {
    double mad = 0.0;
    
    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            int idx = i * imageWidth + j;
            mad += abs(image[idx].r - avgColor.r) +
                   abs(image[idx].g - avgColor.g) +
                   abs(image[idx].b - avgColor.b);
        }
    }
    
    return mad / (double)(3 * width * height);
}

double CalculateMaxPixelDifference(const std::vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int &imageWidth) {
    uint8_t minR = 255, maxR = 0;
    uint8_t minG = 255, maxG = 0;
    uint8_t minB = 255, maxB = 0;

    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            int idx = i * imageWidth + j;

            minR = std::min(minR, image[idx].r);
            maxR = std::max(maxR, image[idx].r);

            minG = std::min(minG, image[idx].g);
            maxG = std::max(maxG, image[idx].g);

            minB = std::min(minB, image[idx].b);
            maxB = std::max(maxB, image[idx].b);
        }
    }

    double diffR = (double)(maxR - minR);
    double diffG = (double)(maxG - minG);
    double diffB = (double)(maxB - minB);
    double diffRGB = (diffR + diffG + diffB) / 3.0;
    return diffRGB;
}

double CalculateEntropy(const std::vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int &imageWidth) {
    std::vector<int> freqR(256, 0), freqG(256, 0), freqB(256, 0);
    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            int idx = i * imageWidth + j;
            freqR[image[idx].r]++;
            freqG[image[idx].g]++;
            freqB[image[idx].b]++;
        }
    }
    
    int totalPixels = width * height;
    double H = 0.0;
    for (int i = 0; i < 256; i++) {
        if (freqR[i] > 0) {
            double p = (double)freqR[i] / (double)totalPixels;
            H -= p * log2(p);
        }
        if (freqG[i] > 0) {
            double p = (double)freqG[i] / (double)totalPixels;
            H -= p * log2(p);
        }
        if (freqB[i] > 0) {
            double p = (double)freqB[i] / (double)totalPixels;
            H -= p * log2(p);
        }
    }

    return H / 3.0;
}

double CalculateSSIM(const std::vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor, int &imageWidth) {
    int totalPixels = width * height;
    double sum1R = 0.0, sum1R2 = 0.0, sum12R = 0.0;
    double sum1G = 0.0, sum1G2 = 0.0, sum12G = 0.0;
    double sum1B = 0.0, sum1B2 = 0.0, sum12B = 0.0;

    double mean2R = (double)avgColor.r;
    double mean2G = (double)avgColor.g;
    double mean2B = (double)avgColor.b;

    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            int idx = i * imageWidth + j;
            double r1 = (double)image[idx].r;
            sum1R += r1;
            sum1R2 += r1 * r1;
            sum12R += r1 * mean2R;

            double g1 = (double)image[idx].g;
            sum1G += g1;
            sum1G2 += g1 * g1;
            sum12G += g1 * mean2G;

            double b1 = (double)image[idx].b;
            sum1B += b1;
            sum1B2 += b1 * b1;
            sum12B += b1 * mean2B;
        }
    }

    double mean1R = sum1R / (double)totalPixels;
    double mean1G = sum1G / (double)totalPixels;
    double mean1B = sum1B / (double)totalPixels;

    double var1R = sum1R2 / (double)totalPixels - mean1R * mean1R;
    double var1G = sum1G2 / (double)totalPixels - mean1G * mean1G;
    double var1B = sum1B2 / (double)totalPixels - mean1B * mean1B;

    double covR = sum12R / (double)totalPixels - mean1R * mean2R;
    double covG = sum12G / (double)totalPixels - mean1G * mean2G;
    double covB = sum12B / (double)totalPixels - mean1B * mean2B;

    double L_val = 255.0;
    double K1 = 0.01, K2 = 0.03;
    double C1 = (K1 * L_val) * (K1 * L_val);
    double C2 = (K2 * L_val) * (K2 * L_val);

    double ssimR = (2 * mean1R * mean2R + C1) * (2 * covR + C2) / ((mean1R * mean1R + mean2R * mean2R + C1) * (var1R + C2));
    double ssimG = (2 * mean1G * mean2G + C1) * (2 * covG + C2) / ((mean1G * mean1G + mean2G * mean2G + C1) * (var1G + C2));
    double ssimB = (2 * mean1B * mean2B + C1) * (2 * covB + C2) / ((mean1B * mean1B + mean2B * mean2B + C1) * (var1B + C2));

    return (ssimR + ssimG + ssimB) / 3.0;
}
