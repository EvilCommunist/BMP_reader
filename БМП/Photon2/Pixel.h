#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <iterator>
#include <windows.h>
#include <TCHAR.h>

class Pixel
{
private:
    uint8_t red;
    uint8_t green;
    uint8_t blue;
public:

    void SetPixel(uint8_t r, uint8_t g, uint8_t b);

    uint8_t GetRed();
    uint8_t GetGreen();
    uint8_t GetBlue();

    void PrintPixel();
    void PrintPixel(std::ostream& out);
    void PixelToImage(std::ostream& out);
};