#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <iterator>
#include <windows.h>
#include <TCHAR.h>
#include <vector>
#include "Struct.h"
#include "Pixel.h"
#include "Matrix.h"
#include "BMPdata.h"

void BMPdata::setHeader(HeaderReader h)
{
	bmpHead = h;
}
void BMPdata::setColorMatrix(Matrix mat)
{
	colours.setMatrix(mat);
	colours = mat;
}

Matrix BMPdata::getMatrix()
{
	return colours;
}

void BMPdata::fillBMP(std::ostream& out)
{
	out << bmpHead.sign1;
	out << bmpHead.sign2;
	out << bmpHead.fileSize;
	out << bmpHead.reserve1;
	out << bmpHead.reserve2;
	out << bmpHead.paddToData;
	out << bmpHead.bmpType;
	out << bmpHead.width;
	out << bmpHead.height;
	out << bmpHead.cursor;
	out << bmpHead.bpp;
	out << bmpHead.contain;
	out << bmpHead.pixDataSize;
	out << bmpHead.ppmX;
	out << bmpHead.ppmY;
	out << bmpHead.colorDataSize;
	out << bmpHead.cellsNum;

	for (size_t i = 0; i < colours.GetMatHeight(); ++i) {
		for (size_t j = 0; j < colours.GetMatWidth(); ++j) {
			colours.GetPixel(i, j).PixelToImage(out);
		}
		out << std::endl;
	}
}