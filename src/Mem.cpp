/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mem.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:49:00 by nallani           #+#    #+#             */
/*   Updated: 2022/12/09 05:57:44 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mem.hpp"
#include "Cpu.hpp"
#include <fstream>
#include <iostream>

#define MEM_SIZE (0xFFFF + 1)

Mem::Mem()
{
	isValid = true;
	internalArray = new unsigned char[MEM_SIZE];
	memSize = MEM_SIZE;
}

Mem::Mem(int size)
{
	isValid = true;
	internalArray = new unsigned char[size];
	memSize = size;
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
	value = newValue;
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
    if (addr == 0xFF46) {
		std::cout << "DMA transfert requested at address: " << +newValue << "00" << std::endl;
		if (newValue <= 0xF1) {
			memcpy(&mem[0xFE00], &mem[(newValue << 8)], 0x9f);
			std::cout << "DMA transfert done" << std::endl;
		} //TODO CGB DMA FF51->FF55
	}
	return value;
}
