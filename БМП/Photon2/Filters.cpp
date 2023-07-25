#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <iterator>
#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
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
    if (arg == "-contr")
        return 'o';
    return arg[1];
}


Matrix filterCrop(Matrix mat, size_t newwidth, size_t newheight){
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


Matrix filterGs(Matrix mat){
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


Pixel makeSmoothPixel(Matrix mat, Coordinate cp, Coordinate ulp, int z) {
    Pixel pix = mat.GetPixel(cp.height-ulp.height, cp.width- ulp.width);
    int pixRed=pix.GetRed(), pixGreen = pix.GetGreen(), pixBlue = pix.GetBlue(), summ = 1;
    for (int i = 0; i < mat.GetMatHeight(); ++i)
    {
        for (int j = 0; j < mat.GetMatWidth(); ++j)
        {
            if (((i == cp.height - ulp.height))&&((j==cp.width - ulp.width)))
               continue;
            pixRed += mat.GetPixel(i, j).GetRed();
            pixGreen += mat.GetPixel(i, j).GetGreen();
            pixBlue += mat.GetPixel(i, j).GetBlue();
            summ++;
        }
    }

    pixRed = pixRed / summ;
    pixGreen = pixGreen / summ;
    pixBlue = pixBlue / summ;
    if (pixBlue > 255)
        pixBlue = 255;
    if (pixRed > 255)
        pixRed = 255;
    if (pixGreen > 255)
        pixGreen = 255;
    pix.SetPixel(pixRed, pixGreen, pixBlue);
    return pix;
}

Matrix filterSmooth(Matrix mat, int zone) { // Размытие работает, но изображение сильно сереет, непонятно в чём дело
    if (zone % 2 == 0)
        throw std::runtime_error("Используйте нечётную зону размытия");

    Matrix newmat = mat;
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
           if (k + zone / 2 >= mat.GetMatHeight())
               x_n = mat.GetMatHeight() - 1;
           else
               x_n = k + zone / 2;;
           if (g + zone / 2 >= mat.GetMatWidth())
               y_n = mat.GetMatWidth() - 1;
           else
               y_n = g + zone / 2;
            Coordinate currentPix{ k, g }, upleftCoord{x_0, y_0}, downrightCoord{x_n, y_n};
            Matrix localZoneMat(y_n - y_0+1, x_n - x_0+1);
            
            for (int i = x_0; i < x_n+1; ++i)
            {
                for (int j = y_0; j < y_n+1; ++j)
                {
                    localZoneMat.SetPixel(i-x_0, j-y_0, newmat.GetPixel(i, j));
                }
            }
            mat.SetPixel(k, g, makeSmoothPixel(localZoneMat, currentPix, upleftCoord, zone));
        }
    }
    return mat;
}


Pixel newPix(Matrix mat){
    Pixel newpix{};
    int sumRed=mat.GetPixel(1,1).GetRed()*5, sumGreen = mat.GetPixel(1, 1).GetGreen() * 5, sumBlue = mat.GetPixel(1, 1).GetBlue() * 5;
    for (int i = 0; i < 3; i++) {
        for (int j = 0+(i+1)%2; j < 3; j+=2) {
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
    if (sumRed < 0)
        sumRed = 0;
    if (sumBlue < 0)
        sumBlue = 0;
    if (sumGreen < 0)
        sumGreen = 0;
    newpix.SetPixel(sumRed,sumGreen,sumBlue);
    return newpix;
}

Matrix filterBorder(Matrix mat){
    Matrix newmat=mat;
    for (int i=1; i<mat.GetMatHeight()-1;i++){
        for (int j =1; j<mat.GetMatWidth()-1;j++){
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


Pixel contrFunc(Pixel pix){
    Pixel pixel=pix;
    int pixRed=pixel.GetRed(), pixGreen = pixel.GetGreen(), pixBlue = pixel.GetBlue();
    pixGreen = pixGreen * 1.5 - 22;
    pixBlue = pixBlue * 1.5 - 22;
    pixRed = pixRed * 1.5 - 22;
    if (pixRed > 255)
        pixRed = 255;
    if (pixRed < 0)
        pixRed = 0;
    if (pixGreen > 255)
        pixGreen = 255;
    if (pixGreen < 0)
        pixGreen = 0;
    if (pixBlue > 255)
        pixBlue = 255;
    if (pixBlue < 0)
        pixBlue = 0;
    pixel.SetPixel(pixRed, pixGreen, pixBlue);

    return pixel;
}

Matrix filterContrast(Matrix mat){
    Matrix newmat=mat;
    
    for (int i = 0; i < mat.GetMatHeight();i++) {
        for (int j = 0; j < mat.GetMatWidth(); j++) {
            newmat.SetPixel(i, j, contrFunc(mat.GetPixel(i, j)));
        }
    }
    return newmat;
}


Matrix changeMatrixDown(int i, int j, Matrix mat)
{
    Matrix matr = mat;
    //Нижнетреугольное преобразование
    for (int k = i; k < mat.GetMatHeight() - 1; k++)
    {
        long double koeff = -matr.GetValue(k + 1, j) / matr.GetValue(i, j);
        for (int n = j; n < matr.GetMatWidth(); n++)
        {
            long double newvalue = koeff * matr.GetValue(i, n) + matr.GetValue(k + 1, n);
            matr.SetValue(k + 1, n, newvalue);
        }
    }
    return matr;
}

std::vector<long double> gauss(Matrix mat) {
    std::vector<long double> ans{ 0,0,0,0 };
    for (int i = 0; i < mat.GetMatHeight(); i++)
    {
        for (int j = 0; j < mat.GetMatWidth(); j++)
        {
            std::cout << mat.GetValue(i, j) << "  ";
        }
        std::cout << std::endl;
    }
    for (int i = 0; i < mat.GetMatHeight() - 1; i++)
    {
        mat = changeMatrixDown(i, i, mat);
    }
    short int k = 3;
    long double buff{};
    for (int i = 4 - 1; i >= 0; i--) {
        buff = 0;
        for (int j = 4; j > i; j--) {
            buff += mat.GetValue(i, j - 1) * ans[j - 1];
        }
        ans[i] = (mat.GetValue(i, 4) - buff) / mat.GetValue(i, i);
    }
    return ans;
}

Matrix transpone(Matrix mat, short int angle1) {
    long double angle =  angle1 *M_PI/180;
    Matrix transponedImage(cos(angle) * mat.GetMatHeight() + cos(M_PI / 2 - angle) * mat.GetMatWidth(), sin(angle) * mat.GetMatHeight() + cos(angle) * mat.GetMatWidth());
    for (int i = 0; i < transponedImage.GetMatHeight(); i++) {
        for (int j = 0; j < transponedImage.GetMatWidth(); j++) {
            Pixel black{};
            transponedImage.SetPixel(i, j, black);
        }
    }
    Coordinate leftTop{ 0, mat.GetMatHeight() * cos(angle) - 1 }, rightTop{ mat.GetMatHeight() * sin(angle) - 1, 0 }, botRight{ transponedImage.GetMatWidth() - 1, mat.GetMatWidth() * cos(M_PI / 2 - angle) - 1 }, botLeft{ mat.GetMatWidth() * cos(angle) - 1, transponedImage.GetMatHeight() - 1 },
               rT{ mat.GetMatWidth()-1, 0 }, bR{ mat.GetMatWidth()-1 , mat.GetMatHeight()-1 }, bL{ 0, mat.GetMatHeight()-1 };
    std::vector<Coordinate> newCord {leftTop, rightTop, botRight, botLeft}; // Переназначить точки!!!!
    std::vector<Coordinate> oldCord{ {0,0}, rT, bR, bL};
    Matrix a_b_SLAU(5,4);
    
    //fillmatrix_x
    for (int i = 3; i >=0; i--) {
        a_b_SLAU.SetValue(i, 0, newCord[3-i].width);
        a_b_SLAU.SetValue(i, 1, newCord[3-i].height);
        a_b_SLAU.SetValue(i, 2, newCord[3-i].height* newCord[3-i].width);
        a_b_SLAU.SetValue(i, 3, 1);
        a_b_SLAU.SetValue(i, 4, oldCord[3-i].width);
    }
    std::vector<long double> angleValues_x{ 0,0,0,0 }, angleValues_y{0,0,0,0};
    angleValues_x = gauss(a_b_SLAU);

    //fillmatrix_y
    for (int i = 3; i >=0; i--) {
        a_b_SLAU.SetValue(i, 0, newCord[3-i].width);
        a_b_SLAU.SetValue(i, 1, newCord[3-i].height);
        a_b_SLAU.SetValue(i, 2, newCord[3-i].height * newCord[3-i].width);
        a_b_SLAU.SetValue(i, 3, 1);
        a_b_SLAU.SetValue(i, 4, oldCord[3-i].height);
    }
    angleValues_y = gauss(a_b_SLAU);

    for (int i = 0; i < transponedImage.GetMatHeight(); i++) {
        for (int j = 0; j < transponedImage.GetMatWidth(); j++) {
            int i1=0, j1=0;// Получить дельты на 4 пикселя и смешать их
            j1 = j * angleValues_x[0] + i * angleValues_x[1] + j*i* angleValues_x[2] + angleValues_x[3]+0.5;
            i1 = j * angleValues_y[0] + i * angleValues_y[1] + j * i * angleValues_y[2] + angleValues_y[3]+0.5;
            if((i1>=0)&&(i1<mat.GetMatHeight())&& (j1 >= 0) && (j1 < mat.GetMatWidth()))
                transponedImage.SetPixel(i, j, mat.GetPixel(i1, j1));
        }
    }

    return transponedImage;
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
            case 'o': matrix = filterContrast(matrix);
            case 't': matrix = transpone(matrix, stod(arg[i + 1]));
            }
        }
    }

    return matrix;
}