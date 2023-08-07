#pragma once
#include <iostream>
#include <fstream>
#include <array>
using byte = unsigned char;

void readBytes(std::ifstream& stream, byte* begin, std::streamsize count);

template <class T>
T read(std::ifstream& stream) {
    T tmp;
    readBytes(stream, reinterpret_cast<byte*>(&tmp), sizeof(T));
    return tmp;
}

template <class T, size_t head>
std::array<T, head> readArray(std::ifstream& stream) {
    std::array<T, head> array;
    byte* tmpAsBytes = reinterpret_cast<byte*>(array.data());
    readBytes(stream, tmpAsBytes, sizeof(T) * head);
    return array;
}


template <class T>
T takeNextField(std::ifstream& stream) {
    const auto val = read<T>(stream);
    return val;
}

//return смещение до начала пиксельных данных