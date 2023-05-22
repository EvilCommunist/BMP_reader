#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <iterator>
#include <windows.h>
#include <TCHAR.h>
#include <vector>

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

template <class T, size_t size>
std::array<T, size> readArray(std::ifstream& stream) {
    std::array<T, size> array;
    byte* tmpAsBytes = reinterpret_cast<byte*>(array.data());
    readBytes(stream, tmpAsBytes, sizeof(T) * size);
    return array;
}


template <class T>
T takeNextField(std::ifstream& stream, const char* name) {
    const auto val = read<T>(stream);
    return val;
}

 //return смещение до начала пиксельных данных
 
uint32_t takeHeadInf(std::ifstream& input) {
    const auto sign = readArray<uint8_t, 2>(input);
    std::cout << "Сигнатура: " << sign.front() << sign.back() << std::endl;

    takeNextField<uint32_t>(input,"Размер файла (байт)");

    const auto reserved = readArray<uint16_t, 2>(input);
    std::cout << "Зарезервированные поля (ожидается 0): " << reserved.front() << " " << reserved.back() << std::endl;

    return takeNextField<uint32_t>(input,"Смещение (в байтах)");
}

struct Size {
    int32_t width;
    int32_t height;
};

Size takeBitMapInfo(std::ifstream& input) { 
    uint32_t struck = takeNextField<uint32_t>(input,"Размер (в байтах) структуры");
     
    if ((struck!=12) && (struck != 40) && (struck != 108) && (struck != 124)) // Проверка на валидность типа БМП
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

    const auto width = takeNextField<int32_t>(input,"width");
    const auto height = takeNextField<int32_t>(input,"height");
    return { width, height };
} 

class Pixel
{
private:
    uint8_t red;
    uint8_t green;
    uint8_t blue;
public:

    void SetPixel(uint8_t r, uint8_t g, uint8_t b)
    {
        red = r;
        green = g;
        blue = b;
    }

    void PrintPixel(std::ostream& out)
    {
        out << "(" << (unsigned)red << "," << (unsigned)green << "," << (unsigned)blue << ")" << " ";
    }
};

class Matrix {
private:
    
    std::vector<Pixel> image;

    size_t width;

    size_t height;

    bool CoordIsValid(unsigned int i, unsigned int j) const 
    {
        return ((i < height) && (j < width));
    }

    int GetCoord(int i, int j) const
    {
        if (CoordIsValid(i, j))
        {
            return i * width + j;
        }
        else
        {
            return width*height;
        }
    }

public:

    Matrix(size_t widthread, size_t heightread)
    {
        width = widthread;
        height = heightread;
        image.resize(width*height);
    }

    void SetPixel(unsigned int i, unsigned int j, Pixel pix)
    {
        image[GetCoord(i, j)] = pix;
    }

    Pixel GetPixel(unsigned int i, unsigned int j) const
    {
        return image[GetCoord(i, j)];
    }

    size_t GetMatHeight()
    {
        return height;
    }

    size_t GetMatWidth()
    {
        return width;
    }
};
//////////////////////////////////////////////////////////////////////////
Matrix addFilters(std::vector<std::string> arg1, Matrix matr)
{
    std::vector<std::string> arg = arg1;
    Matrix matrix = matr;
    for (int i = 0; i < arg.size(); i++)
    std::cout << arg[i] << "   ";
    std:: cout << initializeargument(arg[0]);
   /* for (size_t i = 0; i<arg.size(); i++)
    {
        if (arg[i][0]=='-') // InitializeToken
        {
            char c = initializeargument(arg[i]);
            switch (c)
            {
            case 'c': filterCrop(matrix, stod(arg[i + 1]), stod(arg[i + 2])); i += 2; break;
            case 'g': break;
            }
        }
    }
    Matrix matrya = matrix;*/
    return matrix;
}

char initializeargument(std::string arg)
{
    // В виду наличия аргументов с разными первыми буквами ограничимся выводом первой буквы аргумента
    return arg[1];
}
/*
Matrix filterCrop(Matrix mat, size_t newwidth, size_t newheight)
{
    Matrix newmat(newwidth, newheight);
    for (size_t i = 0; i < mat.GetMatHeight(); ++i) {
        for (size_t j = 0; j < mat.GetMatWidth(); ++j) {
            newmat.SetPixel(i, j, mat.GetPixel(i, j));
        }
    }
    return newmat;
}*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
 
    addFilters(arg, mat);

    return mat;
}

void printMatrix(const Matrix& matrix, std::ostream& out) // Вывод матрицы
{
    Matrix mat = matrix;

    for (size_t i = 0; i < mat.GetMatHeight(); ++i) {
        for (size_t j = 0; j < mat.GetMatWidth(); ++j) {
            mat.GetPixel(i, j).PrintPixel(out);
        }
        out << std::endl;
    }
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