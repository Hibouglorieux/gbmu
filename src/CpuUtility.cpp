/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CpuUtility.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/09 20:25:14 by nallani           #+#    #+#             */
/*   Updated: 2022/11/09 22:34:11 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cpu.hpp"
#include <iostream>

unsigned char	Cpu::getTargetBit(unsigned short opcode)
{
    // opcode % 40 removes the offset for first operation that need this function (0x40-0x7f)
    // & 0xF0 >> 4 to get the higher bit, multiplied by 2 to get the result
    unsigned char nb = (((opcode % 0x40) & 0xF0) >> 4) * 2;
    if (((opcode & 0xF) % 0x7) == 1)
        nb++;
    return nb;
}

unsigned char&	Cpu::getTargetRegister(unsigned short opcode)
{
	if (opcode > 0x40 && opcode <= 0x8f)
	{
		switch ((opcode - 0x40) / 0x8)
		{
			case 0x07:
				return A;
			case 0x00:
				return B;
			case 0x01:
				return C;
			case 0x02:
				return D;
			case 0x03:
				return E;
			case 0x04:
				return H;
			case 0x05:
				return L;
			case 0x06:
				return PHL;
			default:
				logErr("Opcode is too big, shouldn't be more than 0x7F");
		}
	}
	else if ((opcode & 0x0F) == 0x0E)
	{
		switch (opcode)
		{
			case 0x0E:
				return C;
			case 0x1E:
				return E;
			case 0x2E:
				return L;
			case 0x3E:
				return A;
			default:
				logErr("wrong opcode sent to getTargetRegister");
		}
	}
	else if ((opcode & 0x0F) == 0x06)
	{
		switch (opcode)
		{
			case 0x06:
				return B;
			case 0x16:
				return D;
			case 0x26:
				return H;
			case 0x36:
				return PHL;
			default:
				logErr("unkown opcode sent to getTargetRegister");
		}
	}
	logErr("Opcode is too little, should at least 0x40");
	exit(-1);
}

unsigned char&	Cpu::getSourceRegister(unsigned short opcode)
{
	switch(opcode & 0x07 )
	{
		case 0x07:
			return A;
		case 0x00:
			return B;
		case 0x01:
			return C;
		case 0x02:
			return D;
		case 0x03:
			return E;
		case 0x04:
			return H;
		case 0x05:
			return L;
		case 0x06:
			return PHL;
		default:
			logErr("called getSourceRegister with wrong opcode");
	}
	exit(-1);
}

void	Cpu::setZeroFlag(bool value)
{
	if (value)
		F |= value << 7;
	else
		F &= value << 7;
}

void	Cpu::setSubtractFlag(bool value)
{
	if (value)
		F |= value << 6;
	else
		F &= value << 6;
}

void	Cpu::setHalfCarryFlag(bool value)
{
	if (value)
		F |= value << 5;
	else
		F &= value << 5;
}

void	Cpu::setCarryFlag(bool value)
{
	if (value)
		F |= value << 4;
	else
		F &= value << 4;
}

void	Cpu::setFlags(bool zero, bool sub, bool halfCarry, bool carry)
{
	setZeroFlag(zero);
	setSubtractFlag(sub);
	setHalfCarryFlag(halfCarry);
	setCarryFlag(carry);
}

bool	Cpu::getZeroFlag()
{
	return (F >> 7) & 1;
}

bool	Cpu::getSubtractFlag()
{
	return (F >> 6) & 1;
}

bool	Cpu::getHalfCarryFlag()
{
	return (F >> 5) & 1;
}

bool	Cpu::getCarryFlag()
{
	return (F >> 4) & 1;
}

unsigned char Cpu::readByte()
{
	return mem[PC++];
}

unsigned short Cpu::readShort()
{
	unsigned short shortVal = readByte();
	shortVal += ((unsigned short)readByte() << 8);
	return shortVal;
}

const unsigned char& Cpu::getData(int i)
{
	return mem[i];
}

void Cpu::logErr(std::string msg)
{
	std::cerr << msg << std::endl;
	exit(-1);
}
