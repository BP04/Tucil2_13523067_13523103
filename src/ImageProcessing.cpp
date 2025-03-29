#include <iostream>
#include "QuadTree.hpp"
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

RGBPixel CalculateAverageColor(const vector<RGBPixel> &image, int x, int y, int width, int height)
{
    uint32_t r = 0, g = 0, b = 0;
    for (int i = y; i < y + height; i++)
    {
        for (int j = x; j < x + width; j++)
        {
            int idx = i * width + j;
            r += image[idx].r;
            g += image[idx].g;
            b += image[idx].b;
        }
    }
    int totalPixel = width * height;
    return RGBPixel((u_int8_t)(r / totalPixel), (u_int8_t)(g / totalPixel), (u_int8_t)(b / totalPixel));
}

double CalculateVariance(const vector<RGBPixel> &image, int x, int y, int width, int height, const RGBPixel &avgColor)
{
    double variance = 0.0;
    for (int i = y; i < y + height; i++)
    {
        for (int j = x; j < x + width; j++)
        {
            int idx = i * width + j;
            variance += pow(image[idx].r - avgColor.r, 2) +
                        pow(image[idx].g - avgColor.g, 2) +
                        pow(image[idx].b - avgColor.b, 2);
        }
    }
    return variance / (3 * width * height);
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
    fclose(file);
}

int main()
{
    try
    {
        int width, height;
        vector<RGBPixel> image = LoadImage("ishow_speed.jpeg", width, height);
        SaveImage("ishow_meat.jpeg", image, width, height);
    }
    catch (const ImageLoadException &e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}

// run sementara masih manual
//  g++ -o bin/run src/QuadTree.cpp src/ImageProcessing.cpp -ljpeg
//  ./bin/run