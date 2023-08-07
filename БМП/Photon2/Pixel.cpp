#pragma once
#include <iostream>
#include <fstream>
#include "Pixel.h"

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

void Pixel::PrintPixel() // Отладочный метод
{
    std::cout << "(" << (unsigned)red << "," << (unsigned)green << "," << (unsigned)blue << ")" << " ";
}

void Pixel::PixelToImage(std::ostream& out) // Бинарное заполнение согласно формату БМП
{
    out.write((char*)&blue, sizeof(uint8_t));
    out.write((char*)&green, sizeof(uint8_t));
    out.write((char*)&red, sizeof(uint8_t));
}