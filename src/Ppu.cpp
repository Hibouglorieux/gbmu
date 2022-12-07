/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ppu.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 19:58:01 by nallani           #+#    #+#             */
/*   Updated: 2022/12/08 00:12:08 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ppu.hpp"
#include <algorithm>

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
	for (int x = 0; x < 8; x++)
	{
		// get color based on the merge of the two bytes with the same bit
		// 0b10001000 0b00010010 will give 0x10, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01 and 0x00
		bool bit1 = byte1 & (1 << x);
		bool bit2 = byte2 & (1 << x);
		unsigned char byteColorCode = (bit1 << 1) | (bit2 << 1);
		tilePixels[x] = getColor(byteColorCode, paletteAddress);
	}
	return tilePixels;
}

std::array<int, NB_LINES> getBackgroundLine(int yLineToFetch)
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
			tilePixels = getWindowTile((xInLine + WX_OFFSET - mem[WX] as usize) / 8);// should not underflow/panic because of windowDraw bool
		}
		else if (bBackgroundEnabled)
		{
			tilePixels = getBackgroundTile(BGTileIT + mem[SCX] as usize / 8);
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
				continue;
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
	return color;
}

std::array<SpriteData, NB_LINES> Ppu::getOamLine(int yLineToFetch)
{
	std::vector<int> spritesFound;
	std::array<SpriteData, NB_LINES> spriteLine;
	if (!((*mem)[LCDC] & (1 << 1))) // if OBJ flag isnt enabled, return empty array
	{
		spriteLine.fill({0, false});
		return spriteLine;
	}
	const int OAM_Addr = 0xFE00;
	unsigned char spriteHeight = ((*mem)[LCDC] & 1 << 2) ? 16 : 8; // type of sprite from flag

	// 1 - fetch the sprites needed for that line
	for (int i = 0; i < 40; i++)
	{
		int address = OAM_Addr + (i * 4);	
		unsigned char posY = (*mem)[address];
		unsigned char posX = (*mem)[address + 1];

		unsigned char firstPixelDrawn = posY - 16;
		unsigned char lastPixelDrawn = posY - 16 + spriteHeight;

		// verify if the sprite should be rendered on this line
		// if posX == 0 then sprite is totally off the screen.
		// same goes for posY that starts at 0x10 (as it can be 16 height)
		if (firstPixelDrawn <= yLineToFetch &&
				lastPixelDrawn > yLineToFetch
				&& posX > 0)
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

std::array<int, 8> Ppu::getBackgroundTile(unsigned char tileNumber)
{
	// WIP TODO
	return {};
}

std::array<int, 8> Ppu::getWindowTile(unsigned char tileNumber)
{
	// WIP TODO
	return {};
}
/*

   pub struct PPU {
FIFO: Vec<u8>,
screenBuff: Vec<u8>,
windowCurrentLine: usize,
}

impl PPU {
pub fn default() -> Self {
Self {
FIFO: vec![0, 16],
screenBuff: vec![0, 160*144],
windowCurrentLine: 0,
}
}
fn get_data(mem: &Vec<u8>, loc: usize) -> u8 {
if mem[VBK] == 0
{
return mem[loc];
}
else
{
todo!();
}
}

fn get_vram_location(mem: &mut Vec<u8>) -> usize{
//TODO need to call get_data to access virtualized memory on CGB
if mem[LCDC] & (1 << 4) != 0
{
return 0x8000;
}
else
{
return 0x8800;
}
}

fn get_window_tile(&mut self, mem: &Vec<u8>, xOffsetInMap: usize) -> [u8; 8] {

let windowMap: usize  = if mem[LCDC] & (1 << 6) == 0 {0x9800} else {0x9C00};
let windowDataAddress: usize = if mem[LCDC] & (1 << 4) == 0 {0x8800} else {0x8000};

// 32 is because each line is 32 byte, windowCurrentLine because it may or may not be updated
// if it was rendered on previous lines NOTE unsure about this, need to be tested
// div by 8 because each tile is 8 * 8 byte
let yOffsetInMap: usize = 32 * (self.windowCurrentLine / 8);

let addressInMap = windowMap + xOffsetInMap + yOffsetInMap;
if xOffsetInMap > 0x3ff || yOffsetInMap > 0x3ff
{
panic!("offset in window tile is superior to 1024 to fetch the tile data and is {}",
xOffsetInMap + yOffsetInMap); // panic is there to see if we need to loop over 1024
							  // it should never happen if i understood correctly
							  }

							  let tileNumber: usize = mem[addressInMap] as usize;
							  let yOffset: usize = 2 * (self.windowCurrentLine % 8);

							  return PPU::fetch_tile_color(mem, windowDataAddress + (tileNumber * (2 * 8)), yOffset, BGP);
							  }

//TODO WIP
fn get_background_tile(mem: &Vec<u8>, mut xOffsetInMap: usize) -> [u8; 8] {

let BGMap: usize = if mem[LCDC] & (1 << 3) == 0 {0x9800} else {0x9C00};
let BGDataAddress: usize = if mem[LCDC] & (1 << 4) == 0 {0x8800} else {0x8000};

let mut yOffsetInMap: usize = 32 * ((mem[SCY] + mem[LY]) as usize / 8);

xOffsetInMap &= 1023;
yOffsetInMap &= 1023;

let addrInMap: usize = BGMap + xOffsetInMap + yOffsetInMap;

let tileNumber: usize = mem[addrInMap] as usize;
let yOffset: usize = 2 * (mem[SCY] as usize + mem[LY] as usize) % 8;

return PPU::fetch_tile_color(mem, BGDataAddress + (tileNumber * (2 * 8)), yOffset, BGP);
}

fn fetch_background_line(&mut self, mem: &Vec<u8>) -> [u8; 160]{
	let mut lineColor: [u8; 160] = [0; 160];
	let screenStartX = mem[SCX] as usize;
	let screenStartY = mem[SCY] as usize;
	let windowEnabled: bool = mem[LCDC] & (1 << 5) == 0;
	let backgroundEnabled: bool = mem[LCDC] & 1 == 0;
	let windowOffsetX: i16 = mem[WX] as i16;
	let windowOffsetY: u8 = mem[WY];
	let mut BGTileIT: usize = 0;
	let mut xInLine: usize = 0;
	let mut windowDraw: bool = windowEnabled && mem[LY] >= mem[WY] && xInLine as i16 >= mem[WX] as i16 - WX_OFFSET;
	loop
	{
		let mut tmpColoBuf: [u8; 8] = [0; 8];
		//fetch window or background tile
		if windowDraw
		{
			tmpColoBuf = self.get_window_tile(mem, (xInLine + WX_OFFSET - mem[WX] as usize) / 8);// should not underflow/panic because of windowDraw bool
		}
		else if backgroundEnabled
		{
			tmpColoBuf = PPU::get_background_tile(mem, BGTileIT + mem[SCX] as usize / 8);
		}
		{
			// copy tmpColoBuf in the line buffer
			for i in 0usize..8
			{
				if !windowDraw && i + xInLine < (mem[SCX] as usize % 8) // skip pixel if SCX % 8 != 0
																		// if scx == 3 then skip the
																		// first 3 pixels
				{
					continue;
				}
				lineColor[xInLine] = tmpColoBuf[i];
				xInLine += 1;
				if xInLine >= 159 // we have drawn our line
				{
					break;
				}
				// check if window should be enabled,
				// if the condition is met restart draw at that pos
				if !windowDraw && windowEnabled && mem[LY] >= windowOffsetY     // make sure the window has to be rendered with its WX/WY
					&& xInLine as i16 >= windowOffsetX - WX_OFFSET       // make sure window is in on x axis,
																		 // WX == 0x07 and WY == 0x00 means the window will be at the top left of the screen
				{
					// if this condition is met just redraw with window instead of bg now
					windowDraw = true;
					continue;
				}
			}
		}
		BGTileIT += 1;
	}
	//update windowCurrentLine if needed
	if windowDraw
	{
		self.windowCurrentLine += 1;// need to do that in order to fetch the good window line
	}
	return lineColor;
}
}
*/
