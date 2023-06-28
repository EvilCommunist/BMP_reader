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
#include "Size.h"

using byte = unsigned char;

void readBytes(std::ifstream& stream, byte* begin, std::streamsize count) {
    bool success{};

    if (stream.read(reinterpret_cast<char*>(begin), count)) {
        success = stream.gcount() == count;
    }

    if (!success) {
        throw std::runtime_error("read failed");
    }
}

template <class T> 
T read(std::ifstream& stream) {
    T tmp;
    readBytes(stream, reinterpret_cast<byte*>(&tmp), sizeof(T));
    return tmp;
}

template <class T, size_t head>
std::array<T, head> readArray(std::ifstream& stream) {
    std::array<T, head> array;
    byte* tmpAsBytes = reinterpret_cast<byte*>(array.data());
    readBytes(stream, tmpAsBytes, sizeof(T) * head);
    return array;
}


template <class T>
T takeNextField(std::ifstream& stream) {
    const auto val = read<T>(stream);
    return val;
}

 //return смещение до начала пиксельных данных
 
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

char initializeargument(std::string arg)
{
    // В виду наличия аргументов с разными первыми буквами ограничимся выводом первой буквы аргумента
    return arg[1];
}

Matrix filterCrop(Matrix mat, size_t newwidth, size_t newheight)
{
    Matrix newmat(newwidth, newheight);
    for (size_t i = 0; i < newmat.GetMatHeight(); ++i) {
        for (size_t j = 0; j < newmat.GetMatWidth(); ++j) {
            newmat.SetPixel(i, j, mat.GetPixel(i, j));
        }
    }
    return newmat;
}

Matrix filterGs(Matrix mat)
{
    Matrix newmat = mat;
    for (size_t i = 0; i < mat.GetMatHeight(); ++i) {
        for (size_t j = 0; j < mat.GetMatWidth(); ++j) {
            Pixel pix;
            pix.SetPixel(mat.GetPixel(i, j).GetRed() * 0.299, mat.GetPixel(i, j).GetGreen() * 0.587, mat.GetPixel(i, j).GetBlue() * 0.114);
            newmat.SetPixel(i, j, pix);
        }
    }
    return newmat;
}


Matrix addFilters(std::vector<std::string> arg1, Matrix matr) 
{
    std::vector<std::string> arg = arg1;
    Matrix matrix = matr;
    for (size_t i = 0; i < arg.size(); i++)
    {
        if (arg[i][0] == '-') // InitializeToken
        {
            char c = initializeargument(arg[i]);
            switch (c)
            {
            case 'c': matrix = filterCrop(matrix, stod(arg[i + 1]), stod(arg[i + 2])); i += 2; break;
            case 'g': matrix = filterGs(matrix); break;
            }
        }
    }

    return matrix;
}

Matrix openAndFillImage(const std::string& filepath, std::vector<std::string> arg) // Считывание цветов в матрицу
{
    std::ifstream input;
    std::string s1 = filepath;
    input.open(filepath, input.binary | input.in);

    if (!input.is_open()) { // Проверка на открытие файла
        Matrix nullmat(0, 0);
        std::cout << "Файл не открыт";
        return nullmat;
    }

    const uint32_t pixelsOffset = takeHeadInf(input);
    const Size size = takeBitMapInfo(input);
    const size_t pixelsBytesPerLine = size.width * 3;
    const size_t paddingBytes = pixelsBytesPerLine % 4;

    input.seekg(pixelsOffset);

    uint8_t r, g, b;

    Pixel pix;

    Matrix mat(size.width, size.height);

    for (size_t i = 0; i < size.height; ++i) {
        for (size_t j = 0; j < size.width; ++j) {
            b = (unsigned)read<uint8_t>(input);
            g = (unsigned)read<uint8_t>(input);
            r = (unsigned)read<uint8_t>(input);
            pix.SetPixel(r, g, b);
            mat.SetPixel(i, j, pix);
        }
        for (size_t pad = 0; pad != paddingBytes; ++pad) {
            read<uint8_t>(input);
        }
    }
    return addFilters(arg, mat);
}

HeaderReader makeHeader(Matrix mat)
{
    HeaderReader h;
    h.width = mat.GetMatWidth();
    h.height = mat.GetMatHeight();
    h.pixDataSize = h.width * h.height * h.bpp;
    h.fileSize = h.pixDataSize + h.paddToData;
    return h;
}

void printMatrix(const Matrix& matrix, std::ostream& out) // Вывод матрицы
{
    Matrix mat = matrix;
    HeaderReader bmpHead = makeHeader(mat);

    std::cout << std::endl << bmpHead.sign1 << std::endl;
    std::cout << bmpHead.sign2 << std::endl;
    std::cout << bmpHead.fileSize << std::endl;
    std::cout << bmpHead.reserve1 << std::endl;
    std::cout << bmpHead.reserve2 << std::endl;
    std::cout << bmpHead.paddToData << std::endl;
    std::cout << bmpHead.bmpType << std::endl;
    std::cout << bmpHead.width << std::endl;
    std::cout << bmpHead.height << std::endl;
    std::cout << bmpHead.cursor << std::endl;
    std::cout << bmpHead.bpp << std::endl;
    std::cout << bmpHead.contain << std::endl;
    std::cout << bmpHead.pixDataSize << std::endl;
    std::cout << bmpHead.ppmX << std::endl;
    std::cout << bmpHead.ppmY << std::endl;
    std::cout << bmpHead.colorDataSize << std::endl;
    std::cout << bmpHead.cellsNum << std::endl;

    out.write((char*)&bmpHead,sizeof bmpHead);
    out.write((char*)&mat, sizeof mat);
/*
    for (size_t i = 0; i < mat.GetMatHeight(); ++i) {
        for (size_t j = 0; j < mat.GetMatWidth(); ++j) {
            //mat.GetPixel(i, j).PrintPixel(out);
       //     mat.GetPixel(i, j).PrintPixel();
        }
        //out << std::endl;
      //  std::cout << std::endl;
    }*/
}

int main(int files, char* data[])
{
    if (files < 3) throw std::runtime_error("input 2 paths next time");
    setlocale(LC_CTYPE, "Russian");
    std::vector<std::string> arg;

    for (size_t i = 3; i < files; i++) // Создаём вектор аргмуентов
    {
        arg.push_back(data[i]);
    }

    std::ofstream output;
    output.open(data[2], output.out);

    printMatrix(openAndFillImage(data[1], arg), output);
}