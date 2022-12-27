#ifndef TILEPIXELS_HPP
#define TILEPIXELS_HPP

#include <array>
#include <algorithm>
#include "Gameboy.hpp"

struct TilePixels {
    int data[8][8];

    void flipX() {
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 4; x++) {
                int tmp = data[y][x];
                data[y][x] = data[y][7 - x];
                data[y][7 - x] = tmp;
            }
        }
    }
    void flipY() {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 8; x++) {
                int tmp = data[y][x];

                data[y][x] = data[7 - y][x];
                data[7 - y][x] = tmp;
            }
        }
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
