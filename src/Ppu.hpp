/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ppu.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 19:58:03 by nallani           #+#    #+#             */
/*   Updated: 2023/01/03 01:43:08 by nallani          ###   ########.fr       */
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
	bool bIsSet;
};

struct BackgroundData {
	int color;
	int colorCode;
	bool bIsAboveOAM;
};

class Ppu {
public:
//	static void setMem(Mem& cpuMem);
    static std::array<short, PIXEL_PER_LINE> renderedLine;


	static std::array<short, PIXEL_PER_LINE> doOneLine();
	static std::array<SpriteData, PIXEL_PER_LINE> getOamLine();
	static std::array<BackgroundData, PIXEL_PER_LINE> getBackgroundLine(); // TODO add virtual clocks

	static struct TilePixels getTile(int tileAddress, int tileIndex, unsigned short mapAddress);
	static TilePixels getWindowTile(unsigned int xOffsetInMap, unsigned int yOffsetInMap);
	static struct TilePixels getBackgroundTile(unsigned char xOffsetInMap, unsigned char yOffsetInMap);
//    static std::array<int, 8> fetch_tile_color(int tileAddr, int yOffset, int paletteAddr);
	static void resetWindowCounter();

private:
	static unsigned char windowCounter;
};

#endif
