#pragma once
#include <iostream>
#include <fstream>
#include "Foundation.h"

void readBytes(std::ifstream& stream, byte* begin, std::streamsize count) {
    bool success{};

    if (stream.read(reinterpret_cast<char*>(begin), count)) {
        success = stream.gcount() == count;
    }

    if (!success) {
        throw std::runtime_error("read failed");
    }
}