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

#include "define.hpp"

struct OAM_entry {
	unsigned char posY;
	unsigned char posX;
	unsigned char tileIndex;
	unsigned char attributes;

	unsigned char getBGWOverWindow() {return BIT(attributes, 7);}
	unsigned char getFlipY() {return BIT(attributes, 6);}
	unsigned char getFlipX() {return BIT(attributes, 5);}
	unsigned char getPaletteNumber() {return BIT(attributes, 4);}
	unsigned char getTileVramBank() {return BIT(attributes, 3);}
	unsigned char getPaletteNumberCGB() {return (attributes & 0b111);}
};

struct SpriteData {
	int color;
	bool bShouldBeDisplayed;
};

class Ppu {
public:
    static void run(int cycle);
	static void setMem(Mem& cpuMem);

	static std::array<int, NB_LINES> doOneLine();
	static std::array<SpriteData, NB_LINES> getOamLine(int yLineToFetch);
	static std::array<int, NB_LINES> getBackgroundLine(int yLinetoFetch); // TODO add virtual clocks
	static int getColor(unsigned char byteColorCode, int paletteAddress);
	static int getPaletteFromOAMEntry(struct OAM_entry entry);
	static int getSpriteAddressInVRam(struct OAM_entry entry, unsigned char spriteHeight);

	static std::array<int, 8> getTilePixels(int tileAddress, unsigned char yOffset, int paletteAddress);
	static std::array<int, 8> getWindowTile(unsigned int xOffsetInMap, unsigned int yOffsetInMap);
	static std::array<int, 8> getBackgroundTile(unsigned char xOffsetInMap,
			unsigned char yOffsetInMap, unsigned char yOffsetInTile);
    static std::array<int, 8> fetch_tile_color(int tileAddr, int yOffset, int paletteAddr);

private:
};

#endif
