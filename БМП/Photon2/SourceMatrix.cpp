#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <iterator>
#include <windows.h>
#include <TCHAR.h>
#include <vector>
#include "HeaderPixel.h"
#include "SourcePixel.cpp"
#include "HeaderMatrix.h"

bool Matrix::CoordIsValid(unsigned int i, unsigned int j) const
{
    return ((i < height) && (j < width));
}
int Matrix::GetCoord(int i, int j) const
{
    if (CoordIsValid(i, j))
    {
        return i * width + j;
    }
    else
    {
        throw std::runtime_error("Attempt to call coordinate out of range");
    }
}

Matrix::Matrix(size_t widthread, size_t heightread)
{
    width = widthread;
    height = heightread;
    image.resize(width * height);
}

void Matrix::SetPixel(unsigned int i, unsigned int j, Pixel pix)
{
    image[GetCoord(i, j)] = pix;
}
Pixel Matrix::GetPixel(unsigned int i, unsigned int j) const
{
    return image[GetCoord(i, j)];
}

size_t Matrix::GetMatHeight()
{
    return height;
}
size_t Matrix::GetMatWidth()
{
    return width;
}