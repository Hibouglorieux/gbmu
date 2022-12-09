/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ppu.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 19:58:01 by nallani           #+#    #+#             */
/*   Updated: 2022/12/08 23:54:44 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ppu.hpp"
#include "Mem.hpp"
#include <algorithm>
#include <iostream>

Mem* Ppu::mem = nullptr;

void	Ppu::setMem(Mem& cpuMem)
{
	mem = &cpuMem;
}

std::array<int, NB_LINES> Ppu::doOneLine()
{
	int currentLine = (*mem)[LY];
	auto pixelLine = getOamLine(currentLine);
	auto backgroundLine = getBackgroundLine(currentLine);
	std::array<int, NB_LINES> finalLine = {};
	for (int i = 0; i < NB_LINES; i++)
	{
		if (pixelLine[i].bShouldBeDisplayed)
			finalLine[i] = pixelLine[i].color;
		else
			finalLine[i] = backgroundLine[i];
	}
	return finalLine;
}

int	Ppu::getPaletteFromOAMFlags(unsigned char flags)
{
	//this is for DMG only !!
	return ((flags & (1 << 4)) != 0) ? OBP1 : OBP0;
	// TODO add gameboy flags (bit 	0-2 OPB0 to OBP7)
}

int Ppu::getSpriteAddressInVRam(int spriteAddrInOAM, unsigned char spriteHeight)
{
   	// DMG ONLY !
	// TODO add gameboy additionnal bank !
	// which might not be 0x8000 (to see with Mem class)

	int spriteNumber = (*mem)[spriteAddrInOAM + 2];
	// bool bank1 = (*mem)[spriteAddr + 4] & (1 << 3);
	// 0x8000 is start of OBJ Vram, multiplied by the number of the tile * size of sprite
	// sprite is spriteHeight * 2 byte long, added
	return 0x8000 + spriteNumber * spriteHeight * 2;
}

std::array<int, 8> Ppu::getTilePixels(int tileAddress, unsigned char yOffset, int paletteAddress)
{
	// fetch the 8 pixel of a tile 
	std::array<int, 8> tilePixels;
	int lineOfPixelsAddress = tileAddress + yOffset * 2;// looking for real byte
							 // yOffset * 2 because
							 // there are two byte per
							 // "line" of pixel
							 // TODO might need to think of gameboy bank ?
	unsigned char byte1 = (*mem)[lineOfPixelsAddress];
	unsigned char byte2 = (*mem)[lineOfPixelsAddress + 1];
	if (lineOfPixelsAddress >= 0x9800 || lineOfPixelsAddress < 0x8000)
		std::cerr << "access vram not at vram: "<< lineOfPixelsAddress << std::endl;
	for (int x = 0; x < 8; x++)
	{
		// get color based on the merge of the two bytes with the same bit
		// 0b10001000 0b00010010 will give 0x10, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01 and 0x00
		bool bit1 = byte1 & (1 << x);
		bool bit2 = byte2 & (1 << x);
		unsigned char byteColorCode = (bit1 << 1) | (bit2);
		tilePixels[7 - x] = getColor(byteColorCode, paletteAddress);
	}
	return tilePixels;
}

std::array<int, NB_LINES> Ppu::getBackgroundLine(int yLineToFetch)
{
	std::array<int, NB_LINES> backgroundLine;
	bool bWindowEnabled = M_LCDC & (1 << 5);
	bool bBackgroundEnabled = M_LCDC & 1;
	int BGTileIt = 0;
	int xPosInLine = 0;
	bool bDrawWindow = bWindowEnabled && M_LY >= M_WY && xPosInLine >= (M_WX - WX_OFFSET);
	while (xPosInLine < 160)
	{
		std::array<int, 8> tilePixels;
		if (bDrawWindow)
		{
			tilePixels = getWindowTile((xPosInLine + WX_OFFSET - M_WX) / 8, yLineToFetch - M_WY);// should not underflow/panic because of windowDraw bool
		}
		else if (bBackgroundEnabled)
		{
			tilePixels = getBackgroundTile(BGTileIt + M_SCX / 8, (yLineToFetch + M_SCY) / 8, (yLineToFetch + M_SCY) % 8);
			BGTileIt++;
		}
		for (int i = 0; i < 8; i++)
		{
			if (!bDrawWindow && i + xPosInLine < (M_SCX % 8))// skip pixel if SCX % 8 != 0
				continue;				 // if scx == 3 then skip the
									 // first 3 pixels
			backgroundLine[xPosInLine++] = tilePixels[i];
			if (xPosInLine >= 159)
				return backgroundLine;
			// check if window should be enabled,
			// if the condition is met restart draw at that pos
			// make sure the window has to be rendered with its WX/WY
			// make sure window is in on x axis,
			// WX == 0x07 and WY == 0x00 means the window will be at the top left of the screen
			if (bWindowEnabled && yLineToFetch >= M_WY && xPosInLine >= (M_WX - WX_OFFSET))
			{
				bDrawWindow = true;
				break;
			}
		}
	}
	return backgroundLine;
}

int Ppu::getColor(unsigned char byteColorCode, int paletteAddress)
{
	//TODO, especially for CGB and to convert with SDL color !
	//
	// GB color are encoded in 4 bits : 00, 01, 10 , 11
	// 4 shade
	// SDL wrapper only need to wrap that for now	
	// TODO CGB encode 5bits RGB for color in 2 bytes.
	unsigned char bitPosInPalette = byteColorCode == 0b11 ? 6 : byteColorCode == 0b10 ? 4 : byteColorCode == 0b01 ? 2 : 0;
	int color = (*mem)[paletteAddress] & (0b11 << bitPosInPalette);
	color >>= bitPosInPalette;
	return color;
}

std::array<SpriteData, NB_LINES> Ppu::getOamLine(int yLineToFetch)
{
	std::vector<int> spritesFound;
	std::array<SpriteData, NB_LINES> spriteLine;
	if (!BIT(M_LCDC, 1)) // if OBJ flag isnt enabled, return empty array
	{
		spriteLine.fill({0, false});
		return spriteLine;
	}
	const int OAM_Addr = 0xFE00;
	unsigned char spriteHeight = BIT(M_LCDC, 2) ? 16 : 8; // type of sprite from flag

	// 1 - fetch the sprites needed for that line
	for (int i = 0; i < 40; i++)
	{
		int address = OAM_Addr + (i * 4);	
		struct OAM_entry *entry = (struct OAM_entry *)(&(*mem)[address]);

		unsigned char firstPixelDrawn = entry->posY - 16;
		unsigned char lastPixelDrawn = entry->posY - 16 + spriteHeight;

		// verify if the sprite should be rendered on this line
		// if posX == 0 then sprite is totally off the screen.
		// same goes for posY that starts at 0x10 (as it can be 16 height)
		if (firstPixelDrawn <= yLineToFetch &&
				lastPixelDrawn > yLineToFetch
				&& entry->posX > 0)
		{
			spritesFound.push_back(address);
			if (spritesFound.size() >= 10) // exit if we already found 10 sprites to render
				break;
		}
	}

	// 2 -reverse sort sprites so that the first (in X drawn order) will be drawn fully
	std::sort(spritesFound.begin(), spritesFound.end(), [](int a, int b){
			return (*mem)[b + 1] >= (*mem)[a + 1] ? b : a;
			});

	// 3 - copy sprite color into the whole line
	for (int spriteAddr : spritesFound)
	{
		unsigned char posY = (*mem)[spriteAddr];
		unsigned char posX = (*mem)[spriteAddr + 1];
		unsigned char flags = (*mem)[spriteAddr + 3];
		int paletteAddress = getPaletteFromOAMFlags(flags);
		bool bIsAboveBG = !(flags & (1 << 7));
		bool xFlipped = flags & (1 << 5);
		bool yFlipped = flags & (1 << 6);
		int tileAddress = getSpriteAddressInVRam(spriteAddr, spriteHeight);

		unsigned char yOffset = yLineToFetch - (posY - 16); // (posY - 16) is where the first line of the sprite should be drawn
		if (yFlipped) // reverse offset if flipped
			yOffset = spriteHeight - 1 - yOffset;
		// fetch the 8 pixel of the sprite in a tmp buffer
		std::array<int, 8> spritePixels = getTilePixels(tileAddress, yOffset, paletteAddress);
		if (xFlipped)
		{
			std::array<int, 8> tmpArray = spritePixels;
			for (int i=0, j=7; i < 8; i++, j--)
				spritePixels[i] = tmpArray[j];
		}

		// copy the sprite on the line
		for (int x=posX, i=0; (x < posX + 8) && (x < 160); x++, i++)
			spriteLine[x] = {spritePixels[i], bIsAboveBG}; // might need to check color 0 
														   // which is not winning over BG
														   // is it after or before palette ?
														   // (i think its after, then what about 
														   // CGB)
	}
	return spriteLine;
}

std::array<int, 8> Ppu::getBackgroundTile(unsigned char xOffsetInMap, unsigned char yOffsetInMap,
		unsigned char yOffsetInTile)
{
    unsigned int BGMap  = M_LCDC & (1 << 3) ? 0x9C00 : 0x9800;
    unsigned int BGDataAddress = M_LCDC & (1 << 4) ? 0x8000 : 0x8800;

	yOffsetInMap %= 32;
	xOffsetInMap %= 32;
    unsigned int addrInMap = BGMap + xOffsetInMap + yOffsetInMap;
    int tileNumber = (*mem)[addrInMap];
	// 2 * 8 because each tile is 2 * 8 and we need to skip the X previous tiles
	// (which have this size)
    return getTilePixels(BGDataAddress + (tileNumber * (2 * 8)), yOffsetInTile, BGP);
}

std::array<int, 8> Ppu::getWindowTile(unsigned int xOffsetInMap, unsigned int yOffsetInMap)
{
  unsigned int windowMap = M_LCDC & (1 << 6) ? 0x9C00 : 0x9800;
  unsigned int windowDataAddress = M_LCDC & (1 << 4) ? 0x8800 : 0x8000;

  // 32 is because each line is 32 byte, windowCurrentLine because it may or may not be updated
  // if it was rendered on previous lines NOTE unsure about this, need to be tested
  // div by 8 because each tile is 8 * 8 byte
  yOffsetInMap = 32 * (yOffsetInMap / 8);

  unsigned int addressInMap = windowMap + xOffsetInMap + yOffsetInMap;
  // condition is there to see if we need to loop over 1024 it should never happen if i understood correctly
  if (xOffsetInMap > 0x3ff || yOffsetInMap > 0x3ff)
	  std::cerr << "offset in window tile is superior to 1024 to fetch the tile data and is: " <<  xOffsetInMap + yOffsetInMap << std::endl;
  int tileNumber = (*mem)[addressInMap];
  unsigned int yOffset = 2 * (yOffsetInMap % 8);
  return getTilePixels(windowDataAddress + (tileNumber * (2 * 8)), yOffset, BGP);
}
