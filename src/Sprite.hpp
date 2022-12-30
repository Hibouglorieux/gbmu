/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sprite.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/17 18:56:01 by nathan            #+#    #+#             */
/*   Updated: 2022/12/29 22:10:37 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SPRITE_CLASS_H
# define SPRITE_CLASS_H
#include <vector>
#include "Gameboy.hpp"
#include "define.hpp"

struct OAM_entry {
	unsigned char posY;
	unsigned char posX;
	unsigned char tileIndex;
	unsigned char attributes;

	bool getBGWOverWindow() const {return BIT(attributes, 7);}
	bool getFlipY() const {return BIT(attributes, 6);}
	bool getFlipX() const {return BIT(attributes, 5);}
	bool getDMGPalette() const {return BIT(attributes, 4);}
	bool getTileVramBank() const {return BIT(attributes, 3);}
	unsigned char getCGBPalette() const {return (attributes & 0b111);}

	friend bool operator==(const OAM_entry &a, const OAM_entry &b) {
		return ((a.posX == b.posX) && (a.posY == b.posY) && (a.tileIndex == b.tileIndex) && (a.attributes == b.attributes));
	}
};

class Sprite {

public:
    Sprite(OAM_entry newOAM, unsigned char newSpriteHeight);
	~Sprite(void);
    //Sprite(std::vector<std::array<int, 8>> data);

	unsigned char getSpriteHeight() const {return spriteHeight;}
	unsigned short getPaletteAddr() const;
	std::array<int, 8> getLineColorCode(int y) const { return data[y]; }
	std::array<int, 8> getColoredLine(int y) const;
	void flipY();
	void flipX();
private:
	unsigned char	spriteHeight;
	std::vector<std::array<int, 8>>	data;


	//static GamaeboyMode gbMode// need to know if CGB or DMG

	OAM_entry OAM_Data;
};

#endif
