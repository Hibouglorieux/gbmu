/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TilePixels.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/30 22:27:00 by nallani           #+#    #+#             */
/*   Updated: 2022/12/31 03:05:34 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TilePixels.hpp"

#define BGP (0xFF47)
// background palette 0b11000000 => 11, 0b110000 => 10, 0b1100 => 01, 0b11 => 00
// only used in DMG

/*
TilePixels::TilePixels(std::array<std::array<short, 8>, 8> val)
{
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			data[y][x] = val[y][x];
		}
	}
}
*/

// this should get the 4 colors of a cgb palette
unsigned long TilePixels::getCGBPaletteColor(unsigned char paletteNb)
{
	unsigned long colors = 0;
	const unsigned char bytePerPalette = 8;
	const unsigned char bytePerColor = 2;
	for (int i = 0; i < 4; i++)
	{
		// there are 0x3F/64 total colors
		// first byte(0) correspond to low byte of 1rst color of palette 0
		// second(1) of high byte of 1rst color of palette 0
		// third(2) of low byte of 2nd color of palette 0
		// ... 9th byte (0x08) should be low byte of 1rst of color of palette 1
		// there are 8 bytes per 
		unsigned char low = mem.getBGPalette()[paletteNb * bytePerPalette + i * bytePerColor];
		unsigned char high = mem.getBGPalette()[paletteNb * bytePerPalette + i * bytePerColor + 1];
		unsigned short color = (high << 8) | low;
		colors |= ((long)color << (i * 16));
	}
	return colors;
}

short TilePixels::getColor(unsigned char byteColorCode, unsigned long paletteColor)
{
	//TODO, especially for CGB and to convert with SDL color !
	//
	// GB color are encoded in 4 bits : 00, 01, 10 , 11
	// 4 shade
	// SDL wrapper only need to wrap that for now	
	// TODO CGB encode 5bits RGB for color in 2 bytes.
	int color = 0;
	if (!Gameboy::bIsCGB)
	{
		unsigned char bitPosInPalette = byteColorCode == 0b11 ? 6 : byteColorCode == 0b10 ? 4 : byteColorCode == 0b01 ? 2 : 0;
		color = paletteColor & (0b11 << bitPosInPalette);
		color >>= bitPosInPalette;
	}
	else
	{
		unsigned char colorShortPos = byteColorCode == 0b11 ? 3 : byteColorCode == 0b10 ? 2 : byteColorCode == 0b01 ? 1 : 0;
		color = (paletteColor & (0xFFFF << (colorShortPos * 16))) >> (16 * colorShortPos) & 0xFFFF;
		//std::cout << "i have color: " << color << "with byte code: " << +byteColorCode << std::endl;
	}
	return color;
}

std::array<short, 8> TilePixels::getColorLine(int y)
{
	std::array<short, 8> retLine;
	if (!bIsValid)
	{
		retLine.fill(0xFFFF);
		return retLine;
	}
	unsigned long paletteColor = 0;

	// means the tile is valid, else return 0
	if (!Gameboy::bIsCGB)
	{
		paletteColor = mem[BGP];
	}
	else
	{
		unsigned char attribute = mem.getCGBVram()[mapAddr - 0x8000];
		unsigned char paletteNb = attribute & 0b111;
		paletteColor = getCGBPaletteColor(paletteNb);
		//std::cout << "with palette number: " << +paletteNb << " with color: " << paletteColor << std::endl;
	}
	for (int x = 0; x < 8; x++)
		retLine[x] = getColor(data[y][x], paletteColor);
	return retLine;
}

std::array<short, 8> TilePixels::getLineColorCode(int y)
{
	std::array<short, 8> tmp{};
	if (!bIsValid)
		return tmp;
	for (int x = 0; x < 8; x++) {
		tmp[x] = data[y][x];
	}
	return tmp;
}

TilePixels::TilePixels()
{
	bIsValid = false;
}

TilePixels::TilePixels(unsigned short tileAddress, unsigned short mapAddress) : data()
{
	bIsValid = true;
	mapAddr = mapAddress;
	unsigned char* vram = mem.getVram();
	if (Gameboy::bIsCGB)
	{
		unsigned char attribute = mem.getCGBVram()[mapAddr - 0x8000];
		if (BIT(attribute, 3))
			vram = mem.getCGBVram();
	}
	for (int y = 0; y < 8; y++) {

		unsigned char byte1 = vram[tileAddress + (y * 2) - 0x8000];
		unsigned char byte2 = vram[tileAddress + (y * 2) + 1 - 0x8000];
		if (tileAddress >= 0x9800 || tileAddress < 0x8000)
			std::cerr << "access vram not at vram: "<< tileAddress << std::endl;

		for (int x = 0; x < 8; x++)
		{
			// color code is based on the merge of the two bytes with the same bit
			// 0b10001001 0b00010011 will give 0x10, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01 and 0x11
			bool bit1 = byte1 & (1 << x);
			bool bit2 = byte2 & (1 << x);
			unsigned char byteColorCode = (bit2 << 1) | (bit1);
			data[y][7 - x] = byteColorCode;
		}
	}
	if (Gameboy::bIsCGB)
	{
		unsigned char attribute = mem.getCGBVram()[mapAddr - 0x8000];
		if (BIT(attribute, 6))
			flipY();
		if (BIT(attribute, 5))
			flipX();
	}
}

bool TilePixels::isAboveOAM() const
{
	if (!Gameboy::bIsCGB)
		return false;
	unsigned char attribute = mem.getCGBVram()[mapAddr - 0x8000];
	return BIT(attribute, 7);
}
