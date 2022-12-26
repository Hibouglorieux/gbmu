/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mem.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:49:00 by nallani           #+#    #+#             */
/*   Updated: 2022/12/26 16:32:14 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mem.hpp"
#include "Cpu.hpp"
#include <fstream>
#include <iostream>
#include "Joypad.hpp"

#define MEM_SIZE (0xFFFF + 1)

const std::map<unsigned short, unsigned char> Mem::readOnlyBits = {
 {0xFF00, 0b1100'1111}, // 0xCF, 0xFF00 is input register, first 4 bit are
						// set to 0 when pressed, last 2 are unused
 {0xFF02, 0x7C},
				
};

Mem::Mem()
{
	isValid = true;
	internalArray = new unsigned char[MEM_SIZE];
	memSize = MEM_SIZE;
	init();
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

Mem::Mem(std::string pathToRom)
{
	std::ifstream file = std::ifstream(pathToRom, std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "file not found" << std::endl;
		isValid = false;
		memSize = 0;
		return;
	}
	internalArray = new unsigned char[MEM_SIZE];
	file.read((char*)internalArray, MEM_SIZE);
	file.close();
	isValid = true;
	memSize = MEM_SIZE;
	init();
}

Mem::Mem(const Mem& rhs)
{
	if (!rhs.isValid)
	{
		isValid = false;
		memSize = 0;
		return;
	}
	this->internalArray = new unsigned char[MEM_SIZE];
	for (int i = 0; i < MEM_SIZE; i++)
		this->internalArray[i] = rhs.internalArray[i];
	isValid = true;
	memSize = MEM_SIZE;
}

const Mem&	Mem::operator=(const Mem& rhs)
{
	if (isValid)
		delete[] internalArray;
	if (!rhs.isValid)
	{
		isValid = false;
		memSize = 0;
		return *this;
	}
	this->internalArray = new unsigned char[MEM_SIZE];
	for (int i = 0; i < MEM_SIZE; i++)
		this->internalArray[i] = rhs.internalArray[i];
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
	if (isValid)
		delete[] internalArray;
}

MemWrap Mem::operator[](unsigned int i)
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
	return MemWrap(*this, i, internalArray[i]);
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
	return MemWrap(*this, i, internalArray[i]);
}

unsigned char& MemWrap::operator=(unsigned char newValue)
{
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
	if (addr == 0xFF00) {
		Joypad::refresh();
		return (value);
	}
	if (addr == LCDC) {
		if ((value & 0x80) == 0) {
			M_LY = 0;
		}
	}
	if (addr == 0xFF02 && newValue == 0x81)
	{
		if (memRef[0xFF01] == ' ')
		{
			std::cout << std::endl;
		}
		else
		{
			std::cout << (char)(memRef[0xFF01]);
		}
	}

	if (addr == LYC) {
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
	
	// if (addr == LYC)
		// std::cout << "LYC: " << (int)old << " -> " << (int)value << "\n";
    if (addr == 0xFF46) {
		//std::cout << "DMA transfert requested at address: " << +newValue << "00" << std::endl;
		if (newValue <= 0xF1) {
			memcpy(&mem[0xFE00], &mem[(newValue << 8)], 0x9f);
			//std::cout << "DMA transfert done" << std::endl;
		} //TODO CGB DMA FF51->FF55
	}
	return value;
}
