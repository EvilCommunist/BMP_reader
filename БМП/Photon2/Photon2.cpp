#include <iostream>
#include <fstream>
#include <array>
#include <iterator>

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
    char* tmpAsBytes = reinterpret_cast<char*>(&tmp);
    stream.read(tmpAsBytes, sizeof(T));
    return tmp;
}

/**
 * Ленюсь писать полный комментарий. Но очень желательно. Обрати внимание, возвращаемый тип описал,
 * т.к. он вовсе неочевиден из названия
 * \param input ...
 * \param output ...
 * \return смещение до начала пиксельных данных
 */
uint32_t dumpFileHeader(std::ifstream& input, std::ostream& output) {
    const auto sign = read<std::array<uint8_t, 2>>(input);
    output << "Сигнатура: " << sign.front() << sign.back() << std::endl;

    output << "Размер файла (байт): " << read<uint32_t>(input) << std::endl;

    const auto reserved = read<std::array<uint16_t, 2>>(input);
    output << "Зарезервированные поля (ожидается 0): " << reserved.front() << " " << reserved.back() << std::endl;

    const auto offset = read<uint32_t>(input);
    output << "Смещение (в байтах): " << offset << std::endl;
    return offset;
}

struct Size {
    int32_t width;
    int32_t height;
};

Size dumpBitmapInfo(std::ifstream& input, std::ostream& output) {
    const auto structSize = read<uint32_t>(input);
    output << "Размер (в байтах) структуры: " << structSize << std::endl;

    //TODO: check core/3/4/5
    // Сначала - ширина. У тебя было наоборот
    const auto width = read<int32_t>(input);
    const auto height = read<int32_t>(input);
    output << "width x height: " << width << " " << height << std::endl;

    output << "Проверка курсора (должно быть 1): " << read<uint16_t>(input) << std::endl;
    output << "Число бит на пиксель: " << read<uint16_t>(input) << std::endl;
    output << "Способ хранения пикселей (должно быть 0): " << read<uint16_t>(input) << std::endl;
    output << "Размер пиксельных данных (или 0), байт: " << read<uint32_t>(input) << std::endl;

    output << "Кол-во пикселей на метр (x, y)" << read<int32_t>(input) << " " << read<int32_t>(input) << std::endl;

    output << "Размер таблицы цветов " << read<uint32_t>(input) << std::endl;
    output << "Количество ячеек от начала таблицы цветов до последней используемой " << read<uint32_t>(input) << std::endl;
    return {width, height};
}

void dumpAsText(std::ifstream& input, std::ostream& output) {
    const uint32_t pixelsOffset = dumpFileHeader(input, output);
    const Size size = dumpBitmapInfo(input, output);

    const size_t pixelsBytesPerLine = size.width * 3;
    const size_t paddingBytes = pixelsBytesPerLine % 4;
    // полный размер строки в байтах = pixelsBytesPerLine + paddingBytes

    input.seekg(pixelsOffset);

    for (size_t i = 0; i < size.height; ++i) {
        for (size_t j = 0; j < size.width; ++j) {
            output << "("
                   << (unsigned) read<uint8_t>(input) << ","
                   << (unsigned) read<uint8_t>(input) << ","
                   << (unsigned) read<uint8_t>(input) << ") ";
        }

        for (size_t pad = 0; pad != paddingBytes; ++pad) {
            read<uint8_t>(input);
        }
        output << std::endl;
    }
}

int main() {
    setlocale(LC_CTYPE, "Russian");
    std::string picture = "/home/andrey/Downloads/Telegram Desktop/lenna.bmp";
    std::ifstream input;
    input.open(picture, input.binary | input.in);
    std::string numfile = "C:\\Users\\AT241\\OneDrive\\Рабочий стол\\Pazhiloy\\test2.txt";
    std::ofstream output;
    output.open(numfile, output.out);

    //dumpAsText(input, output);
    dumpAsText(input, std::cout);
}