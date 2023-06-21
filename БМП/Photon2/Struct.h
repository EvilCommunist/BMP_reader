#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <iterator>
#include <windows.h>
#include <TCHAR.h>
#include <vector>

struct HeaderReader // Заполнение всех полей кроме размера файла и размера картинки автоматическое. Задача: рассчитать размер файла в байтах и пиксельных данных
{
    uint8_t sign1;
    uint8_t sign2;
    uint32_t fileSize;
    uint16_t reserve1;
    uint16_t reserve2;
    uint32_t paddToData;
    uint32_t bmpType;
    size_t width;
    size_t height;
    uint16_t cursor;
    uint16_t bpp; // bit per pixel
    uint32_t contain;
    uint32_t pixDataSize;
    uint32_t ppmX; // pixel per meter
    uint32_t ppmY;
    uint32_t colorDataSize;
    uint32_t cellsNum; // Кол-во ячеек от начала таблицы цветов до последней используемой
};