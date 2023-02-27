#include<iostream>
#include<fstream>
using namespace std;
int main()
{
    setlocale(LC_CTYPE, "Russian");
    char binaar;
    uint8_t colour;
    unsigned int col, cnt, counter, i, size;
    char16_t she;
    size_t height, width, h, w;
    width = height = h= w = 0;
    //string picture = "C:\\Users\\AT241\\OneDrive\\Рабочий стол\\Pazhiloy\\lenna.bmp";
    string picture = "C:\\Users\\AT241\\OneDrive\\Рабочий стол\\Pazhiloy\\testtest.bmp";
    fstream pic;
    pic.open(picture, pic.binary | pic.in);
    //string numfile = "C:\\Users\\AT241\\OneDrive\\Рабочий стол\\Pazhiloy\\younoturrr.txt";
    string numfile = "C:\\Users\\AT241\\OneDrive\\Рабочий стол\\Pazhiloy\\testtest.txt";
    fstream file;
    file.open(numfile, file.out);
    cnt = counter = size = 0;
    while (cnt < 2) // Прочтение сигнатуры
    {
        if (cnt < 1)
            file << "Сигнатура: ";
        pic.read(reinterpret_cast<char*>(&binaar), sizeof(binaar));
        file << binaar;
        cnt += 1;
    }
    // Прочтение размера
    pic.read(reinterpret_cast<char*>(&col), sizeof(col));
    file << endl << "Размер файла (байт): " << col << endl;
    while (cnt < 6) // Проверка нулевых полей
    {
        pic.read(reinterpret_cast<char*>(&binaar), sizeof(binaar));
        file << "Зарезервированное поле " << cnt - 2 << " (проверка, длжно быть 0): " << (int)binaar << endl;
        cnt += 1;
    }
    // Прочтение смещения
    pic.read(reinterpret_cast<char*>(&col), sizeof(col));
    file << "Смещение (в байтах): " << col << endl << endl;
    // Прочтение структуры
    pic.read(reinterpret_cast<char*>(&col), sizeof(col));
    file << "Размер (в битах) структуры: " << col << endl;
    if (col < 50)
    {
        if (col < 25)
            file << "Имя структуры: BITMAPCOREHEADER" << endl;
        else
            file << "Имя структуры: BITMAPINFOHEADER" << endl;
    }
    else
    {
        if (col < 110)
            file << "Имя структуры: BITMAPV4HEADER" << endl;
        else
            file << "Имя структуры: BITMAPV5HEADER" << endl;
    }
    // Прочтение высоты
    pic.read(reinterpret_cast<char*>(&col), sizeof(col));
    file << "Высота (в пикселях): " << (long)col << endl;
    height = (long)col;
    // Прочтение ширины
    pic.read(reinterpret_cast<char*>(&col), sizeof(col));
    file << "Ширина (в пикселях): " << (long)col << endl;
    width = (long)col;
    // Прочтение "курсора"
    pic.read(reinterpret_cast<char*>(&she), sizeof(she));
    file << "Проверка курсора (должно быть 1): " << (int)she << endl;
    // Прочтение количества бит на пискель
    pic.read(reinterpret_cast<char*>(&she), sizeof(she));
    file << "Число бит на пиксель: " << (int)she << endl;
    int bpp = (int)she;
    // Прочтение способа хранения пикселей
    pic.read(reinterpret_cast<char*>(&col), sizeof(col));
    file << "Способ хранения пикселей: " << col << "   ///   BI_RGB (двумерный массив)" << endl; // В данном случае мы имеем способ BI_RGB (двумерный массив), примем это за факт.
    // Прочтение размера пиксельных данных в байт
    pic.read(reinterpret_cast<char*>(&col), sizeof(col));
    file << "Размер пиксельных данных в байт: " << col << endl;
    // Прочтение кол-ва пикселей на метр по горизонтали
    pic.read(reinterpret_cast<char*>(&col), sizeof(col));
    file << "Кол-во пикселей на метр по горизонту: " << (long)col << endl;
    // Прочтение кол-ва пикселей на метр по вертикали
    pic.read(reinterpret_cast<char*>(&col), sizeof(col));
    file << "Кол-во пикскелей на метр по вертикали: " << (long)col << endl;
    // Прочтение размера таблицы цветов
    pic.read(reinterpret_cast<char*>(&col), sizeof(col));
    file << "Размер таблицы цветов: " << col << endl;
    // Прочтение кол-ва ячеек от таблицы цветов до неё самой
    pic.read(reinterpret_cast<char*>(&col), sizeof(col));
    file << "Кол-во ячеек от таблицы цветов до неё самой: " << col << endl << "Заметим, что в данном случае цвета записаны в обратном порядке" << endl << endl << "Ниже представлена кодировка цветов:" << endl;
    //Можно сделать универсальное прочтение файла с помощью добавления условного счётчика типа "cond", но в данном случае принимаем "динамическое" прочтение BMP файла
    size = width*bpp/8;
    for (h=0; h<height; h++) //Непосредственно считывание кодировки цветов
    {
        for (w=0; w<width; w++)
        {
            if(size>2)
            { 
                i = 0;
                file << "(";
                while (i < 3)
                {
                    pic.read(reinterpret_cast<char*>(&colour), sizeof(colour));
                    file << hex << (long)colour;
                    if (i < 2)
                        file << ",";
                    i += 1;
                }
              file << ") ";
              size -= 3;
            }
            else
            {
                while (size > 0) // Пропуск "дополненных" байтов
                {
                    pic.read(reinterpret_cast<char*>(&colour), sizeof(colour));
                    size -= 1;
                }
            }
        }
        w = 0;
    }
}