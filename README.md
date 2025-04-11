# Tucil2_13523067_13523103
## Overview
This program is a C++ application for image compression using a quadtree data structure, offering five metrics: variance, mean absolute deviation (MAD), maximum pixel difference, entropy, and the Structural Similarity Index (SSIM). Users can either input a threshold value directly or specify a target compression ratio, and the program will automatically adjust the threshold via binary search to achieve the desired ratio. Additionally, each quadtree subdivision step is recorded and rendered as a GIF animation to provide a visualization of the block‑based compression process.

## Requirements and Instalation

```bash
sudo apt update
sudo apt install libjpeg-turbo8-dev libgif-dev build-essential cmake
```

## How to Run Program
mkdir build
cd build
cmake ..
make
```
## How to Use Program 
