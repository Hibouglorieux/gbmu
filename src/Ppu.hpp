/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ppu.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 19:58:03 by nallani           #+#    #+#             */
/*   Updated: 2022/12/19 17:10:38 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PPU_CLASS_H
# define PPU_CLASS_H

#include "Mem.hpp"
#include <vector>
#include <array>
#include "TilePixels.hpp"
#include "Sprite.hpp"
#include "define.hpp"

struct SpriteData {
	int color;
	bool bIsAboveBackground;
	int colorCode;
};

struct BackgroundData {
	int color;
	int colorCode;
};

class Ppu {
public:
//	static void setMem(Mem& cpuMem);
    static std::array<int, PIXEL_PER_LINE> finalLine;


	static std::array<int, PIXEL_PER_LINE> doOneLine();
	static std::array<SpriteData, PIXEL_PER_LINE> getOamLine();
	static std::array<BackgroundData, PIXEL_PER_LINE> getBackgroundLine(); // TODO add virtual clocks
	static int getPaletteFromOAMEntry(struct OAM_entry entry);
	static int getSpriteAddressInVRam(struct OAM_entry entry, unsigned char spriteHeight);

	static struct TilePixels getTile(int tileAddress, int tileIndex, int paletteAddress);
	static TilePixels getWindowTile(unsigned int xOffsetInMap, unsigned int yOffsetInMap);
	static struct TilePixels getBackgroundTile(unsigned char xOffsetInMap, unsigned char yOffsetInMap);
//    static std::array<int, 8> fetch_tile_color(int tileAddr, int yOffset, int paletteAddr);
	static void resetWindowCounter();

private:
	static unsigned char windowCounter;
};

#endif
