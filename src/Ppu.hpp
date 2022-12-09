/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ppu.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 19:58:03 by nallani           #+#    #+#             */
/*   Updated: 2022/12/09 03:05:53 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mem.hpp"
#include <vector>
#include <array>

#ifndef PPU_CLASS_H
# define PPU_CLASS_H

//VRAM:
//starts at 8000 to 97FF
//0x8000 is always sprites, background can be either at 0x8000 or 0x8800

// FF40 is LCDC (LCD Control Regiser)
// 7: LCD display enable
// 6: Window Tile Map Address : 0 is 0x9800-0x9BFF 1 is 0x9C00-0x9FFF // NOTE verify
// 5: Window Enable
// 4: BG & Window Tile data 0 is 0x8800-0x97FF 1 is 0x8000-0x8FFF
// (noted   BG Characters in offial doc)
// 3: BG Tile Map Address 0 is 0x9800-0x9BFF 1 is 0x9C00-0x9FFF // NOTE verify
// 2 : OBJ size (0 is 8x8 | 1 is 8x16) (obj == sprite)
// 1: OBJ enable (0 off, 1 on)
// 0: Background Enable (0 off, 1 on) (always true in CGB)
#define LCDC (0xFF40)


// OAM Attributes (FE00 to FEA0 (40 x 4 bytes)):
//	byte 0 is posY
//	byte 1 is posX
//	byte 2 is tileIndex
//	byte 3 are flags:
//
// 	Bit7   BG and Window over OBJ (0=No, 1=BG and Window colors 1-3 over the OBJ)
//  Bit6   Y flip          (0=Normal, 1=Vertically mirrored)
//  Bit5   X flip          (0=Normal, 1=Horizontally mirrored)
//  Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
//  Bit3   Tile VRAM-Bank  **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
//  Bit2-0 Palette number  **CGB Mode Only**     (OBP0-7)


// FF41 is LCDC status
// 6: LYC = LY interrupt
// 5: Mode 2 OAM Interrupt
// 4: Mode 1 V-Blank Interrupt
// 3: Mode 0 H-Blank Interrupt
// 2: LYC = LY Flag
// 1: Mode 00 => ram access, 10 OBJ search, 01 V-Blank, 11 LCD transfer
#define LCDC_STATUS (0xFF41)
#define SCY (0xFF42) // scroll Y // LCD viewport offset
#define SCX (0xFF43) // scroll X // LCD viewport offset
#define LY (0xFF44) // LCDC Y coordinate // (which line is currently rendered)
#define LYC (0xFF45) //  LY compare register (compare to this and can set FF41 byte 2 to launch interrupt if needed)
#define DMA (0xFF46) // DMA transfer and start
#define BGP (0xFF47) // background palette 0b11000000 => 11, 0b110000 => 10, 0b1100 => 01, 0b11 => 00
#define OBP0 (0xFF48) // Object/Sprite palette 0
#define OBP1 (0xFF49) // Object/Sprite palette 1
#define WY (0xFF4A) // Window Y pos, 0 is the top // (where should the window be placed virtually on the background)
#define WX (0xFF4B) // Window X pos, 7 should be the start https://hacktix.github.io/GBEDG/ppu/ // XXX to check
#define WX_OFFSET (7)
#define LCD_Y (0xFB00) //Top edge when == 10
#define VBK (0xFF4F) // CGB only, VRAM bank specification, 0 means bank0, 1 bank1

#define NB_LINES 160

struct SpriteData {
	int color;
	bool bShouldBeDisplayed;
};

class Ppu {
public:
	static void setMem(Mem& cpuMem);

	static std::array<int, NB_LINES> doOneLine();
	static std::array<SpriteData, NB_LINES> getOamLine(int yLineToFetch);
	static std::array<int, NB_LINES> getBackgroundLine(int yLinetoFetch); // TODO add virtual clocks
	static int getColor(unsigned char byteColorCode, int paletteAddress);
	static int getPaletteFromOAMFlags(unsigned char flags);
	static int getSpriteAddressInVRam(int spriteAddrInOAM, unsigned char spriteHeight);

	static std::array<int, 8> getTilePixels(int tileAddress, unsigned char yOffset, int paletteAddress);
	static std::array<int, 8> getWindowTile(unsigned int xOffsetInMap, unsigned int yOffsetInMap);
	static std::array<int, 8> getBackgroundTile(unsigned char xOffsetInMap,
			unsigned char yOffsetInMap, unsigned char yOffsetInTile);
    static std::array<int, 8> fetch_tile_color(int tileAddr, int yOffset, int paletteAddr);

private:
};

#define M_LCDC (mem[LCDC])
#define M_LCDC_STATUS (mem[LCDC_STATUS])
#define M_SCY (mem[SCY]) 
#define M_SCX (mem[SCX]) 
#define M_LY (mem[LY]) 
#define M_LYC (mem[LYC]) 
#define M_DMA (mem[DMA]) 
#define M_OBP0 (mem[OBP0]) 
#define M_OBP1 (mem[OBP1]) 
#define M_WY (mem[WY]) 
#define M_WX (mem[WX]) 
#define M_LCD_Y (mem[LCD_Y]) 
#define M_VBK (mem[VBK]) 

#endif
