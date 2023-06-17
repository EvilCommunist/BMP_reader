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

class Matrix {
private:

    std::vector<Pixel> image;

    size_t width;
    size_t height;

    bool CoordIsValid(unsigned int i, unsigned int j) const;
    int GetCoord(int i, int j) const;

public:

    Matrix(size_t widthread, size_t heightread);

    void SetPixel(unsigned int i, unsigned int j, Pixel pix);
    Pixel GetPixel(unsigned int i, unsigned int j) const;

    size_t GetMatHeight();
    size_t GetMatWidth();
};