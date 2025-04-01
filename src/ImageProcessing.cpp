#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "QuadTree.hpp"
#include "Metrics.hpp"
#include "ImageLoadException.hpp"

#include <iostream>
#include <sys/stat.h>
#include <chrono>
#include <jpeglib.h>
#include <jerror.h>
#include <fstream>
#include <string>

std::vector<RGBPixel> LoadImage(std::string fileName, int &width, int &height) {
    int channels;
    unsigned char* image_data = stbi_load(fileName.c_str(), &width, &height, &channels, 0);

    if (!image_data) {
        throw ImageLoadException("Can't open file: " + fileName);
    }

    std::vector<RGBPixel> pixels;
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int index = (i * width + j) * channels;

            uint8_t r = image_data[index + 0];
            uint8_t g = image_data[index + 1];
            uint8_t b = image_data[index + 2];

            pixels.emplace_back(RGBPixel(r, g, b));
        }
    }

    return pixels;
}

void SaveImage(std::string fileName, const std::vector<RGBPixel> &image, int width, int height) {
    std::vector<uint8_t> rawData;
    rawData.reserve(width * height * 3);

    for (const auto& pixel : image) {
        rawData.push_back(pixel.r);
        rawData.push_back(pixel.g);
        rawData.push_back(pixel.b);
    }

    std::string ext = fileName.substr(fileName.find_last_of('.') + 1);
    for (int i = 0; ext[i] != '\0'; ++i) {
        if ('A' <= ext[i] && ext[i] <= 'Z') {
            ext[i] += 32;
        }
    }

    bool success = false;

    if (ext == "png") {
        success = stbi_write_png(fileName.c_str(), width, height, 3, rawData.data(), width * 3);
    }
    else if (ext == "jpg" || ext == "jpeg") {
        int quality = 100;
        success = stbi_write_jpg(fileName.c_str(), width, height, 3, rawData.data(), quality);
    }
    else {
        std::cerr << "Unsupported file extension: ." << ext << std::endl;
        return;
    }

    if (success) {
        std::cout << "Saved image to " << fileName << std::endl;
    } else {
        std::cerr << "Failed to save image: " << fileName << std::endl;
    }
}

RGBPixel CalculateAverageColor(const std::vector<RGBPixel> &image, int x, int y, int width, int height, int imageWidth) {
    uint32_t r = 0, g = 0, b = 0;
    for (int i = y; i < y + height; i++)
    {
        for (int j = x; j < x + width; j++)
        {
            int idx = i * imageWidth + j;
            r += image[idx].r;
            g += image[idx].g;
            b += image[idx].b;
        }
    }
    int totalPixel = width * height;
    return RGBPixel((u_int8_t)(r / totalPixel), (u_int8_t)(g / totalPixel), (u_int8_t)(b / totalPixel));
}

std::unique_ptr<QuadTreeNode> BuildQuadTree(std::vector<RGBPixel> &image, int x, int y, int w, int h, double threshold, int minBlockSize, int errorMeasurementChoice, int imageWidth) {
    RGBPixel avgColor = CalculateAverageColor(image, x, y, w, h, imageWidth);
    double error;

    switch (errorMeasurementChoice)
    {
    case 1:
        // Variance
        error = CalculateVariance(image, x, y, w, h, avgColor, imageWidth);
        break;
    case 2:
        // Mean Absolute Deviation (MAD)
        error = CalculateMeanAbsoluteDeviation(image, x, y, w, h, avgColor, imageWidth);
        break;
    case 3:
        // Max Pixel Difference
        error = CalculateMaxPixelDifference(image, x, y, w, h, avgColor, imageWidth);
        break;

    case 4:
        // Entropy (rentang 0-8)
        error = CalculateEntropy(image, x, y, w, h, avgColor, imageWidth);
        break;

    case 5:
        // SSIM (rentang 0-1)
        double ssim = CalculateSSIM(image, x, y, w, h, avgColor, imageWidth);
        error = 1.0 - ssim;
        break;
    }

    if (error < threshold || (w * h) < minBlockSize)
    {
        return std::make_unique<QuadTreeNode>(x, y, w, h, avgColor, true);
    }

    int halfWidth = w / 2;
    int remWidth = w - halfWidth;
    int halfHeight = h / 2;
    int remHeight = h - halfHeight;

    auto node = std::make_unique<QuadTreeNode>(x, y, w, h, avgColor, false);
    node->atasKiri = BuildQuadTree(image, x, y, halfWidth, halfHeight, threshold, minBlockSize, errorMeasurementChoice, imageWidth);
    node->atasKanan = BuildQuadTree(image, x + halfWidth, y, remWidth, halfHeight, threshold, minBlockSize, errorMeasurementChoice, imageWidth);
    node->bawahKiri = BuildQuadTree(image, x, y + halfHeight, halfWidth, remHeight, threshold, minBlockSize, errorMeasurementChoice, imageWidth);
    node->bawahKanan = BuildQuadTree(image, x + halfWidth, y + halfHeight, remWidth, remHeight, threshold, minBlockSize, errorMeasurementChoice, imageWidth);
    return node;
}

void reconstructImage(std::vector<RGBPixel> &outputImage, std::unique_ptr<QuadTreeNode> &node, int imageWidth) {
    if (!node)
    {
        throw ImageLoadException("Node is null");
    }

    if (node->isLeaf)
    {
        for (int i = 0; i < node->height; i++)
        {
            for (int j = 0; j < node->width; j++)
            {
                int idx = (node->y + i) * imageWidth + (node->x + j);
                outputImage[idx] = node->color;
            }
        }
        return;
    }

    reconstructImage(outputImage, node->atasKiri, imageWidth);
    reconstructImage(outputImage, node->atasKanan, imageWidth);
    reconstructImage(outputImage, node->bawahKiri, imageWidth);
    reconstructImage(outputImage, node->bawahKanan, imageWidth);
}

double CalculateCompressionRatio(const std::string &uncompressedFile, const std::string &compressedFile) {
    struct stat uncompressedStat, compressedStat;
    if (stat(uncompressedFile.c_str(), &uncompressedStat) != 0 ||
        stat(compressedFile.c_str(), &compressedStat) != 0)
    {
        throw ImageLoadException("Cannot get file size: " + uncompressedFile + " or " + compressedFile);
    }

    double uncompressedSize = uncompressedStat.st_size;
    double compressedSize = compressedStat.st_size;
    std::cout << "Uncompressed Size: " << uncompressedSize << " bytes" << std::endl;
    std::cout << "Compressed Size: " << compressedSize << " bytes" << std::endl;
    return (1 - (compressedSize / uncompressedSize)) * 100.0;
}

void TraverseTree(std::unique_ptr<QuadTreeNode> &node, int currdepth, int &maxDepth, int &numLeafs) {
    if (!node)
    {
        throw ImageLoadException("Node is null");
    }

    if (currdepth > maxDepth)
    {
        maxDepth = currdepth;
    }

    if (node->isLeaf)
    {
        numLeafs++;
        return;
    }

    TraverseTree(node->atasKiri, currdepth + 1, maxDepth, numLeafs);
    TraverseTree(node->atasKanan, currdepth + 1, maxDepth, numLeafs);
    TraverseTree(node->bawahKiri, currdepth + 1, maxDepth, numLeafs);
    TraverseTree(node->bawahKanan, currdepth + 1, maxDepth, numLeafs);
}

int main()
{
    try
    {
        int maxDepth = 0, numLeafs = 0;
        int width, height;

        std::vector<RGBPixel> image = LoadImage("test/saiba.jpeg", width, height);
        auto root = BuildQuadTree(image, 0, 0, width, height, 10, 100, 3, width);
        TraverseTree(root, 0, maxDepth, numLeafs);
        std::vector<RGBPixel> outputImage(width * height);

        reconstructImage(outputImage, root, width);
        SaveImage("saiba_hasil.jpeg", outputImage, width, height);

        double compressionRatio = CalculateCompressionRatio("saiba.jpeg", "saiba_hasil.jpeg");
        std::cout << "Compression Ratio: " << compressionRatio << "%" << std::endl;
        std::cout << "Max Depth: " << maxDepth << std::endl;
        std::cout << "Number of Leafs: " << numLeafs << std::endl;
    }
    catch (const ImageLoadException &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}