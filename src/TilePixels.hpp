#ifndef TILEPIXELS_HPP
#define TILEPIXELS_HPP

#include <array>
#include <algorithm>
#include "Gameboy.hpp"

struct TilePixels {
    std::array<std::array<int, 8>, 8> data;

    std::array<int, 8> getLine(int y) { return data[y]; }
    void flipX() {
        for (int i = 0; i < 8; i++)
            std::reverse(data[i].begin(), data[i].end());
    }
    void flipY() {
        std::reverse(data.begin(), data.end());
    }

    std::array<int, 8> operator[](int y) { return getLine(y); }
    int getColor(unsigned char byteColorCode, int paletteAddress);

    TilePixels(std::array<std::array<int, 8>, 8> val);
    TilePixels(int tileAddress, int paletteAddress);
};

#endif