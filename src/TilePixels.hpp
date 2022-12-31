/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TilePixels.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/30 22:27:01 by nallani           #+#    #+#             */
/*   Updated: 2022/12/31 04:01:56 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TILEPIXELS_HPP
#define TILEPIXELS_HPP

#include <array>
#include <algorithm>
#include "Gameboy.hpp"

struct TilePixels {
    short data[8][8]{};

    void flipX() {
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 4; x++) {
                int tmp = data[y][x];
                data[y][x] = data[y][7 - x];
                data[y][7 - x] = tmp;
            }
        }
    }
    void flipY() {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 8; x++) {
                int tmp = data[y][x];

                data[y][x] = data[7 - y][x];
                data[7 - y][x] = tmp;
            }
        }
    }

	static unsigned long getCGBPaletteColor(unsigned char paletteNb, const std::array<unsigned char, 64>& paletteArray);
	// returns the color in a short
	// if its CGB mode then 
	// the color is encoded in two bytes
    static short getColor(unsigned char byteColorCode, unsigned long paletteColor);
	std::array<short, 8> getColorLine(int y);
	std::array<short, 8> getLineColorCode(int y);

	bool isAboveOAM() const;

	TilePixels();
    //TilePixels(std::array<std::array<int, 8>, 8> val);
    TilePixels(unsigned short tileAddress, unsigned short mapAddress);
	unsigned short mapAddr;
	bool bIsValid;
};

#endif
