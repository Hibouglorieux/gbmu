/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cpu.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:46:17 by nallani           #+#    #+#             */
/*   Updated: 2022/11/10 17:47:21 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cpu.hpp"
#include <functional>

unsigned short Cpu::PC = 0;
unsigned short Cpu::SP = 0;
unsigned short Cpu::registers[4] = {};


unsigned char& Cpu::A = reinterpret_cast<unsigned char*>(registers)[1];
unsigned char& Cpu::F = reinterpret_cast<unsigned char*>(registers)[0];
unsigned char& Cpu::B = reinterpret_cast<unsigned char*>(registers)[3];
unsigned char& Cpu::C = reinterpret_cast<unsigned char*>(registers)[2];
unsigned char& Cpu::D = reinterpret_cast<unsigned char*>(registers)[5];
unsigned char& Cpu::E = reinterpret_cast<unsigned char*>(registers)[4];
unsigned char& Cpu::H = reinterpret_cast<unsigned char*>(registers)[7];
unsigned char& Cpu::L = reinterpret_cast<unsigned char*>(registers)[6];

unsigned short& Cpu::AF = registers[0];
unsigned short& Cpu::BC = registers[1];
unsigned short& Cpu::DE = registers[2];
unsigned short& Cpu::HL = registers[3];

Mem Cpu::mem = Mem();
Clock Cpu::clock = Clock(Cpu::mem);

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
}

void Cpu::run(int printStart, int printEnd)
{
	int i = 0;
	while (true)
	{
		//std::string lol;
		//std::getline(std::cin, lol);
		//std::cout << i << std::endl;
		//std::cout << "PC is: 0x" << std::hex << std::setw(4) << PC << std::endl;

		if (i >= printStart && i < printEnd)
		{
			//std::cout << "iteration: " << i << std::endl;
			printRegisters();
		}
		unsigned char opcode = executeInstruction();
		//std::cout << "executed opcode: 0x" << std::hex << std::setw(2) << +opcode << std::endl;
		i++;
	}
}

bool Cpu::loadRom(std::string pathToFile)
{
	mem = Mem(pathToFile);
	return mem.isValid;
}

unsigned char Cpu::executeInstruction()
{
	unsigned char opcode = readByte();
	std::function<unsigned char()> instruction = [](){std::cerr << "wololo" << std::endl; return 2;};
	switch (opcode)
	{
		case 0x00:
			instruction = [&](){ return nop();};
			break;
		case 0x10:
			instruction = [&](){ return stop();};
			break;
		case 0x27:
			instruction = [&](){ return daa();};
			break;
		case 0x2F:
			instruction = [&](){ return cpl();};
			break;
		case 0x37:
			instruction = [&](){ return scf();};
			break;
		case 0x3F:
			instruction = [&](){ return ccf();};
			break;
		case 0x76:
			instruction = [&](){ return halt();};
			break;
		case 0xF3:
			instruction = [&](){ return di();};
			break;
		case 0xFB:
			instruction = [&](){ return ei();};
			break;

		case 0x01:
		case 0x11:
		case 0x21:
		case 0x31:
			instruction = [&](){ return load_r16_from_d16(opcode);};
			break;
		case 0x02:
		case 0x12:
		case 0x22:
		case 0x32:
			instruction = [&](){ return load_r16_a(opcode);};
			break;
		case 0x03:
		case 0x13:
		case 0x23:
		case 0x33:
			instruction = [&](){ return inc_r16(opcode);};
			break;
		case 0x04:
		case 0x0C:
		case 0x14:
		case 0x1C:
		case 0x24:
		case 0x2C:
		case 0x3C:
		case 0x34:
			instruction = [&](){ return inc_r8(opcode);};
			break;
		case 0x05:
		case 0x0D:
		case 0x15:
		case 0x1D:
		case 0x25:
		case 0x2D:
		case 0x3D:
		case 0x35:
			instruction = [&](){ return dec_r8(opcode);};
			break;
		case 0x06:
		case 0x16:
		case 0x26:
		case 0x36:
		case 0x0E:
		case 0x1E:
		case 0x2E:
		case 0x3E:
			instruction = [&](){ return load_r_d8(getTargetRegister(opcode));};
			break;
		case 0x07:
		case 0x0F:
			instruction = [&](){ return rca(opcode);};
			break;
		case 0x08:
			instruction = [&](){ return load_sp_to_a16();};
			break;
		case 0x09:
		case 0x19:
		case 0x29:
		case 0x39:
			instruction = [&](){ return add_hl_r16(opcode);};
			break;
		case 0x0A:
		case 0x1A:
		case 0x2A:
		case 0x3A:
			instruction = [&](){ return load_a_r16(opcode);};
			break;
		case 0x0B:
		case 0x1B:
		case 0x2B:
		case 0x3B:
			instruction = [&](){ return dec_r16(opcode);};
			break;
		case 0x17:
		case 0x1F:
			instruction = [&](){ return ra(opcode);};
			break;
		case 0x18:
			instruction = [&](){ return jr_s8();};
			break;
		case 0x20:
		case 0x28:
		case 0x30:
		case 0x38:
			instruction = [&](){ return jr_s8_flag(opcode);};
			break;
		case 0x40 ... 0x75:
		case 0x77 ... 0x7F:
			instruction = [&](){ return load_r_r(getTargetRegister(opcode), getSourceRegister(opcode));};
			break;
		case 0x80 ... 0x87:
			instruction = [&](){ return add_a_r8(getSourceRegister(opcode));};
			break;
		case 0x88 ... 0x8F:
			instruction = [&](){ return adc_a_r8(getSourceRegister(opcode));};
			break;
		case 0x90 ... 0x97:
			instruction = [&](){ return sub_r8(getSourceRegister(opcode));};
			break;
		case 0x98 ... 0x9F:
			instruction = [&](){ return sbc_r8(getSourceRegister(opcode));};
			break;
		case 0xA0 ... 0xA7:
			instruction = [&](){ return and_r8(getSourceRegister(opcode));};
			break;
		case 0xA8 ... 0xAF:
			instruction = [&](){ return xor_r8(getSourceRegister(opcode));};
			break;
		case 0xB0 ... 0xB7:
			instruction = [&](){ return or_r8(getSourceRegister(opcode));};
			break;
		case 0xB8 ... 0xBF:
			instruction = [&](){ return cp_r8(getSourceRegister(opcode));};
			break;
		case 0xC0:
		case 0xC8:
		case 0xD0:
		case 0xD8:
			instruction = [&](){ return ret_flag(opcode);};
			break;
		case 0xC1:
		case 0xD1:
		case 0xE1:
		case 0xF1:
			instruction = [&](){ return pop(opcode);};
			break;
		case 0xC2:
		case 0xCA:
		case 0xD2:
		case 0xDA:
			instruction = [&](){ return jp_flag_a16(opcode);};
			break;
		case 0xC3:
			instruction = [&](){ return jp_a16();};
			break;
		case 0xC4:
		case 0xCC:
		case 0xD4:
		case 0xDC:
			instruction = [&](){ return call_flag_a16(opcode);};
			break;
		case 0xC5:
		case 0xD5:
		case 0xE5:
		case 0xF5:
			instruction = [&](){ return push(opcode);};
			break;
		case 0xC6:
			instruction = [&](){ return add_a_d8();};
			break;
		case 0xC7:
		case 0xD7:
		case 0xE7:
		case 0xF7:
		case 0xCF:
		case 0xDF:
		case 0xEF:
		case 0xFF:
			instruction = [&](){ return rst_n(opcode);};
			break;
		case 0xC9:
			instruction = [&](){ return ret();};
			break;
		case 0xCD:
			instruction = [&](){ return call_a16();};
			break;
		case 0xCE:
			instruction = [&](){ return adc_a_d8();};
			break;
		case 0xD6:
			instruction = [&](){ return sub_d8();};
			break;
		case 0xD9:
			instruction = [&](){ return reti();};
			break;
		case 0xDE:
			instruction = [&](){ return sbc_d8();};
			break;
		case 0xE0:
			instruction = [&](){ return load_a8_a();};
			break;
		case 0xE2:
			instruction = [&](){ return load_c_a();};
			break;
		case 0xE6:
			instruction = [&](){ return and_d8();};
			break;
		case 0xE8:
			instruction = [&](){ return add_sp_s8();};
			break;
		case 0xE9:
			instruction = [&](){ return jp_hl();};
			break;
		case 0xEA:
			instruction = [&](){ return load_a16_a();};
			break;
		case 0xEE:
			instruction = [&](){ return xor_d8();};
			break;
		case 0xF0:
			instruction = [&](){ return load_a_a8();};
			break;
		case 0xF2:
			instruction = [&](){ return load_a_c();};
			break;
		case 0xF6:
			instruction = [&](){ return or_d8();};
			break;
		case 0xF8:
			instruction = [&](){ return load_hl_from_sp_plus_s8();};
			break;
		case 0xF9:
			instruction = [&](){ return load_sp_from_hl();};
			break;
		case 0xFA:
			instruction = [&](){ return load_a_a16();};
			break;
		case 0xFE:
			instruction = [&](){ return cp_d8();};
			break;

		case 0xCB:
			{
				opcode = readByte();

				unsigned char& targetRegister = getSourceRegister(opcode);
				unsigned char targetBit = getTargetBit(opcode);
				// name might be confusing but it's correct
				/*
				   instruction = [&](){ return let targetRegister: Register8BitIdentifier = get_source_register_with_opcode(opcode);};
				   instruction = [&](){ return let targetBit: u8 = get_targetBit(opcode);};
				   */
				switch (opcode) {
					case 0x00 ... 0x07:
						instruction = [&](){ return rlc_r8(targetRegister);};
						break;
					case 0x08 ... 0x0F:
						instruction = [&](){ return rrc_r8(targetRegister);};
						break;
					case 0x10 ... 0x17:
						instruction = [&](){ return rl_r8(targetRegister);};
						break;
					case 0x18 ... 0x1F:
						instruction = [&](){ return rr_r8(targetRegister);};
						break;
					case 0x20 ... 0x27:
						instruction = [&](){ return sla_r8(targetRegister);};
						break;
					case 0x28 ... 0x2F:
						instruction = [&](){ return sra_r8(targetRegister);};
						break;
					case 0x30 ... 0x37:
						instruction = [&](){ return swap_r8(targetRegister);};
						break;
					case 0x38 ... 0x3F:
						instruction = [&](){ return srl_r8(targetRegister);};
						break;
					case 0x40 ... 0x7F:
						instruction = [&](){ return bit_n_r8(targetBit, targetRegister);};
						break;
					case 0x80 ... 0xBF:
						instruction = [&](){ return res_n_r8(targetBit, targetRegister);};
						break;
					case 0xC0 ... 0xFF:
						instruction = [&](){ return set_n_r8(targetBit, targetRegister);};
						break;
					default:
						logErr("exec: Error unknown prefix instruction opcode");
				}
			}
			break;
		default:
			{
				std::cerr << "previuous opcode: 0x" << std::hex << +mem[PC - 2] << std::endl;
				logErr(string_format("exec: Error unknown instruction opcode: 0x%X", opcode));
			}
	}
	clock += instruction();
	return opcode;
}
