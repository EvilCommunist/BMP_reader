#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "Struct.h"
#include "Pixel.h"
#include "Matrix.h"
#include "Size.h"
#include "Foundation.h"
#include"Filters.h"
#include "ImageInf.h"

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

    uint8_t r=0, g=0, b=0;

    Pixel pix;

    Matrix mat(size.width, size.height);

    for (size_t i = 0; i < size.height; ++i) {
        for (size_t j = 0; j < size.width; ++j) {
            b = (unsigned)read<char>(input);
            g = (unsigned)read<char>(input);
            r = (unsigned)read<char>(input);
            pix.SetPixel(r, g, b);
            mat.SetPixel(i, j, pix);
        }
        for (size_t pad = 0; pad != paddingBytes; ++pad) { // Ориентация сюда
            read<uint8_t>(input);
        }
    }
    return addFilters(arg, mat);
}

void printMatrix(const Matrix& matrix, std::ostream& out) // Вывод изображения в файл (надо доработать)
{
    Matrix mat = matrix;
    HeaderReader bmpHead = makeHeader(mat);

    out.write((char*)&bmpHead.sign1, sizeof(uint8_t));
    out.write((char*)&bmpHead.sign2, sizeof(uint8_t));
    out.write((char*)&bmpHead.fileSize, sizeof(uint32_t));
    out.write((char*)&bmpHead.reserve1, sizeof(uint16_t));
    out.write((char*)&bmpHead.reserve2, sizeof(uint16_t));
    out.write((char*)&bmpHead.paddToData, sizeof(uint32_t));
    out.write((char*)&bmpHead.bmpType, sizeof(uint32_t));
    out.write((char*)&bmpHead.width, sizeof(int32_t));
    out.write((char*)&bmpHead.height, sizeof(int32_t));
    out.write((char*)&bmpHead.cursor, sizeof(uint16_t));
    out.write((char*)&bmpHead.bpp, sizeof(uint16_t));
    out.write((char*)&bmpHead.contain, sizeof(uint32_t));
    out.write((char*)&bmpHead.pixDataSize, sizeof(uint32_t));
    out.write((char*)&bmpHead.ppmX, sizeof(int32_t));
    out.write((char*)&bmpHead.ppmY, sizeof(int32_t));
    out.write((char*)&bmpHead.colorDataSize, sizeof(uint32_t));
    out.write((char*)&bmpHead.cellsNum, sizeof(uint32_t));
 
    Pixel prt1;
    size_t paddCounter = mat.GetMatWidth()*3;
    for (size_t i = 0; i < mat.GetMatHeight(); i++) {
        for(size_t j = 0; j < mat.GetMatWidth(); j++){
            prt1 = mat.GetPixel(i, j);
            prt1.PixelToImage(out);// Рассинхрон значений пикселей на втрой строчке заполнения (раньше, 240 строчка в файле), проверить паддинговые значения!!!!
        }
        while (paddCounter % 4 != 0) {
            out.write((char*)&bmpHead.reserve1, sizeof(uint8_t));
            paddCounter++;
        }
        paddCounter = mat.GetMatWidth() * 3;
    }
}

void errorChecker(const std::string& filepath, const std::string& filepath1) // Метод проверки ошибок
{
    std::ifstream input;
    input.open(filepath, input.binary | input.in);
    std::ifstream input1;
    input1.open(filepath1, input.binary | input.in);
    std::ofstream out;
    out.open("C:\\Users\\AT241\\source\\repos\\Photon2\\Pazhiloy\\younoturrr.txt");
        takeNextField<int8_t>(input);
        takeNextField<int8_t>(input1);
        takeNextField<int8_t>(input);
        takeNextField<int8_t>(input1);
        takeNextField<int32_t>(input);
        takeNextField<int32_t>(input1);
        takeNextField<int32_t>(input);
        takeNextField<int32_t>(input1);
        takeNextField<int32_t>(input);
        takeNextField<int32_t>(input1);
        takeNextField<int32_t>(input);
        takeNextField<int32_t>(input1);//Смещенеи данных
        takeNextField<int32_t>(input);
        takeNextField<int32_t>(input1);
        takeNextField<int32_t>(input);
        takeNextField<int32_t>(input1);
        takeNextField<int16_t>(input);
        takeNextField<int16_t>(input1);
        takeNextField<int16_t>(input);
        takeNextField<int16_t>(input1);
        takeNextField<int32_t>(input);
        takeNextField<int32_t>(input1);
        takeNextField<int32_t>(input);
        takeNextField<int32_t>(input1);
        takeNextField<int32_t>(input);
        takeNextField<int32_t>(input1);
        takeNextField<int32_t>(input);
        takeNextField<int32_t>(input1);
        takeNextField<int32_t>(input);
        takeNextField<int32_t>(input1);
        takeNextField<int32_t>(input);
        takeNextField<int32_t>(input1);
        for (int i = 0; i < 1024; i++) {
            auto printRGB = [](auto& in, auto& out) {
                unsigned r = takeNextField<uint8_t>(in);
                unsigned g = takeNextField< uint8_t>(in);
                unsigned b = takeNextField<uint8_t>(in);

                if (!in.good()) {
                    throw std::runtime_error("EOF");
                }
                out << "[" << r << ", " << g << ", " << b << "]";
            };

            out << i << " ";
            printRGB(input, out);
            out << " ||| ";
            printRGB(input1, out);
            out << std::endl;
        }
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
    output.open(data[2], std::ios::binary);

    printMatrix(openAndFillImage(data[1], arg), output);
    errorChecker(data[1], data[2]);
}