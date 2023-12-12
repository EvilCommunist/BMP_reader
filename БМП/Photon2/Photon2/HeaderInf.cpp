#pragma once
#include <iostream>
#include <fstream>
#include "Struct.h"
#include "Pixel.h"
#include "Matrix.h"
#include "Size.h"
#include "Foundation.h"

uint32_t takeHeadInf(std::ifstream& input) {
    const auto sign = readArray<uint8_t, 2>(input);
    std::cout << "Сигнатура: " << sign.front() << sign.back() << std::endl;

    takeNextField<uint32_t>(input);

    const auto reserved = readArray<uint16_t, 2>(input);
    std::cout << "Зарезервированные поля (ожидается 0): " << reserved.front() << " " << reserved.back() << std::endl;

    return takeNextField<uint32_t>(input);
}

Size takeBitMapInfo(std::ifstream& input) {
    uint32_t struck = takeNextField<uint32_t>(input);

    if ((struck != 12) && (struck != 40) && (struck != 108) && (struck != 124)) // Проверка на валидность типа БМП
        throw std::runtime_error("unknown BMP type");

    switch (struck)
    {
    case 12:
        throw std::runtime_error("cannot read this type of BMP (BITMAPCOREHEADER)");
        break;
    case 40:
        std::cout << "Имя структуры: BITMAPINFOHEADER, файл корректный" << std::endl;
        break;
    case 108:
        throw std::runtime_error("cannot read this type of BMP (BITMAPV4HEADER)");
        break;
    case 124:
        throw std::runtime_error("cannot read this type of BMP (BITMAPV5HEADER)");
        break;
    }

    const auto width = takeNextField<int32_t>(input);
    const auto height = takeNextField<int32_t>(input);
    return { width, height };
}

HeaderReader makeHeader(Matrix mat)
{
    HeaderReader h;
    h.width = mat.GetMatWidth();
    h.height = mat.GetMatHeight();
    h.pixDataSize = h.width * h.height * h.bpp / 8;
    h.fileSize = h.pixDataSize + h.paddToData;
    return h;
}
