#pragma once
#include <iostream>
#include <fstream>
#include "Struct.h"
#include "Pixel.h"
#include "Matrix.h"
#include "Size.h"
#include "Foundation.h"

uint32_t takeHeadInf(std::ifstream& input);

Size takeBitMapInfo(std::ifstream& input);

HeaderReader makeHeader(Matrix mat);