/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sprite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/17 18:56:00 by nathan            #+#    #+#             */
/*   Updated: 2022/12/19 16:31:02 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Sprite.hpp"
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
	for (unsigned char y = 0; y < spriteHeight; y++)
	{
		unsigned char byte1 = mem[spriteAddr + (y * 2)];
		unsigned char byte2 = mem[spriteAddr + (y * 2) + 1];
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

unsigned short Sprite::getPaletteAddr() const
{
	//TODO change that for CGB
	if (OAM_Data.getDMGPalette() == 1)
		return OBP1;
	else
		return OBP0;
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

int	Sprite::getColor(unsigned char byteColorCode) const
{
	//TODO, especially for CGB and to convert with SDL color !
	//
	// GB color are encoded in 4 bits : 00, 01, 10 , 11
	// 4 shade
	// SDL wrapper only need to wrap that for now	
	// TODO CGB encode 5bits RGB for color in 2 bytes.
	unsigned char bitPosInPalette = byteColorCode == 0b11 ? 6 : byteColorCode == 0b10 ? 4 : byteColorCode == 0b01 ? 2 : 0;
	int color = mem[getPaletteAddr()] & (0b11 << bitPosInPalette);
	color >>= bitPosInPalette;
	return color;
}

std::array<int, 8> Sprite::getColoredLine(int y) const
{
	std::array<int, 8> retLine = getLineColorCode(y);

	for (int x = 0; x < 8; x++)
		retLine[x] = getColor(retLine[x]);
	return retLine;
}