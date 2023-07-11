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
#include "Coordinate.h"
#include "Foundation.h"

char initializeargument(std::string arg)
{
    // В виду наличия аргументов с разными первыми буквами ограничимся выводом первой буквы аргумента
    return arg[1];
}


Matrix filterCrop(Matrix mat, size_t newwidth, size_t newheight)
{
 //   size_t dx = mat.GetMatWidth() - newwidth - 1;
    size_t dy = mat.GetMatHeight() - newheight - 1;
    Matrix newmat(newwidth, newheight);
    for (size_t i = 0; i < newmat.GetMatHeight(); i++) {
        for (size_t j = 0; j < newmat.GetMatWidth(); ++j) {
            newmat.SetPixel(i, j, mat.GetPixel(i + dy, j));
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
            pix.SetPixel(mat.GetPixel(i, j).GetRed() * 0.299 + mat.GetPixel(i, j).GetGreen() * 0.587 + mat.GetPixel(i, j).GetBlue() * 0.114, mat.GetPixel(i, j).GetRed() * 0.299 + mat.GetPixel(i, j).GetGreen() * 0.587 + mat.GetPixel(i, j).GetBlue() * 0.114, mat.GetPixel(i, j).GetRed() * 0.299 + mat.GetPixel(i, j).GetGreen() * 0.587 + mat.GetPixel(i, j).GetBlue() * 0.114);
            newmat.SetPixel(i, j, pix);
        }
    }
    return newmat;
}


Pixel makeSmoothPixel(Matrix mat, Coordinate cp, Coordinate ulp) {
    Pixel pix = mat.GetPixel(cp.height-ulp.height, cp.width- ulp.width);
    for (int i = 0; i < mat.GetMatHeight(); ++i)
    {
        for (int j = 0; j < mat.GetMatWidth(); ++j)
        {
            if (((i == ulp.height - cp.height))&&((j== ulp.width - cp.width)))
               continue;
            uint8_t pixRed, pixGreen, pixBlue;
            int checker;
            checker = pix.GetRed() + mat.GetPixel(i, j).GetRed();
            if (checker > 255)
                pixRed = 255;
            else
                pixRed = checker;
            checker = pix.GetGreen() + mat.GetPixel(i, j).GetGreen();
            if (checker > 255)
                pixGreen = 255;
            else
                pixGreen = checker;
            checker = pix.GetBlue() + mat.GetPixel(i, j).GetBlue();
            if (checker > 255)
                pixBlue = 255;
            else
                pixBlue = checker;
            if (checker < 0)
                pixRed = 0;
            else
                pixRed = checker;
            checker = pix.GetGreen() + mat.GetPixel(i, j).GetGreen();
            if (checker < 0)
                pixGreen = 0;
            else
                pixGreen = checker;
            checker = pix.GetBlue() + mat.GetPixel(i, j).GetBlue();
            if (checker < 0)
                pixBlue = 0;
            else
                pixBlue = checker;
            pix.SetPixel(pixRed, pixGreen, pixBlue);
        }
    }
    return pix;
}

Matrix filterSmooth(Matrix mat, int zone) { // Размытие работает, но изображение сильно сереет, непонятно в чём дело
    if (zone % 2 == 0)
        throw std::runtime_error("Используйте нечётную зону размытия");

    Matrix newmat = mat;
    for (int i = 0; i < mat.GetMatHeight(); ++i)
    {
        for (int j = 0; j < mat.GetMatWidth(); ++j)
        {
            Pixel pix{};
            pix.SetPixel(mat.GetPixel(i, j).GetRed()/pow(zone,2), mat.GetPixel(i, j).GetGreen() / pow(zone, 2), mat.GetPixel(i, j).GetBlue() / pow(zone, 2));
            newmat.SetPixel(i, j, pix);
        }
    }
   
    for (int k = 0; k < mat.GetMatHeight(); ++k) {
        for (int g = 0; g < mat.GetMatWidth();++g) {
           short int x_0, y_0, x_n, y_n;
           if (k - zone/2 <= 0)
               x_0 = zone / 2 - abs(k-zone/2);
           else
               x_0 = k-zone/2;
           if (g - zone / 2 <= 0)
               y_0 = zone / 2 - abs(g-zone/2);
           else
               y_0 = g - zone / 2;
           if (k + zone / 2 > mat.GetMatHeight())
               x_n = mat.GetMatHeight() - 1;
           else
               x_n = k + zone / 2;;
           if (g + zone / 2 > mat.GetMatWidth())
               y_n = mat.GetMatWidth() - 1;
           else
               y_n = g + zone / 2;
            Coordinate currentPix{ k, g }, upleftCoord{x_0, y_0}, downrightCoord{x_n, y_n};
            Matrix localZoneMat(y_n - y_0 + 1, x_n - x_0 + 1);
            
            for (int i = x_0; i < x_n; ++i)
            {
                for (int j = y_0; j < y_n; ++j)
                {
                    localZoneMat.SetPixel(i-x_0, j-y_0, newmat.GetPixel(i, j));
                }
            }
            mat.SetPixel(k, g, makeSmoothPixel(localZoneMat, currentPix, upleftCoord));
        }
    }
    return mat;
}


Pixel newPix(Matrix mat)
{
    Pixel newpix{};
    int sumRed=mat.GetPixel(1,1).GetRed()*5, sumGreen = mat.GetPixel(1, 1).GetGreen() * 5, sumBlue = mat.GetPixel(1, 1).GetBlue() * 5;
    for (int i = 0; i < 3; i++) {
        for (int j = 0+(i+1)%2; j < 3; j++) {
           sumRed-= mat.GetPixel(i, j).GetRed();
           sumGreen -= mat.GetPixel(i, j).GetGreen();
           sumBlue -= mat.GetPixel(i, j).GetBlue();
        }
    }
    if (sumRed > 255)
        sumRed = 255;
    if (sumBlue > 255)
        sumBlue = 255;
    if (sumGreen > 255)
        sumGreen = 255;
    if (sumRed > 0)
        sumRed = 0;
    if (sumBlue > 0)
        sumBlue = 0;
    if (sumGreen > 0)
        sumGreen = 0;
    newpix.SetPixel(sumRed,sumGreen,sumBlue);
    return newpix;
}

Matrix filterBorder(Matrix mat)
{
    Matrix newmat=mat;
    for (int i=1; i<mat.GetMatHeight()-1;i++){
        for (int j =1; j<mat.GetMatWidth()-1;j++){
            //Coordinate currentPix{ i, j }, upleftCoord{ i-1, j-1 }, downrightCoord{ i+1, j+1 };
            Matrix zonemat(3,3);
            for (int k = -1; k < 2; k++) {
                for (int g = -1; g < 2;g++) {
                    zonemat.SetPixel(k + 1, g + 1, mat.GetPixel(i+k, j+g));
                }
            }            
            newmat.SetPixel(i, j, newPix(zonemat));
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
            case 's': matrix = filterSmooth(matrix, stod(arg[i + 1])); i++; break; // Blure
            case 'b': matrix = filterBorder(matrix);
            }
        }
    }

    return matrix;
}