#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <memory>
#include <stdexcept>

using namespace std;

struct RGBPixel
{
    uint8_t r, g, b;
    RGBPixel();
    RGBPixel(uint8_t r, uint8_t g, uint8_t b);
};

class QuadTreeNode
{
public:
    int x, y;
    int width, height;
    RGBPixel color;
    bool isLeaf;
    unique_ptr<QuadTreeNode> atasKiri, atasKanan, bawahKiri, bawahKanan;

    QuadTreeNode(int x, int y, int width, int height, RGBPixel color, bool isLeaf);
};

class ImageLoadException : public std::runtime_error
{
public:
    explicit ImageLoadException(const std::string &msg)
        : std::runtime_error(msg) {}
};
#endif