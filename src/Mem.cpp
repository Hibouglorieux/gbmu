/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mem.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:49:00 by nallani           #+#    #+#             */
/*   Updated: 2022/12/26 21:44:08 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mem.hpp"
#include "Cpu.hpp"
#include <fstream>
#include <iostream>
#include "Joypad.hpp"

#define MEM_SIZE (0xFFFF + 1)
#define RAM_BANK_SIZE (8 * 1024)
#define ROM_BANK_SIZE (16 * 1024)

const std::map<unsigned short, unsigned char> Mem::readOnlyBits = {
 {0xFF00, 0b1100'1111}, // 0xCF, 0xFF00 is input register, first 4 bit are
						// set to 0 when pressed, last 2 are unused
 {0xFF02, 0b0111'1100}, // serial control
 {0xFF07, 0b1111'1000}, // TAC. unused last 5 bits
 {0xFF0F, 0b1110'0000}, // IF, unusued last 7 bits
 {0xFF41, 0b1000'0111}, // LCDC Stat, unused bit7, first 3 bit are readOnly (set by ppu)
 {0xFF4D, 0b0111'1110}, // Key1
 {0xFF4F, 0b1111'1110}, // VBK, Vbank spec, only 1rst bit used
 {0xFF52, 0b0000'1111}, // HDMA2
 {0xFF53, 0b1110'0000}, // HDMA3
 {0xFF54, 0b0000'1111}, // HDMA4
 {0xFF68, 0b0100'0000}, // BCPS
 {0xFF6A, 0b0100'0000}, // OCPS
 {0xFF70, 0b1111'1000}, // SVBK, only 3 first bits used
				
};

Mem::Mem()
{
	romBanks = {};
	extraRamBanks = {};
	internalArray = nullptr;
	isValid = false;
	memSize = 0;
}

void Mem::supervisorWrite(unsigned int addr, unsigned char value)
{
	Mem::internalArray[addr] = value;
}

Mem::Mem(int size)
{
	isValid = true;
	internalArray = new unsigned char[size];
	memSize = size;
	init();
}

Mem::Mem(const std::string& pathToRom)
{
	std::ifstream file = std::ifstream(pathToRom, std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "file not found" << std::endl;
		isValid = false;
		memSize = 0;
		return;
	}

	// read some values in the 
	file.seekg(0, std::ifstream::end);
	int fileLen = file.tellg();
	if (fileLen < 32'768)
	{
			std::cerr << "Error with rom at wrong format" << std::endl;
			throw("");
	}
	file.seekg(0x148, std::ifstream::beg);
	char romSizeCode;
   	file.read(&romSizeCode, 1);
	int romBanksNb = getRomBanksNb(romSizeCode);
	if (fileLen != romBanksNb * 1024 * 16)
	{
		std::cerr << "Wrong size read in header: " << romBanksNb * 1024 * 16;
		throw("");
	}
	char ramSizeCode;
   	file.read(&ramSizeCode, 1);
	file.seekg(0, std::ifstream::beg);
	int extraRamBanksNb = getExtraRamBanksNb(ramSizeCode);

	for (int i = 0; i < romBanksNb; i++)
		romBanks.push_back(new unsigned char[ROM_BANK_SIZE]);
	std::cout << "created " << romBanksNb << " rom banks" << std::endl;

	for (int i = 0; i < extraRamBanksNb; i++)
		extraRamBanks.push_back(new unsigned char[RAM_BANK_SIZE]);
	std::cout << "created " << extraRamBanksNb << " extra ram banks" << std::endl;

	internalArray = new unsigned char[MEM_SIZE];
//    bzero(internalArray, MEM_SIZE);

    for (int i = 0; i < romBanksNb; i++) {
        file.read((char *) romBanks[i], ROM_BANK_SIZE);
    }

//    for (int i = 0; i < extraRamBanksNb; i++) {
//        file.read((char *) extraRamBanks[i], ROM_BANK_SIZE);
//    }
    file.close();
	isValid = true;
	memSize = MEM_SIZE;
	std::cout << "loaded rom with title: " << getTitle() << std::endl;
	std::cout << "CartRidge type: " << (int)getCartridgeType() << std::endl;

	init();
}

const Mem&	Mem::operator=(const Mem& rhs)
{
	if (isValid)
	{
		delete[] internalArray;
		for (unsigned char* ramBank : extraRamBanks)
			delete[] ramBank;
		extraRamBanks.clear();
		for (unsigned char* romBank : romBanks)
			delete[] romBank;
		romBanks.clear();
	}

	if (!rhs.isValid)
	{
		isValid = false;
		memSize = 0;
		return *this;
	}
	this->internalArray = new unsigned char[MEM_SIZE];
    bzero(internalArray, MEM_SIZE);

    for (int i = 0; i < MEM_SIZE; i++)
		this->internalArray[i] = rhs.internalArray[i];

	for (auto extraRamBank : rhs.extraRamBanks)
	{
		this->extraRamBanks.push_back(new unsigned char[RAM_BANK_SIZE]);
        auto ram = this->extraRamBanks.back();
//        bzero(ram, RAM_BANK_SIZE);
		memcpy(this->extraRamBanks.back(), extraRamBank, RAM_BANK_SIZE);
	}
	for (auto romBank : rhs.romBanks)
	{
		this->romBanks.push_back(new unsigned char[ROM_BANK_SIZE]);
		memcpy(this->romBanks.back(), romBank, ROM_BANK_SIZE);
	}
	isValid = true;
	memSize = MEM_SIZE;
	return *this;
}

void Mem::init()
{
	for (auto const& it: readOnlyBits)
	{
		short address = it.first;
		unsigned char value = it.second;
		internalArray[address] = value;
	}
}

Mem::~Mem()
{
	std::cout << "Mem destroyed !" << std::endl;
	if (!isValid)
		return;

	delete[] internalArray;
	for (unsigned char* ramBank : extraRamBanks)
		delete[] ramBank;
	extraRamBanks.clear();
	for (unsigned char* romBank : romBanks)
		delete[] romBank;
	romBanks.clear();
}

MemWrap Mem::operator[](unsigned int i)
{
	if (i >= memSize)
	{
		std::cerr << "Error, trying to access mem at: " + std::to_string(i) +
				" but mem size is: " + std::to_string(memSize) << std::endl;
		throw("");
	}
	if (!isValid)
	{
		std::cerr << "Error, trying to access uninitialized mem" << std::endl;
		throw("");
	}
	return MemWrap(*this, i, getRefWithBanks(i));
//	return MemWrap(*this, i, internalArray[i]);
}
const MemWrap Mem::operator[](unsigned int i) const
{
	if (i >= memSize)
	{
		std::cerr << "Error, trying to access mem at: " << i <<
			" but mem size is: " << memSize<< std::endl;
		exit(-1);
	}
	if (!isValid)
	{
		std::cerr << "trying to access bad memory" << std::endl;
		exit(-1);
	}
	return MemWrap(*this, i, getRefWithBanks(i));
	//return MemWrap(*this, i, internalArray[i]);
}

unsigned char& Mem::getRefWithBanks(unsigned short addr) const
{
	if (addr <= 0x3FFF)
    {
    		return romBanks[0][addr];
    }
	else if (addr >= 0x4000 && addr <= 0x7FFF) {
//        if (bModeRamBank && romBanks.size() >= 64)
//			selectedRomBank = (ramBankNumber << 5);
        unsigned char selectedRomBank = (UpperRomBankNumber << 5) + LowerRomBankNumber;
        if (!bModeRamBank) // ignore ramBankNumber << 5
            selectedRomBank &= 0x1F;
        if (selectedRomBank == 0 || selectedRomBank == 0x20 || selectedRomBank == 0x40 || selectedRomBank == 0x60 )
            selectedRomBank += 1;
        selectedRomBank &= romBanks.size() - 1; // make sure it's in range
		return romBanks[selectedRomBank][addr-0x4000];
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
        if (!bEnableRam) { return internalArray[addr]; }
        int ramBankNb = !bModeRamBank ? 0 : ramBankNumber;
		return extraRamBanks[ramBankNb][addr - 0xA000];
	}
	else
		return internalArray[addr];
}

unsigned char& MemWrap::operator=(unsigned char newValue) {

	if (addr < 0x7FFF) {
        if (addr >= 0x6000 && addr <= 0x7FFF) {
            if (newValue)
                memRef.bModeRamBank = true; // RAM BANK
            else
                memRef.bModeRamBank = false; // ROMBANK
        }
        if (addr >= 0x4000 && addr <= 0x5FFF) {
            if (!memRef.bModeRamBank && (newValue < memRef.extraRamBanks.size() + 1 || newValue < memRef.romBanks.size()))
                memRef.UpperRomBankNumber = newValue & 0b11;
            else if (memRef.bModeRamBank && (newValue < memRef.extraRamBanks.size() + 1 || newValue < memRef.romBanks.size()))
                memRef.ramBankNumber = newValue & 0b11;
        }
        if (addr >= 0x2000 && addr <= 0x3FFF)// LOWER ROMBANK
        {
            memRef.LowerRomBankNumber = newValue & 0x1F;// shouldnt be equal to 0
        }
		if (addr <= 0x1FFF) {
            memRef.bEnableRam = (newValue &= 0xA);
        }
		return value;// TODO XXX that might pose a problem
	}
	// make sure the new value doesnt override read only bits
	if (Mem::readOnlyBits.count(addr))
		newValue = newValue | Mem::readOnlyBits.at(addr);
	if (addr == LCDC_STATUS) {
		// Ignore bit 7, 1, 2 and 3
		// XXX nallani: that has to be uncorrect, did you mean bit 0, 1, 2 ?
		// fix comment and add it to readOnlyBits
		value &= 0x87;
		value |= (newValue & (~0x87));
		return (value);
	}
	value = newValue;
	//JOYPAD register is 0xFF00
	if (addr == 0xFF00)
		Joypad::refresh();
    /* recursive call
//	if (addr == LCDC) {
//		if ((value & 0x80) == 0) {
//			M_LY = 0;
//		}
//	}
//	if (addr == 0xFF02 && newValue == 0x81)
//	{
//		if (memRef[0xFF01] == ' ')
//		{
//			std::cout << std::endl;
//		}
//		else
//		{
//			std::cout << (char)(memRef[0xFF01]);
//		}
//	}
*/
    // rework too
	if (addr == LYC ) {
		if (value == M_LY)
			SET(M_LCDC_STATUS, 2)
		else
			RES(M_LCDC_STATUS, 2);
	}
	if (addr == LY) {
		if (value == M_LYC)
			SET(M_LCDC_STATUS, 2)
		else
			RES(M_LCDC_STATUS, 2);
	}
	
    if (addr == 0xFF46) {
//		std::cout << "DMA transfert requested at address: " << +newValue << "00" << std::endl;
		if (newValue <= 0xF1) {
			memcpy(&mem[0xFE00], &mem[(newValue << 8)], 0xa0);// TODO change with banks
//			std::cout << "DMA transfert done" << std::endl;
		} //TODO CGB DMA FF51->FF55
	}
	return value;
}

int		Mem::getRomBanksNb(char romSizeCode)
{
	int value = 2;
	switch (romSizeCode)
	{
		case 8:
			value *= 2;
			[[fallthrough]];
		case 7:
			value *= 2;
			[[fallthrough]];
		case 6:
			value *= 2;
			[[fallthrough]];
		case 5:
			value *= 2;
			[[fallthrough]];
		case 4:
			value *= 2;
			[[fallthrough]];
		case 3:
			value *= 2;
			[[fallthrough]];
		case 2:
			value *= 2;
			[[fallthrough]];
		case 1:
			value *= 2;
			[[fallthrough]];
		case 0:
			return value;
		default:
			std::cerr << "wrong romSizeCodeReceived" << std::endl;
			throw("");
	}
}

int		Mem::getExtraRamBanksNb(char ramSizeCode)
{
	switch (ramSizeCode)
	{
		case 0:
			return 0;
		case 2:
			return 1;
		case 3:
			return 4;
		case 4:
			return 16;
		case 5:
			return 8;
		default:
			std::cerr << "wrong ramSizeCode received" << std::endl;
			throw("");
	}
}

std::string Mem::getTitle()
{
	int i = 0x134;
	std::string title;
	while (romBanks[0][i] != '\0' && i < 0x143)
	{
		title += (char)romBanks[0][i];
		i++;
	}
	return title;
}

bool	Mem::isCGB()
{
	return romBanks[0][0x143] | 0xC0;
}

int		Mem::getCartridgeType()
{
	// might be unused with getRamSize / getRomSize instead
	return romBanks[0][0x147];
}
