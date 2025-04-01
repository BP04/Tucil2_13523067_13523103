#include <iostream>
#include "QuadTree.hpp"
#include "Metrics.hpp"
#include <sys/stat.h>
#include <jpeglib.h>
#include <jerror.h>
#include <fstream>
#include <string>

vector<RGBPixel> LoadImage(const string &fileName, int &width, int &height)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *file = fopen(fileName.c_str(), "rb");

    if (!file)
    {
        throw ImageLoadException("Cannot open file: " + fileName);
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, file);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    width = cinfo.output_width;
    height = cinfo.output_height;
    int numChannels = cinfo.output_components;

    if (numChannels != 3)
    {
        jpeg_destroy_decompress(&cinfo);
        fclose(file);
        throw ImageLoadException("Only RGB are supported!!");
    }

    vector<RGBPixel> image(width * height);
    JSAMPROW row = new unsigned char[width * 3];

    while (cinfo.output_scanline < cinfo.output_height)
    {
        jpeg_read_scanlines(&cinfo, &row, 1);
        for (int x = 0; x < width; x++)
        {
            int idx = (cinfo.output_scanline - 1) * width + x;
            image[idx].r = row[x * 3];
            image[idx].g = row[x * 3 + 1];
            image[idx].b = row[x * 3 + 2];
        }
    }

    delete[] row;
    jpeg_finish_decompress(&cinfo);
    fclose(file);
    return image;
}

RGBPixel CalculateAverageColor(const vector<RGBPixel> &image, int x, int y, int width, int height, int imageWidth)
{
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

void SaveImage(const string &fileName, const vector<RGBPixel> &image, int width, int height)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *file = fopen(fileName.c_str(), "wb");

    if (!file)
    {
        throw ImageLoadException("Cannot open file: " + fileName);
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, file);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row = new unsigned char[width * 3];
    while (cinfo.next_scanline < cinfo.image_height)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = cinfo.next_scanline * width + x;
            row[x * 3] = image[idx].r;
            row[x * 3 + 1] = image[idx].g;
            row[x * 3 + 2] = image[idx].b;
        }
        jpeg_write_scanlines(&cinfo, &row, 1);
    }

    delete[] row;
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(file);
}

unique_ptr<QuadTreeNode> BuildQuadTree(vector<RGBPixel> &image, int x, int y, int w, int h, double threshold, int minBlockSize, int errorMeasurementChoice, int imageWidth)
{
    RGBPixel avgColor = CalculateAverageColor(image, x, y, w, h, imageWidth);
    double error;

    switch (errorMeasurementChoice)
    {
    case 1:
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
        return make_unique<QuadTreeNode>(x, y, w, h, avgColor, true);
    }

    int halfWidth = w / 2;
    int remWidth = w - halfWidth;
    int halfHeight = h / 2;
    int remHeight = h - halfHeight;

    auto node = make_unique<QuadTreeNode>(x, y, w, h, avgColor, false);
    node->atasKiri = BuildQuadTree(image, x, y, halfWidth, halfHeight, threshold, minBlockSize, errorMeasurementChoice, imageWidth);
    node->atasKanan = BuildQuadTree(image, x + halfWidth, y, remWidth, halfHeight, threshold, minBlockSize, errorMeasurementChoice, imageWidth);
    node->bawahKiri = BuildQuadTree(image, x, y + halfHeight, halfWidth, remHeight, threshold, minBlockSize, errorMeasurementChoice, imageWidth);
    node->bawahKanan = BuildQuadTree(image, x + halfWidth, y + halfHeight, remWidth, remHeight, threshold, minBlockSize, errorMeasurementChoice, imageWidth);
    return node;
}

void reconstructImage(vector<RGBPixel> &outputImage, unique_ptr<QuadTreeNode> &node, int imageWidth)
{
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
                outputImage[idx] = node->color; // Node Image pada Leaf hanya diambil average nya saja
            }
        }
        return;
    }

    reconstructImage(outputImage, node->atasKiri, imageWidth);
    reconstructImage(outputImage, node->atasKanan, imageWidth);
    reconstructImage(outputImage, node->bawahKiri, imageWidth);
    reconstructImage(outputImage, node->bawahKanan, imageWidth);
}

double CalculateCompressionRatio(const string &uncompressedFile, const string &compressedFile)
{
    struct stat uncompressedStat, compressedStat;
    if (stat(uncompressedFile.c_str(), &uncompressedStat) != 0 ||
        stat(compressedFile.c_str(), &compressedStat) != 0)
    {
        throw ImageLoadException("Cannot get file size: " + uncompressedFile + " or " + compressedFile);
    }

    double uncompressedSize = uncompressedStat.st_size;
    double compressedSize = compressedStat.st_size;
    cout << "Uncompressed Size: " << uncompressedSize << " bytes" << endl;
    cout << "Compressed Size: " << compressedSize << " bytes" << endl;
    return (1 - (compressedSize / uncompressedSize)) * 100.0;
}

int main()
{
    try
    {
        int width, height;
        vector<RGBPixel> image = LoadImage("misteri.jpeg", width, height);
        auto root = BuildQuadTree(image, 0, 0, width, height, 0.5, 100, 5, width);
        vector<RGBPixel> outputImage(width * height);

        reconstructImage(outputImage, root, width);
        SaveImage("misteri_hasil.jpeg", outputImage, width, height);

        double compressionRatio = CalculateCompressionRatio("misteri.jpeg", "misteri_hasil.jpeg");
        cout << "Compression Ratio: " << compressionRatio << "%" << endl;
    }
    catch (const ImageLoadException &e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}

// run sementara masih manual
//  g++ -o bin/run src/QuadTree.cpp src/Metrics.cpp src/ImageProcessing.cpp -ljpeg
//  ./bin/run
