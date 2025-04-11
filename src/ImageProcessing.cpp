#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "gifenc.h"
#include "QuadTree.hpp"
#include "Metrics.hpp"
#include "ImageLoadException.hpp"

#include "gif-library/iff2gif/neuquant.hpp"

#include <iostream>
#include <iomanip>
#include <sys/stat.h>
#include <chrono>
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

void SaveImage(std::string fileName, const std::vector<RGBPixel> &image, int &width, int &height, bool show) {
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

    if(show) {
        if (success) {
            std::cout << "Gambar berhasil disimpan di " << fileName << std::endl;
        } else {
            std::cerr << "Gambar tidak berhasil disimpan" << std::endl;
        }
    }
}

RGBPixel CalculateAverageColor(const std::vector<RGBPixel> &image, int x, int y, int width, int height, int &imageWidth) {
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
    return RGBPixel((uint8_t)(r / totalPixel), (uint8_t)(g / totalPixel), (uint8_t)(b / totalPixel));
}

std::unique_ptr<QuadTreeNode> BuildQuadTree(std::vector<RGBPixel> &image, int x, int y, int w, int h, double threshold, int minBlockSize, int errorMeasurementChoice, int imageWidth) {
    if(w <= 0 || h <= 0) {
        return nullptr;
    }
    
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
        // Entropy
        error = CalculateEntropy(image, x, y, w, h, avgColor, imageWidth);
        break;

    case 5:
        // SSIM
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

void reconstructImage(std::vector<RGBPixel> &outputImage, std::unique_ptr<QuadTreeNode> &node, int &imageWidth) {
    if (!node)
    {
        return;
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

double CalculateCompressionRatio(const std::string &uncompressedFile, const std::string &compressedFile, bool show) {
    struct stat uncompressedStat, compressedStat;
    if (stat(uncompressedFile.c_str(), &uncompressedStat) != 0) {
        throw ImageLoadException("Cannot get file size: " + uncompressedFile);
    }
    if (stat(compressedFile.c_str(), &compressedStat) != 0) {
        throw ImageLoadException("Cannot get file size: " + compressedFile);
    }

    double uncompressedSize = uncompressedStat.st_size;
    double compressedSize = compressedStat.st_size;
    if (show) {
        std::cout << "Ukuran Sebelum Kompresi: " << uncompressedSize << " bytes" << std::endl;
        std::cout << "Ukuran Setelah Kompresi: " << compressedSize << " bytes" << std::endl;
    }
    return (1 - (compressedSize / uncompressedSize)) * 100.0;
}

int GetMaxDepth(std::unique_ptr<QuadTreeNode> &node) {
    if (!node) {
        return 0;
    }

    if (node->isLeaf) {
        return 1;
    }

    return std::max({GetMaxDepth(node->atasKiri),
                     GetMaxDepth(node->atasKiri),
                     GetMaxDepth(node->bawahKiri),
                     GetMaxDepth(node->bawahKanan)}) + 1;
}

int GetNodeCount(std::unique_ptr<QuadTreeNode> &node) {
    if (!node) {
        return 0;
    }

    if (node->isLeaf) {
        return 1;
    }

    return GetNodeCount(node->atasKiri) +
           GetNodeCount(node->atasKanan) +
           GetNodeCount(node->bawahKiri) +
           GetNodeCount(node->bawahKanan) + 1;
}

void SaveGif(const std::string &gifOutputPath, const std::vector<RGBPixel> &image, std::unique_ptr<QuadTreeNode> &root, int imageWidth, int imageHeight) {
    std::vector<uint8_t> rgbData(image.size() * 3);
    for (size_t i = 0; i < image.size(); ++i) {
        rgbData[i * 3 + 0] = image[i].r;
        rgbData[i * 3 + 1] = image[i].g;
        rgbData[i * 3 + 2] = image[i].b;
    }
    
    NeuQuant* neuquant = new NeuQuant(256);
    Quantizer* quantizer = neuquant;
    quantizer->AddPixels(rgbData.data(), image.size());
    Palette palette = quantizer->GetPalette();
    
    uint8_t gifPalette[256 * 3];
    
    memset(gifPalette, 0, sizeof(gifPalette));
    
    for (size_t i = 0; i < std::min(palette.size(), size_t(256)); ++i) {
        gifPalette[i * 3 + 0] = palette[i].red;
        gifPalette[i * 3 + 1] = palette[i].green;
        gifPalette[i * 3 + 2] = palette[i].blue;
    }
    
    ge_GIF* gif = ge_new_gif(gifOutputPath.c_str(), imageWidth, imageHeight, gifPalette, 8, 0, 0);

    if (!gif) {
        std::cerr << "Failed to create GIF." << std::endl;
        delete quantizer;
        return;
    }
    
    std::vector<QuadTreeNode*> nodes, newNodes;
    nodes.push_back(root.get());
    
    int counter = 0;
    bool cont = true;
    
    while (cont && !nodes.empty()) {
        cont = false;
        std::vector<uint8_t> frameRGB(imageWidth * imageHeight * 3);
        std::vector<uint8_t> frameIndexed(imageWidth * imageHeight);
        
        for (QuadTreeNode* node : nodes) {
            if (!node) continue;
            
            for (int i = 0; i < node->height; i++) {
                for (int j = 0; j < node->width; j++) {
                    int xx = node->x + j, yy = node->y + i;
                    int idx = yy * imageWidth + xx;
                    
                    frameRGB[idx * 3 + 0] = node->color.r;
                    frameRGB[idx * 3 + 1] = node->color.g;
                    frameRGB[idx * 3 + 2] = node->color.b;
                    
                    ColorRegister pixel = {node->color.r, node->color.g, node->color.b};
                    frameIndexed[idx] = neuquant->lookup(pixel);
                }
            }
            

            if (node->isLeaf) {
                newNodes.push_back(node);
            }
            else {
                cont = true;
                newNodes.push_back(node->atasKiri.get());
                newNodes.push_back(node->atasKanan.get());
                newNodes.push_back(node->bawahKiri.get());
                newNodes.push_back(node->bawahKanan.get());
            }
        }
        
        memcpy(gif->frame, frameIndexed.data(), imageWidth * imageHeight);
        ge_add_frame(gif, 100);
        
        nodes.swap(newNodes);
        std::vector<QuadTreeNode*>().swap(newNodes);
    }
    
    std::vector<QuadTreeNode*>().swap(nodes);
    std::vector<QuadTreeNode*>().swap(newNodes);
    delete quantizer;
    
    if (gif) {
        ge_close_gif(gif);
        std::cout << "GIF berhasil disimpan di " << gifOutputPath << std::endl;
    }
}

int main() {
    try {
        int width, height;

        std::string originalImagePath;
        std::string compressedImagePath;
        std::string gifOutputPath;
        int errorMeasurementChoice;
        double threshold;
        int minBlockSize;
        double targetCompressionRatio;
        double low, high;

        std::cout << "Masukkan alamat absolut ke gambar input (contoh: test/a.jpg): ";
        std::getline(std::cin, originalImagePath);

        struct stat buffer;
        while (stat(originalImagePath.c_str(), &buffer) != 0) {
            std::cerr << "File tidak ditemukan: " << originalImagePath << std::endl;
            std::cout << "Masukkan alamat absolut ke gambar input (contoh: test/a.jpg): ";
            std::getline(std::cin, originalImagePath);
        }

        std::cout << "Pilih metode perhitungan error:\n";
        std::cout << "1. Variansi\n";
        std::cout << "2. Rata-rata Deviasi Absolut (MAD)\n";
        std::cout << "3. Selisih Piksel Maksimum\n";
        std::cout << "4. Entropi\n";
        std::cout << "5. SSIM (Structural Similarity Index)\n";
        std::cout << "Masukkan nomor metode (1-5): ";
        std::cin >> errorMeasurementChoice;

        while (errorMeasurementChoice < 1 || errorMeasurementChoice > 5) {
            std::cout << "Pilihan tidak valid. Silakan pilih antara 1-5: ";
            std::cin >> errorMeasurementChoice;
        }

        switch (errorMeasurementChoice)
        {
        case 1:
            // Variance
            low = 0; high = 16256.25;
            break;
        case 2:
            // Mean Absolute Deviation (MAD)
            low = 0; high = 127.5;
            break;
        case 3:
            // Max Pixel Difference
            low = 0; high = 255;
            break;

        case 4:
            // Entropy (rentang 0-8)
            low = 0; high = 8;
            break;

        case 5:
            // SSIM (rentang 0-1)
            low = -1; high = 1;
            break;
        }

        std::cout << "Masukkan nilai threshold: ";
        std::cin >> threshold;

        while (threshold < low || threshold > high) {
            std::cout << "Nilai threshold tidak valid. Silakan masukkan nilai antara " << low << " dan " << high << ": ";
            std::cin >> threshold;
        }

        std::cout << "Masukkan ukuran blok minimum: ";
        std::cin >> minBlockSize;

        while (minBlockSize < 1) {
            std::cout << "Ukuran blok minimum tidak valid. Silakan masukkan nilai lebih besar dari 0: ";
            std::cin >> minBlockSize;
        }

        std::cout << "Masukkan target rasio kompresi (0 untuk menonaktifkan mode ini, 1.0 untuk 100%): ";
        std::cin >> targetCompressionRatio;
        std::cin.ignore();

        while (targetCompressionRatio < 0.0 || targetCompressionRatio > 1.0) {
            std::cout << "Rasio kompresi tidak valid. Silakan masukkan nilai antara 0.0 dan 1.0: ";
            std::cin >> targetCompressionRatio;
            std::cin.ignore();
        }

        std::cout << "Masukkan alamat absolut untuk menyimpan gambar hasil kompresi (contoh: test/b.png): ";
        std::getline(std::cin, compressedImagePath);

        std::cout << "Masukkan alamat absolut untuk menyimpan GIF (contoh: test/process.gif): ";
        std::getline(std::cin, gifOutputPath);

        std::cout << "Memproses gambar..." << std::endl;

        auto startTime = std::chrono::high_resolution_clock::now();

        std::unique_ptr<QuadTreeNode> root;
        std::vector<RGBPixel> image = LoadImage(originalImagePath, width, height);

        std::vector<RGBPixel> outputImage(width * height);

        if (targetCompressionRatio == 0.0) {
            root = BuildQuadTree(image, 0, 0, width, height, threshold, minBlockSize, errorMeasurementChoice, width);
        }
        else {
            // Mengasumsikan rasio bergantung sepenuhnya pada threshold
            
            targetCompressionRatio *= 100.0;

            int tempBlockSize = 1;
            long double L = low, R = high;

            for (int _ = 0; _ < 20; _++) {
                long double M = (L + R) / 2.0;

                root = BuildQuadTree(image, 0, 0, width, height, M, tempBlockSize, errorMeasurementChoice, width);

                outputImage = std::vector<RGBPixel>(width * height);
                reconstructImage(outputImage, root, width);
                SaveImage(compressedImagePath, outputImage, width, height, false);
                double compressionRatio = CalculateCompressionRatio(originalImagePath, compressedImagePath, false);
                if (compressionRatio < targetCompressionRatio) {
                    L = M;
                }
                else {
                    R = M;
                }
            }
        }

        reconstructImage(outputImage, root, width);

        SaveImage(compressedImagePath, outputImage, width, height, true);
        SaveGif(gifOutputPath, outputImage, root, width, height);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        std::cout << "Waktu pemrosesan: " << duration << " ms" << std::endl;

        double compressionRatio = CalculateCompressionRatio(originalImagePath, compressedImagePath, 1);
        std::cout << std::fixed << std::setprecision(6) << "Rasio Kompresi: " << compressionRatio << "%" << std::endl;
        
        int maxDepth = GetMaxDepth(root);
        std::cout << "Kedalaman Maksimum: " << maxDepth << std::endl;
        
        int nodeCount = GetNodeCount(root);
        std::cout << "Banyak Simpul: " << nodeCount << std::endl;
    }
    catch (const ImageLoadException &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}