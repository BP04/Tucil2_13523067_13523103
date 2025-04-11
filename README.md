# Image Compression Using QuadTrees Algorithm
## Overview
This program is a C++ application for image compression using a quadtree data structure, offering five metrics: variance, mean absolute deviation (MAD), maximum pixel difference, entropy, and the Structural Similarity Index (SSIM). Users can either input a threshold value directly or specify a target compression ratio, and the program will automatically adjust the threshold via binary search to achieve the desired ratio. Additionally, each quadtree subdivision step is recorded and rendered as a GIF animation to provide a visualization of the block‑based compression process.

## Requirements and Instalation

```bash
sudo apt update
sudo apt install libjpeg-turbo8-dev libgif-dev build-essential cmake
```

## How to Run Program
```
mkdir build
cd build
cmake ..
make
```
## How to Use Program 
- Set absolute path to your input image (e.g. /home/owen/test/a.jpg)

- Set error metric choice (1–5 for variance, MAD, max diff, entropy, SSIM)

- Set threshold (within the valid range printed)

- Set minimum block size (integer ≥ 1)

- Set target compression ratio (0 to disable auto‑tuning, or 0.0–1.0 to auto‑search)

- Set absolute path to save compressed image (e.g. /home/owen/test/aResult.jpg)

- Set absolute path to save the GIF (e.g. /home/owen/test/aGif.jpg)

## Precaution
Setting up a low threshold with low minimum block size (e.g. 1) can force the quadtree to recurse so deeply that you overflow the call stack. This condition lead to out‑of‑bounds pixel accesses (stack overflow), causing a segmentation fault.
