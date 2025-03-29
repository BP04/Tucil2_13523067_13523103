#include "QuadTree.hpp"

RGBPixel::RGBPixel() : r(0), g(0), b(0) {}
RGBPixel::RGBPixel(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

QuadTreeNode::QuadTreeNode(int x, int y, int width, int height, RGBPixel color, bool isLeaf)
    : x(x), y(y), width(width), height(height), color(color), isLeaf(isLeaf),
      atasKiri(nullptr), atasKanan(nullptr), bawahKiri(nullptr), bawahKanan(nullptr) {}
