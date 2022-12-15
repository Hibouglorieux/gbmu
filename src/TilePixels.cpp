#include "TilePixels.hpp"

TilePixels::TilePixels(std::array<std::array<int, 8>, 8> val) : data(val)
{}

int TilePixels::getColor(unsigned char byteColorCode, int paletteAddress)
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

TilePixels::TilePixels(int tileAddress, int paletteAddress) : data() {
    std::array<std::array<int, 8>, 8> pixels;
	for (int y = 0; y < 8; y++) {

		unsigned char byte1 = mem[tileAddress + (y * 2)];
		unsigned char byte2 = mem[tileAddress + (y * 2) + 1];
		if (tileAddress >= 0x9800 || tileAddress < 0x8000)
			std::cerr << "access vram not at vram: "<< tileAddress << std::endl;

		for (int x = 0; x < 8; x++)
		{
			// get color based on the merge of the two bytes with the same bit
			// 0b10001000 0b00010010 will give 0x10, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01 and 0x00
			bool bit1 = byte1 & (1 << x);
			bool bit2 = byte2 & (1 << x);
			unsigned char byteColorCode = (bit1 << 1) | (bit2);
			pixels[y][7 - x] = getColor(byteColorCode, paletteAddress);
		}
	}
    data = pixels;
}