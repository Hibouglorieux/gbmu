/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hdma.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/06 01:22:57 by nathan            #+#    #+#             */
/*   Updated: 2023/01/07 20:48:55 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Hdma.hpp"
#include "Gameboy.hpp"

uint16_t Hdma::src = 0;
uint16_t Hdma::dst = 0;
uint16_t Hdma::len = 0;
int8_t Hdma::leftClocks = 0;
bool Hdma::bIsWritting = 0;
bool Hdma::bIsInHBlankMode = 0;
bool Hdma::bJustStarted = 0;


void Hdma::writeInHdma(uint16_t dstAddr, uint16_t srcAddr, uint8_t newValue)
{
	std::cout << "received write in Hdma with dstAddr: " << dstAddr
		<< " srcAddr: " << srcAddr << " Value: " << +newValue << std::endl;
	bool bStopping = !BIT(mem[0xFF55], 7);
	bool bIsHBlank = BIT(mem[0xFF55], 7);
	if (bIsWritting && !bStopping)
	{
		std::cerr << "HDMA WEIRD CASE HAPPENING !!!!" << std::endl;
		exit(0);
	}
	else if (bStopping && bIsWritting)
	{
		std::cout << "STOPPED hdma, len left was: " << len << " dstAdr: " << dst << std::endl;
		std::cout << std::endl;
		bIsWritting = false;
		mem.supervisorWrite(0xFF55, newValue);
		//exit(0);
	}
	else
	{
		bJustStarted = true;
		src = srcAddr;
		dst = dstAddr;
		len = ((newValue & 0x7F) + 1) * 0x10;
		bIsWritting = true;
		leftClocks = bIsInHBlankMode ? 0 : 0; // counts for delay
		bIsInHBlankMode = bIsHBlank;
		std::cout << (bIsInHBlankMode ? "starting hdma in HBLANK" : "started hdma normal") << std::endl;
		std::cout << "hdma val is: " << +newValue << std::endl;
		std::cout << "hdma len is: " << len << std::endl;
		std::cout << std::endl;
	}
}

#define MAGIC_DEBUG_VALUE (1)

void Hdma::update(int8_t clockToAdd, uint8_t speed, bool bGameboyInHBlank)
{
	if (bJustStarted)
	{
		// as this update is called when the bit is set,
		// and the clock are added after, we should ignore the first call
		bJustStarted = false;
		return;
	}
	if (!bIsWritting || (bIsInHBlankMode && !bGameboyInHBlank))
		return;
	//clockToAdd *= speed; // NOTE shall we ignore this ?
	(void)speed;
	clockToAdd += leftClocks;
	int countDebug = 0;
	if (clockToAdd >= MAGIC_DEBUG_VALUE)
		std::cout << "update hdma, len left is: " << len << std::endl;
	else
	{
		leftClocks = clockToAdd;
		return;
	}
	// TODO this isnt WORKING AT ALL
	// letting just len > 0 will make it like a memcpy instantly
	while (clockToAdd >= MAGIC_DEBUG_VALUE && len > 0)
	{
		//std::cout << "srcAddr: " << +src << std::endl;
		//std::cout << "dstAddr: " << +dst << std::endl;
		clockToAdd -= MAGIC_DEBUG_VALUE;

		//NOTE XXX 0xC is the minimum for pokemon yellow to work
		//but since i cant understand how it works i just let it at 0xC
		for (int i = 0; i < 0xC && len > 0; i++)
		{
			countDebug++;
			mem[dst] = +(mem[src]);
			dst++;
			src++;
			len--;
		}

	}
	std::cout << "wrote in hdma: " << countDebug << " times" << std::endl;
	mem.supervisorWrite(0xFF55, (uint8_t)((uint8_t)(len / 16) - 1 + (len % 16 == 0 ? 0 : 1)));
	std::cout << "exit hdma update, len left: " << len << std::endl;
	std::cout << "exit hdma update, FF55 is equal to: " << (int)mem[0xFF55] << std::endl;
	leftClocks = clockToAdd;
	if (len == 0)
		bIsWritting = false;
	std::cout << std::endl;
}

void Hdma::updateHBlank()
{
	bJustStarted = false;
	if (!bIsWritting || !bIsInHBlankMode)
		return;
	for (int i = 0; i < 0x10; i++)
	{
		//std::cout << "srcAddr: " << +src << std::endl;
		//std::cout << "dstAddr: " << +dst << std::endl;
		mem[dst] = +(mem[src]);
		dst++;
		src++;
		len--;
	}
	mem.supervisorWrite(0xFF55, (uint8_t)((uint8_t)(len / 16) - 1 + (len % 16 == 0 ? 0 : 1)));
	std::cout << "exit hdma update, len left: " << len << std::endl;
	std::cout << "exit hdma update, FF55 is equal to: " << (int)mem[0xFF55] << std::endl;
	if (len == 0)
		bIsWritting = false;
	std::cout << std::endl;
}
