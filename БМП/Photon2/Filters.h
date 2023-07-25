#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <iterator>
#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <TCHAR.h>
#include <vector>
#include "Struct.h"
#include "Pixel.h"
#include "Matrix.h"
#include "Size.h"
#include "Coordinate.h"
#include "Foundation.h"

char initializeargument(std::string arg);

Matrix filterCrop(Matrix mat, size_t newwidth, size_t newheight);

Matrix filterGs(Matrix mat);

Matrix makeSmoothPixel(Matrix mat, Coordinate cp, Coordinate ulp, int z);
Matrix filterSmooth(Matrix mat, int zone);

Pixel newPix(Matrix mat);
Matrix filterBorder(Matrix mat);

Pixel contrFunc(Pixel pix);
Matrix filterContrast(Matrix mat);

Matrix transpone(Matrix mat, short int angle);

Matrix addFilters(std::vector<std::string> arg1, Matrix matr);