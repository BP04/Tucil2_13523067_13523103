#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <memory>
#include <stdexcept>

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
    std::unique_ptr<QuadTreeNode> atasKiri, atasKanan, bawahKiri, bawahKanan;

    QuadTreeNode(int x, int y, int width, int height, RGBPixel color, bool isLeaf);
};

#endif