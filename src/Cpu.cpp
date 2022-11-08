/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cpu.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:46:17 by nallani           #+#    #+#             */
/*   Updated: 2022/11/08 15:49:19 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cpu.hpp"
#include <iostream>

#define PHL mem[HL]

unsigned short Cpu::PC = 0;
unsigned short Cpu::SP = 0;
unsigned char Cpu::registers[8] = {};

unsigned char& Cpu::A = registers[7];
unsigned char& Cpu::B = registers[6];
unsigned char& Cpu::C = registers[5];
unsigned char& Cpu::D = registers[4];
unsigned char& Cpu::E = registers[3];
unsigned char& Cpu::F = registers[2];
unsigned char& Cpu::H = registers[1];
unsigned char& Cpu::L = registers[0];

unsigned short& Cpu::BC = *reinterpret_cast<unsigned short*>(&C);
unsigned short& Cpu::DE = *reinterpret_cast<unsigned short*>(&E);
unsigned short& Cpu::HL = *reinterpret_cast<unsigned short*>(&L);

Mem Cpu::mem = Mem();
Clock Cpu::clock = Clock();

unsigned char Cpu::readByte()
{
	return mem[PC++];
}

unsigned short Cpu::readShort()
{
	unsigned short shortVal = readByte();
	shortVal += ((unsigned short)readByte() << 7);
	return shortVal;
}

const unsigned char& Cpu::getData(int i)
{
	return mem[i];
}

unsigned char& Cpu::getLoadSource(unsigned int opcode)
{
	switch (opcode & 0x07)
	{
		case 0x7:
			return A;
		case 0x0:
			return B;
		case 0x1:
			return C;
		case 0x2:
			return D;
		case 0x3:
			return E;
		case 0x4:
			return H;
		case 0x5:
			return L;
		case 0x6:
			return PHL;
	}
	std::cout << "Error with SourceTarget not in range with opcode: "
		<< opcode << std::endl;
	exit(-1);
}

unsigned char& Cpu::getLoadTarget(unsigned int opcode)
{
    if (opcode > 0x40 && opcode <= 0x8F)
	{
        switch ((opcode - 0x40) / 0x08)
		{
			case 0x7:
				return A;
			case 0x0:
				return B;
			case 0x1:
				return C;
			case 0x2:
				return D;
			case 0x3:
				return E;
			case 0x4:
				return H;
			case 0x5:
				return L;
			case 0x6:
				return PHL;
			default:
				std::cout << "Error with LoadTarget not in range with opcode: "
					<< opcode << std::endl;
				exit(-1);
        }
    }
    if ((opcode & 0x0F) == 0x0E)
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
				std::cout << "Error with LoadTarget not in range with opcode: "
					<< opcode << std::endl;
				exit(-1);
        }
    }
    if ((opcode & 0x0F) == 0x06)
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
				std::cout << "Error with LoadTarget not in range with opcode: "
					<< opcode << std::endl;
				exit(-1);
        }
    }
	std::cout << "Error with LoadTarget not in range with opcode: "
		<< opcode << std::endl;
	exit(-1);
}

unsigned char Cpu::getTargetBit(unsigned int opcode)
{
    // opcode % 40 removes the offset for first operation that need this function (0x40-0x7f)
    // & 0xF0 >> 4 to get the higher bit, multiplied by 2 to get the result
    unsigned char nb = (((opcode % 0x40) & 0xF0) >> 4) * 2;
    if ((opcode & 0xF) % 0x7 == 1) {
        nb++;
    }
    return nb;
}

void Cpu::loadBootRom()
{
	PC = 0x100;
	BC = 0x0000;
	DE = 0xFF56;
	HL = 0x000D;
	SP = 0xFFFE;
	A = 0x11;
	F = 0x80;
	mem[0xFF44] = 0x90;
	clock = 32916 / 2;
}


void Cpu::executeInstruction()
{
	unsigned char opcode = readByte();
	unsigned char& loadSource = getLoadSource(opcode);
	unsigned char& loadTarget = getLoadTarget(opcode);
	switch (opcode)
	{
		case 0x00:
			nop();
			break;
		case 0x10:
			stop();
			break;
		case 0x27:
			daa();
			break;
		case 0x2F:
			cpl();
			break;
		case 0x37:
			scf();
			break;
		case 0x3F:
			ccf();
			break;
		case 0x76:
			halt();
			break;
		case 0xF3:
			di();
			break;
		case 0xFB:
			ei();
			break;

		case 0x01:
		case 0x11:
		case 0x21:
		case 0x31:
			load_r16_from_d16(opcode);
			break;
		case 0x02:
		case 0x12:
		case 0x22:
		case 0x32:
			load_r16_a(opcode);
			break;
		case 0x03:
		case 0x13:
		case 0x23:
		case 0x33:
			inc_r16(opcode);
			break;
		case 0x04:
		case 0x0C:
		case 0x14:
		case 0x1C:
		case 0x24:
		case 0x2C:
		case 0x3C:
		case 0x34:
			inc_r8(opcode);
			break;
		case 0x05:
		case 0x0D:
		case 0x15:
		case 0x1D:
		case 0x25:
		case 0x2D:
		case 0x3D:
		case 0x35:
			dec_r8(opcode);
			break;
		case 0x06:
		case 0x16:
		case 0x26:
		case 0x0E:
		case 0x1E:
		case 0x2E:
		case 0x3E:
			load_r_d8(loadTarget);
			break;
		case 0x07:
		case 0x0F:
			rca(opcode);
			break;
		case 0x08:
			load_sp_to_a16(opcode);
			break;
		case 0x09:
		case 0x19:
		case 0x29:
		case 0x39:
			add_hl_r16(opcode);
			break;
		case 0x0A:
		case 0x1A:
		case 0x2A:
		case 0x3A:
			load_a_r16(opcode);
			break;
		case 0x0B:
		case 0x1B:
		case 0x2B:
		case 0x3B:
			dec_r16(opcode);
			break;
		case 0x17:
		case 0x1F:
			ra(opcode);
			break;
		case 0x18:
			jr_s8();
			break;
		case 0x20:
		case 0x28:
		case 0x30:
		case 0x38:
			jr_s8_flag(opcode);
			break;
		case 0x36:
			load_hl_d8();
			break;
		case 0x40 ... 0x75:
		case 0x77 ... 0x7F:
			load_r_r(loadTarget, loadSource);
			break;
			/*
		case 0x46:
		case 0x56:
		case 0x66:
		case 0x4E:
		case 0x5E:
		case 0x6E:
		case 0x7E:
			load_r_hl(loadTarget, loadSource);
			break;
		case 0x70 ... 0x75:
		case 0x77:
			load_hl_r(loadTarget, loadSource);
			break;
			*/
		case 0x80 ... 0x87: // 0x86 problem
			add_a_r8(loadSource);
			break;
			/*
		case 0x86:
			add_a_hl();
			break;
			*/
		case 0x88 ... 0x8F: // 0x8E probllem
			adc_a_r8(loadSource);
			break;
			/*
		case 0x8E:
			adc_a_hl();
			break;
			*/
		case 0x90 ... 0x97: // 0x96 problem
			sub_r8(loadSource);
			break;
			/*
		case 0x96:
			sub_hl();
			break;
			*/
		case 0x98 ... 0x9F: // 0x9e problem
			sbc_r8(loadSource);
			break;
			/*
		case 0x9E:
			sbc_hl();
			break;
			*/
		case 0xA0 ... 0xA7: // 0xA6 problem
			and_r8(loadSource);
			break;
			/*
		case 0xA6:
			and_hl();
			break;
			*/
		case 0xA8 ... 0xAF:
			xor_r8(loadSource);
			break;
		case 0xB0 ... 0xB7:
			or_r8(loadSource);
			break;
		case 0xB8 ... 0xBF:
			cp_r8(loadSource);
			break;
		case 0xC0:
		case 0xC8:
		case 0xD0:
		case 0xD8:
			ret_flag(opcode);
			break;
		case 0xC1:
		case 0xD1:
		case 0xE1:
		case 0xF1:
			pop(opcode);
			break;
		case 0xC2:
		case 0xCA:
		case 0xD2:
		case 0xDA:
			jp_flag_a16(opcode);
			break;
		case 0xC3:
			jp_a16();
			break;
		case 0xC4:
		case 0xCC:
		case 0xD4:
		case 0xDC:
			call_flag_a16(opcode);
			break;
		case 0xC5:
		case 0xD5:
		case 0xE5:
		case 0xF5:
			push(opcode);
			break;
		case 0xC6:
			add_a_d8();
			break;
		case 0xC7:
		case 0xD7:
		case 0xE7:
		case 0xF7:
		case 0xCF:
		case 0xDF:
		case 0xEF:
		case 0xFF:
			rst_n(opcode);
			break;
		case 0xC9:
			ret();
			break;
		case 0xCD:
			call_a16();
			break;
		case 0xCE:
			adc_a_d8();
			break;
		case 0xD6:
			sub_d8();
			break;
		case 0xD9:
			reti();
			break;
		case 0xDE:
			sbc_d8();
			break;
		case 0xE0:
			load_a8_a();
			break;
		case 0xE2:
			load_c_a();
			break;
		case 0xE6:
			and_d8();
			break;
		case 0xE8:
			add_sp_s8(opcode);
			break;
		case 0xE9:
			jp_hl();
			break;
		case 0xEA:
			load_a16_a();
			break;
		case 0xEE:
			xor_d8();
			break;
		case 0xF0:
			load_a_a8();
			break;
		case 0xF2:
			load_a_c();
			break;
		case 0xF6:
			or_d8();
			break;
		case 0xF8:
			load_hl_from_sp_plus_s8();
			break;
		case 0xF9:
			load_sp_from_hl();
			break;
		case 0xFA:
			load_a_a16();
			break;
		case 0xFE:
			cp_d8();
			break;

		case 0xCB:
			{
				opcode = readByte();

				unsigned char& targetRegister = A;
				unsigned char targetBit = 1;
				// name might be confusing but it's correct
				/*
				   let targetRegister: Register8BitIdentifier = get_source_register_with_opcode(opcode);
				   let targetBit: u8 = get_targetBit(opcode);
				   */
				switch (opcode) {
					case 0x00 ... 0x07:
						rlc_r8(targetRegister);
						break;
					case 0x08 ... 0x0F:
						rrc_r8(targetRegister);
						break;
					case 0x10 ... 0x17:
						rl_r8(targetRegister);
						break;
					case 0x18 ... 0x1F:
						rr_r8(targetRegister);
						break;
					case 0x20 ... 0x27:
						sla_r8(targetRegister);
						break;
					case 0x28 ... 0x2F:
						sra_r8(targetRegister);
						break;
					case 0x30 ... 0x37:
						swap_r8(targetRegister);
						break;
					case 0x38 ... 0x3F:
						srl_r8(targetRegister);
						break;
					case 0x40 ... 0x7F:
						bit_n_r8(targetBit, targetRegister);
						break;
					case 0x80 ... 0xBF:
						res_n_r8(targetBit, targetRegister);
						break;
					case 0xC0 ... 0xFF:
						set_n_r8(targetBit, targetRegister);
						break;
					default:
						std::cerr << "exec: Error unknown prefix instruction opcode" << std::endl;
						exit(-1);
				}
			}
			break;
	}
}
