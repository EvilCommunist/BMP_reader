#include <iostream>
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
T printNextField(std::ifstream& stream, std::ostream& output, const char* name) {
    const auto val = read<T>(stream);
    output << name << ": " << val << std::endl;
    return val;
}

 //return смещение до начала пиксельных данных
 
uint32_t dumpFileHeader(std::ifstream& input, std::ostream& output) {
    const auto sign = readArray<uint8_t, 2>(input);
    output << "Сигнатура: " << sign.front() << sign.back() << std::endl;

    printNextField<uint32_t>(input, output, "Размер файла (байт)");

    const auto reserved = readArray<uint16_t, 2>(input);
    output << "Зарезервированные поля (ожидается 0): " << reserved.front() << " " << reserved.back() << std::endl;

    return printNextField<uint32_t>(input, output, "Смещение (в байтах)");
}

struct Size {
    int32_t width;
    int32_t height;
};

Size dumpBitmapInfo(std::ifstream& input, std::ostream& output) {
    uint32_t struck = printNextField<uint32_t>(input, output, "Размер (в байтах) структуры");

    switch (struck) // Примерная обработка ошибки структуры
    {
        case 12:
            output << "Имя структуры: BITMAPCOREHEADER" << std::endl;
            throw std::runtime_error("cannot read this type of BMP");
            break;
        case 40:
            output << "Имя структуры: BITMAPINFOHEADER" << std::endl;
            break;
        case 108:
            output << "Имя структуры: BITMAPV4HEADER" << std::endl;
            throw std::runtime_error("cannot read this type of BMP");
            break;
        case 124:
            output << "Имя структуры: BITMAPV5HEADER" << std::endl;
            throw std::runtime_error("cannot read this type of BMP");
            break;
    }

    const auto width = printNextField<int32_t>(input, output, "width");
    const auto height = printNextField<int32_t>(input, output, "height");
    printNextField<uint16_t>(input, output, "Проверка курсора (должно быть 1)");
    printNextField<uint16_t>(input, output, "Число бит на пиксель");
    printNextField<uint32_t>(input, output, "Способ хранения пикселей (должно быть 0)");
    printNextField<uint32_t>(input, output, "Размер пиксельных данных (или 0), байт");
    printNextField<int32_t>(input, output, "Кол-во пикселей на метр (x)");
    printNextField<int32_t>(input, output, "Кол-во пикселей на метр (y)");
    printNextField<uint32_t>(input, output, "Размер таблицы цветов");
    printNextField<uint32_t>(input, output, "Количество ячеек от начала таблицы цветов до последней используемой");
    return { width, height };
} 

class Pixel
{
private:
    uint8_t red;
    uint8_t green;
    uint8_t blue;
public:

    void SetPixel(uint8_t &r, uint8_t &g, uint8_t &b)
    {
        red = r;
        green = g;
        blue = b;
    }

    int GetPixelRed()
    {
        return red;
    }

    int GetPixelGreen()
    {
        return green;
    }

    int GetPixelBlue()
    {
        return blue;
    }
};

class Matrix {
private:
    
    std::vector<Pixel> image;

public:
    
    void SetCoordinateData(Pixel p)
    {
            image.push_back(p);
    }
    
    Pixel GetCoordinateData(size_t& width,int& i, int& j)
    {
        return image[i* width +j];
    }
};

void dumpAsText(std::ifstream& input, std::ostream& output) {
    const uint32_t pixelsOffset = dumpFileHeader(input, output);
    const Size size = dumpBitmapInfo(input, output); 
    const size_t pixelsBytesPerLine = size.width * 3; 
    const size_t paddingBytes = pixelsBytesPerLine % 4;
    // полный размер строки в байтах = pixelsBytesPerLine + paddingBytes

    input.seekg(pixelsOffset); 

    uint8_t r, g, b;

    for (size_t i = 0; i < size.height; ++i) {
        for (size_t j = 0; j < size.width; ++j) {
            r = (unsigned)read<uint8_t>(input);
            g = (unsigned)read<uint8_t>(input);
            b = (unsigned)read<uint8_t>(input);
            SetPixel(r, g, b); // Ошибка компиляции, не понятно что делать
            output << "("
                << r << ","
                << g << ","
                << b << ") ";
        }

        for (size_t pad = 0; pad != paddingBytes; ++pad) {
            read<uint8_t>(input);
        }
        output << std::endl;
    }
}

int main (int files, char* data[])
{
    if (files != 3) throw std::runtime_error("input 2 paths next time");
    setlocale(LC_CTYPE, "Russian");
    //std::string picture = "C:\\Users\\AT241\\OneDrive\\Рабочий стол\\Pazhiloy\\lenna.bmp";
    std::ifstream input;
    std::string s1 = data[1];
    input.open(data[1], input.binary | input.in);
    // std::string numfile = "C:\\Users\\AT241\\OneDrive\\Рабочий стол\\Pazhiloy\\younoturr.txt";
    std::ofstream output;
    output.open(data[2], output.out);

    //dumpAsText(input, output);

    bool ok = input.is_open();
    if (ok)
    {
    dumpAsText(input, output);
    }
    else
    {
        std::cout << "Файл не открыт";
    }
 }