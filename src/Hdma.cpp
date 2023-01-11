/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hdma.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/06 01:22:57 by nathan            #+#    #+#             */
/*   Updated: 2023/01/11 03:27:48 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Hdma.hpp"
#include "Gameboy.hpp"

uint16_t Hdma::src = 0;
uint16_t Hdma::dst = 0;
uint16_t Hdma::len = 0;
bool Hdma::bIsWritting = false;
bool Hdma::bIsInHBlankMode = false;


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
	}
	else
	{
		src = srcAddr;
		dst = dstAddr;
		len = ((newValue & 0x7F) + 1) * 0x10;
		bIsWritting = true;
		bIsInHBlankMode = bIsHBlank;
		std::cout << (bIsInHBlankMode ? "starting hdma in HBLANK" : "started hdma normal") << std::endl;
		std::cout << "hdma val is: " << +newValue << std::endl;
		std::cout << "hdma len is: " << len << std::endl;
		std::cout << std::endl;
	}
}

#define MAGIC_DEBUG_VALUE (1)

int Hdma::update()
{
	if (!bIsWritting || bIsInHBlankMode)
		return 0;
	for (int i = 0; i < 2; i++)
	{
		mem[dst] = +(mem[src]);
		dst++;
		src++;
		len--;
	}
	mem.supervisorWrite(0xFF55, (uint8_t)((uint8_t)(len / 16) - 1 + (len % 16 == 0 ? 0 : 1)));
	if (len == 0)
		bIsWritting = false;
	return 1;
}

int Hdma::updateHBlank()
{
	if (!bIsWritting || !bIsInHBlankMode)
		return 0;
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
	if (len == 0)
		bIsWritting = false;
	return 8;
}
