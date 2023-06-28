#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <iterator>
#include <windows.h>
#include <TCHAR.h>
#include <vector>

struct HeaderReader // ���������� ���� ����� ����� ������� ����� � ������� �������� ��������������. ������: ���������� ������ ����� � ������ � ���������� ������
{
    uint8_t sign1 = 'B';
    uint8_t sign2 = 'M';
    uint32_t fileSize;
    uint16_t reserve1 = 0;
    uint16_t reserve2 = 0;
    uint32_t paddToData = 54;
    uint32_t bmpType = 40;
    size_t width;
    size_t height;
    uint16_t cursor = 1;
    uint16_t bpp = 24; // bit per pixel
    uint32_t contain = 0;
    uint32_t pixDataSize;
    uint32_t ppmX = 2835; // pixel per meter
    uint32_t ppmY = 2835;
    uint32_t colorDataSize = 0;
    uint32_t cellsNum = 0; // ���-�� ����� �� ������ ������� ������ �� ��������� ������������
  /* HeaderReader()
    {
        sign1 = 'B';
        sign2 = 'M';
        reserve1 = reserve2 = 0;
        paddToData = 54;
        bmpType = 40;
        cursor = 1;
        ppmX = ppmY = 2835;
        colorDataSize = 0;
        cellsNum = 0;
        contain = 0;
        bpp = 24;
    }*/
};