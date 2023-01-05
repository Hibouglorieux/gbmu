/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MBC.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/28 22:13:42 by nallani           #+#    #+#             */
/*   Updated: 2022/12/29 18:31:20 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MBC.hpp"
#include "define.hpp"
#include <iostream>

MBC* MBC::createMBC(unsigned char mbcCode)
{
	if (mbcCode == 0)
		return new RomOnly;
	if (mbcCode >= 1 && mbcCode <= 3)
		return new MBC1();
	if (mbcCode >= 5 && mbcCode <= 6)
	{
		std::cout << "Warning: MBC2 isn't fully supported" << std::endl;
		return new MBC2();// XXX isnt really supported but works anyway
	}
	if (mbcCode >= 0x0F && mbcCode <= 0x13)
	{
		std::cout << "WARNING: MBC3 is WIP !" << std::endl;
		return new MBC3();
	}
	if (mbcCode >= 0x19 && mbcCode <= 0x1E)
		return new MBC5();
	std::cout << "Unsupported MBC received: " << +mbcCode << std::endl;
	throw("3");
}

unsigned char RomOnly::writeInRom(unsigned short addr, unsigned char value)
{
	(void) addr;
	(void) value;
	return value;
}

unsigned short RomOnly::getRomBank(unsigned short addr)
{
	if (addr < 0x4000)
		return 0;
	return 1;
}

unsigned char RomOnly::getRamBank()
{
	return 0xFF;
}

unsigned char MBC1::writeInRom(unsigned short addr, unsigned char value)
{
	if (addr >= 0x6000 && addr <= 0x7FFF)
	{
		bAdvancedBankingMode = value;
	}
	if (addr >= 0x4000 && addr <= 0x5FFF)
	{
		highBitsRomBankNumberOrRam = value & 0b11;
	}
	if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		lowBitsRomBankNumber = value & 0x1F;
	}
	if (addr <= 0x1FFF)
	{
		bEnableRam = ((value & 0xF) == 0xA);
	}
	return value;
}

unsigned short MBC1::getRomBank(unsigned short addr)
{
	unsigned char selectedRomBank = 0;
	if (addr <= 0x3FFF)
    {
		if (bAdvancedBankingMode)
			selectedRomBank |= (highBitsRomBankNumberOrRam << 5);
		return selectedRomBank;
    }
	else if (addr >= 0x4000 && addr <= 0x7FFF)
	{
        selectedRomBank = lowBitsRomBankNumber | (highBitsRomBankNumberOrRam << 5);
        if (lowBitsRomBankNumber == 0)
            selectedRomBank += 1;
		return selectedRomBank;
	}
	else
	{
		std::cout << "error trying to call getRomBank with invalid address: " << addr << std::endl;
		throw("4");
	}
}

unsigned char MBC1::getRamBank()
{
	if (!bEnableRam)
		return 0xFF;// 0xFF stands for no extra ram used
	return bAdvancedBankingMode ? highBitsRomBankNumberOrRam : 0;
}


unsigned char MBC2::writeInRom(unsigned short addr, unsigned char value)
{
	if (addr <=	0x3FFF)
	{
		if (!BIT(addr, 8)) {
			// Controls RAM enable
			bEnableRam = (value == 0xA);

		} else {
			// Controls ROM Bank
			romBankNb = (value & 0b1111);
		}
	}
	if (addr >= 0x2100 && addr <= 0x2FFF)
	{
		romBankNb = value & 0x0F;
	}
	return value;
}

unsigned short MBC2::getRomBank(unsigned short addr)
{
	if (addr < 0x4000)
		return 0;
	return romBankNb == 0 ? 1 : romBankNb;
}

unsigned char MBC2::getRamBank()
{
	return 0xFF;// wont work with saves but works otherwise
}

rtc MBC3::getCurrentTime() {
	// TODO
	rtc current_time;

	memset(&time, 0, sizeof(rtc));

  	return current_time;
}


unsigned char MBC3::writeInRom(unsigned short addr, unsigned char value)
{
	//WIP
	if (addr < 0x1FFF)
		bEnableRam = value == 0xA;
	// if (addr >= 0xA000 && addr <= 0xBFFF)
	// {
	// 	if (bEnableRam)
	// 		;
	// }
	if (addr >= 0x2000 && addr <= 0x3FFF) {
		// 2000-3FFF - ROM Bank Number
		romBankNb = (value & 0b01111111);
	}
	if (addr >= 0x4000 && addr <= 0x5FFF) {
		// 4000-5FFF - RAM Bank Number - or - RTC Register Select
		if (value <= 0x07) {
			// RAM Bank Number
			ramBankNb = value;

		} else if (value <= 0x0C) {
			// RTC Register Select
			switch (value)
			{
			case 0x08:
				// Bind Seconds
				rtcBindNb = 0;
				break;
			case 0x09:
				// Bind Minutes
				rtcBindNb = 1;
				break;
			case 0x0A:
				// Bind Hours
				rtcBindNb = 2;
				break;
			case 0x0B:
				// Bind Lower 8 bits of day counter
				rtcBindNb = 3;
				break;
			case 0x0C:
				// Bind Upper 1 bit of day counter
				rtcBindNb = 4;
				break;
			default:
				throw "Wrong value written in RTC Select (MBC3)";
			}
			ramBankNb = 0xFF;
		}
	}
	else if (addr >= 0x6000 && addr <= 0x7FFF) {
		if (lastVal == 0 && value == 1) {
			if (!latched)
				time = getCurrentTime();
			latched = !latched;
		}
	}

	lastVal = value;
	return value;
}

unsigned short MBC3::getRomBank(unsigned short addr)
{
	if (addr < 0x4000)
		return 0;
	if (addr >= 0x4000 && addr <= 0x7FFF)
		return romBankNb == 0 ? 1 : romBankNb;
	return 1;
}

unsigned char MBC3::getRamBank()
{
	return ramBankNb;
}


unsigned char MBC5::writeInRom(unsigned short addr, unsigned char value)
{
	if (addr >= 0x4000 && addr <= 0x5FFF)
	{
		ramBankNb = value;
	}
	if (addr >= 0x3000 && addr <= 0x3FFF)
	{
		bit9 = value;
	}
	if (addr >= 0x2000 && addr <= 0x2FFF)
	{
		leastSignificantRomByte = value;
	}
	if (addr <= 0x1FFF)
	{
		bEnableRam = ((value & 0xF) == 0xA);
	}
	return value;
}

unsigned short MBC5::getRomBank(unsigned short addr)
{
	if (addr < 0x4000)
		return 0;
	return leastSignificantRomByte | (bit9 << 8);
}

unsigned char MBC5::getRamBank()
{
	if (!bEnableRam)
		return 0xFF;
	return ramBankNb;
}
