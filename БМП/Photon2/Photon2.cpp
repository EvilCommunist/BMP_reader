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
#include "BMPdata.h"

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
 
HeaderReader takeHeadInf(std::ifstream& input) { 
    const auto sign = readArray<uint8_t, 2>(input);
    HeaderReader h;

    h.sign1 = sign.front();
    h.sign2= sign.back();

    h.fileSize = takeNextField<uint32_t>(input);

    const auto reserved = readArray<uint16_t, 2>(input);
    h.reserve1 = reserved.front();
    h.reserve2 = reserved.back();

    h.paddToData = takeNextField<uint32_t>(input);

    return h;
}

HeaderReader takeBitMapInfo(std::ifstream& input, HeaderReader h) { 
    uint32_t struck = takeNextField<uint32_t>(input);

    if ((struck!=12) && (struck != 40) && (struck != 108) && (struck != 124)) // Проверка на валидность типа БМП
        throw std::runtime_error("unknown BMP type");

     switch (struck)
    {
        case 12:
            throw std::runtime_error("cannot read this type of BMP (BITMAPCOREHEADER)");
            break;
        case 40:
            std::cout << "Имя структуры: BITMAPINFOHEADER, файл корректный" << std::endl;
            h.bmpType = struck;
            break;
        case 108:
            throw std::runtime_error("cannot read this type of BMP (BITMAPV4HEADER)");
            break;
        case 124:
            throw std::runtime_error("cannot read this type of BMP (BITMAPV5HEADER)");
            break;
    }

     h.width = takeNextField<int32_t>(input);
     h.height = takeNextField<int32_t>(input);
     h.cursor= takeNextField<uint16_t>(input);
     h.bpp = takeNextField<uint16_t>(input);
     h.contain = takeNextField<uint32_t>(input);
     h.pixDataSize = takeNextField<uint32_t>(input);
     h.ppmX = takeNextField<int32_t>(input);
     h.ppmY = takeNextField<int32_t>(input);
     h.colorDataSize = takeNextField<uint32_t>(input);
     h.cellsNum = takeNextField<uint32_t>(input);
     
    return h;
} 

char initializeargument(std::string arg)
{
    // В виду наличия аргументов с разными первыми буквами ограничимся выводом первой буквы аргумента
    return arg[1];
}

BMPdata filterCrop(Matrix mat, size_t newwidth, size_t newheight, HeaderReader h)
{
    h.width = newwidth;
    h.height = newheight;
    h.pixDataSize = newwidth * newheight * h.bpp;
    h.fileSize = h.pixDataSize + h.paddToData;

    BMPdata newpic;
    newpic.setHeader(h);
    Matrix newmat(newwidth, newheight);
    for (size_t i = 0; i < newmat.GetMatHeight(); ++i) {
        for (size_t j = 0; j < newmat.GetMatWidth(); ++j) {
            newmat.SetPixel(i, j, mat.GetPixel(i, j));
        }
    }
    newpic.setColorMatrix(newmat); 
    
    return newpic;
}

BMPdata filterGs(Matrix mat, HeaderReader h)
{
    BMPdata newpic;
    newpic.setHeader(h);
    Matrix newmat = mat;
    for (size_t i = 0; i < mat.GetMatHeight(); ++i) {
        for (size_t j = 0; j < mat.GetMatWidth(); ++j) {
            Pixel pix;
            pix.SetPixel(mat.GetPixel(i, j).GetRed() * 0.299, mat.GetPixel(i, j).GetGreen() * 0.587, mat.GetPixel(i, j).GetBlue() * 0.114);
            newmat.SetPixel(i, j, pix);
        }
    }
    newpic.setColorMatrix(newmat);
    return newpic;
}

BMPdata addFilters(std::vector<std::string> arg1, Matrix matr, HeaderReader h) 
{
    std::vector<std::string> arg = arg1;
    Matrix matrix = matr;
    BMPdata newpic;
    for (size_t i = 0; i<arg.size(); i++)
    {
        if (arg[i][0]=='-') // InitializeToken
        {
            char c = initializeargument(arg[i]);
            switch (c)
            {
            case 'c': newpic = filterCrop(matrix, stod(arg[i + 1]), stod(arg[i + 2]), h); i += 2; break;
            case 'g': newpic = filterGs(matrix, h); break;
            }
        }
    }

    return newpic;
}

BMPdata openAndFillImage(const std::string& filepath, std::vector<std::string> arg) // Считывание цветов в матрицу
{
    std::ifstream input;
    std::string s1 = filepath;
    input.open(filepath, input.binary | input.in);
   
    if (!input.is_open()) { // Проверка на открытие файла
        /*Matrix nullmat(0, 0);
        std::cout << "Файл не открыт";
        return nullmat;*/
        throw std::runtime_error("Файл не открыт, попробуйте снова");
    }

    HeaderReader head;

    head = takeHeadInf(input);

    const uint32_t pixelsOffset = head.paddToData;
    head = takeBitMapInfo(input, head);
    const size_t pixelsBytesPerLine = head.width * 3;
    const size_t paddingBytes = pixelsBytesPerLine % 4;

    input.seekg(pixelsOffset);

    uint8_t r, g, b;
    Pixel pix;

    Matrix mat(head.width, head.height);

    for (size_t i = 0; i < head.height; ++i) {
        for (size_t j = 0; j < head.width; ++j) {
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

    return addFilters(arg, mat, head);
}

void printMatrix(BMPdata pic, std::ostream& out) // Вывод матрицы
{
    /*Matrix mat = matrix;

    for (size_t i = 0; i < mat.GetMatHeight(); ++i) {
        for (size_t j = 0; j < mat.GetMatWidth(); ++j) {
            mat.GetPixel(i, j).PrintPixel(out);
        }
        out << std::endl;
    }*/
    pic.fillBMP(out);
}

int main (int files, char* data[])
{
    if (files < 3) throw std::runtime_error("input 2 paths next time");
    setlocale(LC_CTYPE, "Russian");
    std::vector<std::string> arg;

    for (size_t i = 3; i<files; i++) // Создаём вектор аргмуентов
    {
        arg.push_back(data[i]);
    }

    std::ofstream output;
    output.open(data[2], output.out);

    printMatrix(openAndFillImage(data[1], arg), output);
 }