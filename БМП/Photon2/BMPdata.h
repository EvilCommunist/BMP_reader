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

class BMPdata
{
private:
	HeaderReader bmpHead;
	Matrix colours;
public:
	BMPdata() {
	}
	void setHeader(HeaderReader h);
	void setColorMatrix(Matrix mat);

	Matrix getMatrix();

	void fillBMP(std::ostream& out);
};