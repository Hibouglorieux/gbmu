#ifndef TILEPIXELS_HPP
#define TILEPIXELS_HPP

#include <array>
#include <algorithm>
#include "Gameboy.hpp"

struct TilePixels {
    std::array<std::array<int, 8>, 8> data{};

    void flipX() {
        for (int i = 0; i < 8; i++)
            std::reverse(data[i].begin(), data[i].end());
    }
    void flipY() {
        std::reverse(data.begin(), data.end());
    }

    static int getColor(unsigned char byteColorCode, unsigned short paletteAddress);
	std::array<int, 8> getColorLine(int y);
	std::array<int, 8> getLineColorCode(int y);

	TilePixels();
    TilePixels(std::array<std::array<int, 8>, 8> val);
    TilePixels(int tileAddress, unsigned short newPaletteAddress);
	unsigned short paletteAddress;
};

#endif
