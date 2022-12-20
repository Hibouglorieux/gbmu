#include "TilePixels.hpp"

TilePixels::TilePixels(std::array<std::array<int, 8>, 8> val) : data(val)
{}

int TilePixels::getColor(unsigned char byteColorCode, unsigned short paletteAddress)
{
	//TODO, especially for CGB and to convert with SDL color !
	//
	// GB color are encoded in 4 bits : 00, 01, 10 , 11
	// 4 shade
	// SDL wrapper only need to wrap that for now	
	// TODO CGB encode 5bits RGB for color in 2 bytes.
	unsigned char bitPosInPalette = byteColorCode == 0b11 ? 6 : byteColorCode == 0b10 ? 4 : byteColorCode == 0b01 ? 2 : 0;
	int color = mem[paletteAddress] & (0b11 << bitPosInPalette);
	color >>= bitPosInPalette;
	return color;
}

std::array<int, 8> TilePixels::getColorLine(int y)
{
	std::array<int, 8> retLine = getLineColorCode(y);

	for (int x = 0; x < 8; x++)
		retLine[x] = getColor(retLine[x], paletteAddress);
	return retLine;
}

std::array<int, 8> TilePixels::getLineColorCode(int y)
{
	return data[y];
}


TilePixels::TilePixels(int tileAddress, unsigned short newPaletteAddress) : data() {
	paletteAddress = newPaletteAddress;
    std::array<std::array<int, 8>, 8> pixels;
	for (int y = 0; y < 8; y++) {

		unsigned char byte1 = mem[tileAddress + (y * 2)];
		unsigned char byte2 = mem[tileAddress + (y * 2) + 1];
		if (tileAddress >= 0x9800 || tileAddress < 0x8000)
			std::cerr << "access vram not at vram: "<< tileAddress << std::endl;

		for (int x = 0; x < 8; x++)
		{
			// color code is based on the merge of the two bytes with the same bit
			// 0b10001001 0b00010011 will give 0x10, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01 and 0x11
			bool bit1 = byte1 & (1 << x);
			bool bit2 = byte2 & (1 << x);
			unsigned char byteColorCode = (bit2 << 1) | (bit1);
			pixels[y][7 - x] = byteColorCode;
		}
	}
    data = pixels;
}
