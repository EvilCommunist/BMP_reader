#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "Pixel.h"
#include "Matrix.h"

bool Matrix::CoordIsValid(unsigned int i, unsigned int j) const
{
    return ((i < height) && (j < width));
}
int Matrix::GetCoord(int i, int j) const
{
    if (CoordIsValid(i, j))
    {
        return (i * width + j);
    }
    else
    {
        throw std::runtime_error("Attempt to call coordinate out of range");
    }
}

Matrix::Matrix(size_t widthread, size_t heightread){
    width = widthread;
    height = heightread;
    image.resize(width * height);
}
void Matrix::setMatrix(Matrix mat){
    image.resize(mat.GetMatWidth()* mat.GetMatHeight());
    width = mat.GetMatWidth();
    height = mat.GetMatHeight();
}

void Matrix::SetPixel(unsigned int i, unsigned int j, Pixel pix){
    image[GetCoord(i, j)] = pix;
}
Pixel Matrix::GetPixel(unsigned int i, unsigned int j) const{
    return image[GetCoord(i, j)];
}

size_t Matrix::GetMatHeight(){
    return height;
}
size_t Matrix::GetMatWidth(){
    return width;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool MatMatrix::CoordIsValid(unsigned int i, unsigned int j) const
{
    return ((i < height) && (j < width));
}
int MatMatrix::GetCoord(int i, int j) const
{
    if (CoordIsValid(i, j))
    {
        return (i * width + j);
    }
    else
    {
        throw std::runtime_error("Attempt to call coordinate out of range");
    }
}

MatMatrix::MatMatrix(size_t widthread, size_t heightread) {
    width = widthread;
    height = heightread;
    matrix.resize(width * height);
}
void MatMatrix::setMatrix(MatMatrix mat) {
    matrix.resize(mat.GetMatWidth() * mat.GetMatHeight());
    width = mat.GetMatWidth();
    height = mat.GetMatHeight();
}


void MatMatrix::SetValue(unsigned int i, unsigned int j, long double value) {
    matrix[GetCoord(i, j)] = value;
}
long double MatMatrix::GetValue(unsigned int i, unsigned int j) {
    return matrix[GetCoord(i, j)];
}

size_t MatMatrix::GetMatHeight() {
    return height;
}
size_t MatMatrix::GetMatWidth() {
    return width;
}