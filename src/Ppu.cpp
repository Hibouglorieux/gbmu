/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ppu.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 19:58:01 by nallani           #+#    #+#             */
/*   Updated: 2022/12/09 03:05:38 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ppu.hpp"
#include "Gameboy.hpp"
#include <algorithm>
#include <iostream>

std::array<int, NB_LINES> Ppu::doOneLine()
{
	auto pixelLine = getOamLine();

	auto backgroundLine = getBackgroundLine();


	std::array<int, NB_LINES> finalLine = {0};
	for (int i = 0; i < NB_LINES; i++)
	{
		if (pixelLine[i].bShouldBeDisplayed && pixelLine[i].color) {
			finalLine[i] = pixelLine[i].color;
		}
		else if (BIT(M_LCDC, 0)) {
			finalLine[i] = backgroundLine[i];
		}
	}
	return finalLine;
}

int	Ppu::getPaletteFromOAMEntry(struct OAM_entry entry)
{
	//this is for DMG only !!
	return (entry.getPaletteNumber() ? OBP1 : OBP0);
	// TODO add gameboy flags (bit 	0-2 OPB0 to OBP7)
}

int Ppu::getSpriteAddressInVRam(struct OAM_entry entry, unsigned char spriteHeight)
{
   	// DMG ONLY !
	// TODO add gameboy additionnal bank !
	// which might not be 0x8000 (to see with Mem class)

	// bool bank1 = (*mem)[spriteAddr + 4] & (1 << 3);
	// 0x8000 is start of OBJ Vram, multiplied by the number of the tile * size of sprite
	// sprite is spriteHeight * 2 byte long, added
	return 0x8000 + entry.tileIndex * spriteHeight * 2;
}

TilePixels Ppu::getTile(int tileAddress, int tileIndex, int paletteAddress)
{
	// fetch the 64 pixels of a tile 

	std::array<std::array<int, 8>, 8> pixels;
	for (int y = 0; y < 8; y++) {

		unsigned char byte1 = mem[tileAddress + (tileIndex * 2 * 8) + (y * 2)];
		unsigned char byte2 = mem[tileAddress + (tileIndex * 2 * 8) + (y * 2) + 1];
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
	return TilePixels(pixels);
}

std::array<int, NB_LINES> Ppu::getBackgroundLine()
{
	std::array<int, NB_LINES> backgroundLine;
	bool bWindowEnabled = BIT(M_LCDC, 5);
	bool bBackgroundEnabled = BIT(M_LCDC, 0);
	int BGTileIt = 0;
	int xPosInLine = 0;
	bool bDrawWindow = bWindowEnabled && M_LY >= M_WY && xPosInLine >= (M_WX - WX_OFFSET);
	while (xPosInLine < 160)
	{
		std::array<int, 8> tilePixels;
		if (bDrawWindow)
		{
			tilePixels = getWindowTile((xPosInLine + WX_OFFSET - M_WX) / 8, M_LY - M_WY);// should not underflow/panic because of windowDraw bool
		}
		else if (bBackgroundEnabled)
		{
			tilePixels = getBackgroundTile(BGTileIt + M_SCX / 8, (M_LY + M_SCY) / 8)[(M_LY + M_SCY) % 8];
			BGTileIt++;
		}
		for (int i = 0; i < 8; i++)
		{
			if (!bDrawWindow && i + xPosInLine < (M_SCX % 8))// skip pixel if SCX % 8 != 0
				continue;				 // if scx == 3 then skip the
									 // first 3 pixels
			backgroundLine[xPosInLine++] = tilePixels[i];
			if (xPosInLine >= 160)
				return backgroundLine;
			// check if window should be enabled,
			// if the condition is met restart draw at that pos
			// make sure the window has to be rendered with its WX/WY
			// make sure window is in on x axis,
			// WX == 0x07 and WY == 0x00 means the window will be at the top left of the screen
			if (bWindowEnabled && M_LY >= M_WY && xPosInLine >= (M_WX - WX_OFFSET))
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
	int color = mem[paletteAddress] & (0b11 << bitPosInPalette);
	color >>= bitPosInPalette;
	return color;
}

std::array<SpriteData, NB_LINES> Ppu::getOamLine()
{
	std::vector<struct OAM_entry> spritesFound, spritesFound2;
	std::array<SpriteData, NB_LINES> spriteLine;
	spriteLine.fill({0, false}); // Init first the sprite line
	if (!BIT(M_LCDC, 1)) { // if OBJ flag isnt enabled, return empty array
		return spriteLine;
	}
	const int OAM_Addr = 0xFE00;
	unsigned char spriteHeight = BIT(M_LCDC, 2) ? 16 : 8; // type of sprite from flag

	struct OAM_entry *entry = (struct OAM_entry *)(&mem[OAM_Addr]);
	// 1 - fetch the sprites needed for that line
	for (int i = 0; i < MAX_SPRITES; i++)
	{
		// verify if the sprite should be rendered on this line
		// if posX == 0 then sprite is totally off the screen.
		// same goes for posY that starts at 0x10 (as it can be 16 height)
		if ((M_LY + 16 >= entry->posY && (M_LY + 16 < entry->posY + 8)) // TODO : change 8 to height ?
				&& entry->posX > 0)
		{
			spritesFound.push_back(*entry);
			if (spritesFound.size() >= 10) // exit if we already found 10 sprites to render
				break;
		}
		entry++;
	}


	spritesFound2 = spritesFound;
	// 2 -reverse sort sprites so that the first (in X drawn order) will be drawn fully
	// CHANGE : Priorities : we will draw first the greatest X so the lowest X overlap them
	std::sort(spritesFound.begin(), spritesFound.end(), [=](struct OAM_entry &a, struct OAM_entry &b){
		if (a.posX != b.posX)
			return a.posX > b.posX;
		else {
			// if same X, we pick the sprites earliest in OAM
			auto ndxA = std::find(spritesFound2.begin(), spritesFound2.end(), a) - spritesFound2.begin();
			auto ndxB = std::find(spritesFound2.begin(), spritesFound2.end(), b) - spritesFound2.begin();
			return ndxA > ndxB;
		}
	});

	// 3 - copy sprite color into the whole line
	for (struct OAM_entry spriteEntry : spritesFound)
	{
		int paletteAddress = getPaletteFromOAMEntry(spriteEntry);
		bool bIsAboveBG = !spriteEntry.getBGWOverWindow();
		int tileAddress = getSpriteAddressInVRam(spriteEntry, spriteHeight);
		TilePixels spritePixels = getTile(tileAddress, 0, paletteAddress);

		// TODO : le 16 doit etre remplace par spriteHeight * 2 ???
		unsigned char yOffset = M_LY - (spriteEntry.posY - 16); // (posY - 16) is where the first line of the sprite should be drawn
		if (spriteEntry.getFlipY()) // reverse offset if flipped
			spritePixels.flipY();
		// fetch the 8 pixel of the sprite in a tmp buffer
		if (spriteEntry.getFlipX())
			spritePixels.flipX();

		// copy the sprite on the line
		for (int x=spriteEntry.posX - 8, i=0; (x < spriteEntry.posX) && (x < 160); x++, i++)
			if (x > 0)
				spriteLine[x] = {spritePixels[yOffset][i], bIsAboveBG}; // might need to check color 0 
														   // which is not winning over BG
														   // is it after or before palette ?
														   // (i think its after, then what about 
														   // CGB)
	}
	return spriteLine;
}

TilePixels Ppu::getBackgroundTile(unsigned char xOffsetInMap, unsigned char yOffsetInMap)
{
    unsigned int BGMap  = BIT(M_LCDC, 3) ? 0x9C00 : 0x9800;
    unsigned int BGDataAddress = BIT(M_LCDC, 4) ? 0x8000 : 0x8800;

	// yOffsetInMap %= 32;
	// xOffsetInMap %= 32;

    unsigned int addrInMap = BGMap + xOffsetInMap + (yOffsetInMap * 32);
    int tileNumber = mem[addrInMap];
	// 2 * 8 because each tile is 2 * 8 and we need to skip the X previous tiles
	// (which have this size)
    return getTile(BGDataAddress, tileNumber, OBP0);
}

std::array<int, 8> Ppu::getWindowTile(unsigned int xOffsetInMap, unsigned int yOffsetInMap)
{
  unsigned int windowMap = BIT(M_LCDC, 6) ? 0x9C00 : 0x9800;
  unsigned int windowDataAddress = BIT(M_LCDC, 4) ? 0x8800 : 0x8000;

  // 32 is because each line is 32 byte, windowCurrentLine because it may or may not be updated
  // if it was rendered on previous lines NOTE unsure about this, need to be tested
  // div by 8 because each tile is 8 * 8 byte
  yOffsetInMap = 32 * (yOffsetInMap / 8);

  unsigned int addressInMap = windowMap + xOffsetInMap + yOffsetInMap;
  // condition is there to see if we need to loop over 1024 it should never happen if i understood correctly
  if (xOffsetInMap > 0x3ff || yOffsetInMap > 0x3ff)
	  std::cerr << "offset in window tile is superior to 1024 to fetch the tile data and is: " <<  xOffsetInMap + yOffsetInMap << std::endl;
  int tileNumber = mem[addressInMap];
  unsigned int yOffset = 2 * (yOffsetInMap % 8);
  return getTile(windowDataAddress, tileNumber, BGP)[yOffset];
}
