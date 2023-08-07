#pragma once
#include <iostream>

struct HeaderReader // Заполнение всех полей кроме размера файла и размера картинки автоматическое. Задача: рассчитать размер файла в байтах и пиксельных данных
{
    uint8_t sign1 = 'B';
    uint8_t sign2 = 'M';
    uint32_t fileSize = 0;
    uint16_t reserve1 = 0;
    uint16_t reserve2 = 0;
    uint32_t paddToData = 54;
    uint32_t bmpType = 40;
    int32_t width = 0;
    int32_t height = 0;
    uint16_t cursor = 1;
    uint16_t bpp = 24; // bit per pixel
    uint32_t contain = 0;
    uint32_t pixDataSize;
    int32_t ppmX = 2835; // pixel per meter
    int32_t ppmY = 2835;
    uint32_t colorDataSize = 0;
    uint32_t cellsNum = 0; // Кол-во ячеек от начала таблицы цветов до последней используемой
};