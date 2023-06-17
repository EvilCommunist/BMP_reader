#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <iterator>
#include <windows.h>
#include <TCHAR.h>
#include "HeaderPixel.h"

void Pixel::SetPixel(uint8_t r, uint8_t g, uint8_t b)
{
    red = r;
    green = g;
    blue = b;
}

uint8_t Pixel::GetRed()
{
    return red;
}

uint8_t Pixel::GetGreen()
{
    return green;
}

uint8_t Pixel::GetBlue()
{
    return blue;
}

void Pixel::PrintPixel(std::ostream& out)
{
    out << "(" << (unsigned)red << "," << (unsigned)green << "," << (unsigned)blue << ")" << " ";
}

void Pixel::PixelToImage(std::ostream& out)
{
    out << blue << green << red; // Бинарное заполнение согласно формату БМП
}