/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sprite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/17 18:56:00 by nathan            #+#    #+#             */
/*   Updated: 2023/01/02 22:01:24 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Sprite.hpp"
#include "define.hpp"
#include <algorithm>

Sprite::Sprite(OAM_entry newOAM, unsigned char newSpriteHeight)
{
	this->OAM_Data = newOAM;
	this->spriteHeight = newSpriteHeight;
	data.resize(spriteHeight);

	const unsigned short spritesAddrInVram = 0x8000;
	const int sizeOfSpriteInVram = 2 * 8;

	unsigned char tileIndex = OAM_Data.tileIndex;
	if (spriteHeight == 16) // ignore bit 0
		tileIndex = tileIndex & (0xFF - 1);
	int spriteAddr = spritesAddrInVram + (tileIndex * sizeOfSpriteInVram);
	if (spriteAddr >= 0x9800 || spriteAddr < 0x8000)
		std::cerr << "access vram not at vram: "<< spriteAddr << std::endl;
	const unsigned char* vram = mem.getVram();
	if (Gameboy::bIsCGB)
	{
		if (OAM_Data.getTileVramBank())
			vram = mem.getCGBVram();
	}
	for (unsigned char y = 0; y < spriteHeight; y++)
	{
		unsigned char byte1 = vram[spriteAddr + (y * 2) - 0x8000];
		unsigned char byte2 = vram[spriteAddr + (y * 2) + 1 - 0x8000];
		for (unsigned char x = 0; x < 8; x++)
		{
			bool bit1 = byte1 & (1 << x);
			bool bit2 = byte2 & (1 << x);
			unsigned char byteColorCode = (bit2 << 1) | (bit1);
			data[y][7 - x] = byteColorCode;
			// THIS RETURNS ONLY THE COLOR CODE
		}
	}
}

Sprite::~Sprite(void)
{}

unsigned long Sprite::getPaletteValue() const
{
	//TODO change that for CGB
	if (!Gameboy::bIsCGB)
	{
		if (OAM_Data.getDMGPalette() == 1)
			return mem[OBP1];
		else
			return mem[OBP0];
	}
	else
	{
		return TilePixels::getCGBPaletteColor(OAM_Data.getCGBPalette(), mem.getOBJPalettes());
	}
}

void Sprite::flipY()
{
	std::reverse(data.begin(), data.end());
}

void Sprite::flipX()
{
	for (unsigned int i = 0; i < data.size(); i++)
		std::reverse(data[i].begin(), data[i].end());
}

std::array<short, 8> Sprite::getColoredLine(int y) const
{
	std::array<short, 8> retLine = getLineColorCode(y);

	unsigned long paletteValue = getPaletteValue();
	for (int x = 0; x < 8; x++)
		retLine[x] = TilePixels::getColor(retLine[x], paletteValue);
	return retLine;
}
