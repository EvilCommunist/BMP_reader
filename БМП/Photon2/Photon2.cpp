#include<iostream>
#include<fstream>
using namespace std;
int main()
{
    setlocale(LC_CTYPE, "Russian");
    //string picture = "C:\\Users\\AT241\\OneDrive\\Рабочий стол\\Pazhiloy\\lenna.bmp";
    string picture = "C:\\Users\\AT241\\OneDrive\\Рабочий стол\\Pazhiloy\\test2.bmp";
    fstream pic;   
    pic.open(picture, pic.binary | pic.in);
    //string numfile = "C:\\Users\\AT241\\OneDrive\\Рабочий стол\\Pazhiloy\\younoturrr.txt";
    string numfile = "C:\\Users\\AT241\\OneDrive\\Рабочий стол\\Pazhiloy\\test2.txt";
    fstream file;
    file.open(numfile, file.out);
    short int cnt; // Вспомогательный счётчик для считывания однотипных данных несколько раз подряд
    cnt = 0;
    uint16_t word; // Вспомогательная перееменная для прочтения данных типа WORD 
    while (cnt < 2) // Прочтение сигнатуры
    {
        if (cnt < 1)
            file << "Сигнатура: ";
        pic.read(reinterpret_cast<char*>(&word), sizeof(uint8_t));
        file << (char)word;
        cnt += 1;
    }
    uint32_t bytesize;
    // Прочтение размера
    pic.read(reinterpret_cast<char*>(&bytesize), sizeof(bytesize));
    file << endl << "Размер файла (байт): " << bytesize << endl;
    while (cnt < 6) // Проверка нулевых полей
    {
        pic.read(reinterpret_cast<char*>(&word), sizeof(uint8_t));
        file << "Зарезервированное поле " << cnt - 1 << " (проверка, длжно быть 0): " << (uint8_t)word << endl; // Вот здесь вылезает ошибка (без использования костылей)
        cnt += 1;
    }
    uint32_t changeplace;
    // Прочтение смещения
    pic.read(reinterpret_cast<char*>(&changeplace), sizeof(changeplace));
    file << "Смещение (в байтах): " << changeplace << endl << endl;
    uint32_t struck;
    // Прочтение структуры
    pic.read(reinterpret_cast<char*>(&struck), sizeof(uint8_t));
    file << "Размер (в битах) структуры: " << struck << endl;
    // Определение типа структуры
    switch (struck)
    {
    case 12: file << "Имя структуры: BITMAPCOREHEADER" << endl; break;
    case 40: file << "Имя структуры: BITMAPINFOHEADER" << endl; break;
    case 108: file << "Имя структуры: BITMAPV4HEADER" << endl; break;
    case 124: file << "Имя структуры: BITMAPV5HEADER" << endl; break;
    }
    long height, width;
    // Прочтение высоты
    pic.read(reinterpret_cast<char*>(&height), sizeof(uint8_t));
    file << "Высота (в пикселях): " << height << endl;
    // Прочтение ширины
    pic.read(reinterpret_cast<char*>(&width), sizeof(uint8_t));
    file << "Ширина (в пикселях): " << width << endl;
    // Прочтение "курсора"
    pic.read(reinterpret_cast<char*>(&word), sizeof(uint8_t));
    file << "Проверка курсора (должно быть 1): " << word << endl;
    uint16_t bpp;
    // Прочтение количества бит на пискель
    pic.read(reinterpret_cast<char*>(&bpp), sizeof(uint8_t));
    file << "Число бит на пиксель: " << bpp << endl;
    uint32_t savepic;
    // Прочтение способа хранения пикселей
    pic.read(reinterpret_cast<char*>(&savepic), sizeof(uint8_t));
    file << "Способ хранения пикселей: " << savepic << "   ///   BI_RGB (двумерный массив)" << endl; // В данном случае мы имеем способ BI_RGB (двумерный массив), примем это за факт.
    uint32_t picdata;
    // Прочтение размера пиксельных данных в байт
    pic.read(reinterpret_cast<char*>(&picdata), sizeof(uint8_t));
    file << "Размер пиксельных данных в байт: " << picdata << endl;
    long horpicpm, vertpicpm;
    // Прочтение кол-ва пикселей на метр по горизонтали
    pic.read(reinterpret_cast<char*>(&horpicpm), sizeof(uint8_t));
    file << "Кол-во пикселей на метр по горизонту: " << horpicpm << endl;
    // Прочтение кол-ва пикселей на метр по вертикали
    pic.read(reinterpret_cast<char*>(&vertpicpm), sizeof(uint8_t));
    file << "Кол-во пикскелей на метр по вертикали: " << vertpicpm << endl;
    uint32_t sizecolour;
    // Прочтение размера таблицы цветов
    pic.read(reinterpret_cast<char*>(&sizecolour), sizeof(uint8_t));
    file << "Размер таблицы цветов: " << sizecolour << endl;
    uint32_t lenghtcol;
    // Прочтение кол-ва ячеек от таблицы цветов до неё самой
    pic.read(reinterpret_cast<char*>(&lenghtcol), sizeof(uint8_t));
    file << "Кол-во ячеек от таблицы цветов до неё самой: " << lenghtcol << endl << "Заметим, что в данном случае цвета записаны в обратном порядке" << endl << endl << "Ниже представлена кодировка цветов:" << endl;
    //Можно сделать универсальное прочтение файла с помощью добавления условного счётчика типа "cond", но в данном случае принимаем "динамическое" прочтение BMP файла
    long size, h, w, i;
    size = width*bpp/8;
    long sizecheck = size;
    short int emtybytes = 4;
    short int ost = size % emtybytes;
    uint8_t colour;
    for (h=0; h<height; h++) //Непосредственно считывание кодировки цветов
    {
        for (w=0; w<width; w++)
        {
            if(sizecheck>ost)
            { 
                i = 0;
                file << "(";
                while (i < 3)
                {
                    pic.read(reinterpret_cast<char*>(&colour), sizeof(colour));
                    file << hex << (long)colour; //hex << украл для вывода десятичных формул.
                    if (i < 2)
                        file << ",";
                    i += 1;
                }
              file << ") ";
              sizecheck -= 3;
            }
            else
            {
                while (sizecheck >= 0) // Пропуск "дополненных" байтов
                {
                    pic.read(reinterpret_cast<char*>(&colour), sizeof(colour));
                    sizecheck -= 1;
                }
            }
        }
        w = 0;
        sizecheck = size;
    }
}
// Переименовать переменные, поправить типы, проверить. УБрать лишние буферы в считывании. Мусорные байты находтся вне длинны. 