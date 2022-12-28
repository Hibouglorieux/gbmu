/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MBC.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/28 22:13:42 by nallani           #+#    #+#             */
/*   Updated: 2022/12/28 23:25:40 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MBC.hpp"
#include <iostream>

MBC* MBC::createMBC(unsigned char mbcCode)
{
	if (mbcCode == 0)
		return new RomOnly;
	if (mbcCode >= 1 && mbcCode <= 3)
		return new MBC1();
	if (mbcCode >= 5 && mbcCode <= 6)
	{
		std::cout << "MBC2 not yet coded !" << std::endl;	
		throw("");
		//return new MBC2();
	}
	if (mbcCode >= 0x0F && mbcCode <= 0x13)
	{
		std::cout << "MBC3 not yet coded !" << std::endl;	
		throw("");
		//return new MBC3();
	}
	if (mbcCode >= 0x19 && mbcCode <= 0x1E)
		return new MBC5();
	std::cout << "Unsupported MBC received: " << +mbcCode << std::endl;
	throw("");
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
		throw("");
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
	(void) addr;
	(void) value;
	return value;
}

unsigned short MBC2::getRomBank(unsigned short addr)
{
	if (addr < 0x4000)
		return 0;
	return 1;
}

unsigned char MBC2::getRamBank()
{
	return 0xFF;
}


unsigned char MBC3::writeInRom(unsigned short addr, unsigned char value)
{
	(void) addr;
	(void) value;
	return value;
}

unsigned short MBC3::getRomBank(unsigned short addr)
{
	if (addr < 0x4000)
		return 0;
	return 1;
}

unsigned char MBC3::getRamBank()
{
	return 0xFF;
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
