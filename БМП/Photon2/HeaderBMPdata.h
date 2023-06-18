#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <iterator>
#include <windows.h>
#include <TCHAR.h>
#include <vector>
#include "HeaderStruct.h"
#include "HeaderPixel.h"
#include "SourcePixel.cpp"
#include "HeaderMatrix.h"
#include "SourceMatrix.cpp"

class BMPdata
{
private:
	HeaderReader bmpHead;
	Matrix colours;
public:
	void setHeader(HeaderReader h);
	void setColorMatrix(Matrix mat);

	void fillBMP(std::ostream& out);
};