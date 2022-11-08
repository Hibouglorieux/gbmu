/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cpu.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:46:19 by nallani           #+#    #+#             */
/*   Updated: 2022/11/08 15:48:53 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CPU_CLASS_H
# define CPU_CLASS_H

#include "Mem.hpp"
#include "Clock.hpp"

class Cpu {
public:

	static void loadBootRom();
	static void executeInstruction();

	static unsigned char readByte();
	static unsigned short readShort();


	static const unsigned char& getData(int i);
	static void setData(int i);

	static unsigned short PC;
	static unsigned short SP;
	static unsigned char registers[8];
	static unsigned char& A;
	static unsigned char& B;
	static unsigned char& C;
	static unsigned char& D;
	static unsigned char& E;
	static unsigned char& F;
	static unsigned char& H;
	static unsigned char& L;

	static unsigned short& AF;
	static unsigned short& BC;
	static unsigned short& DE;
	static unsigned short& HL;
	static Mem mem;
	static Clock clock;
private:
	static unsigned char& getLoadSource(unsigned int opcode);
	static unsigned char& getLoadTarget(unsigned int opcode);
	static unsigned char getTargetBit(unsigned int opcode);

	static unsigned char nop();
	static unsigned char stop();
	static unsigned char daa();
	static unsigned char cpl();
	static unsigned char scf();
	static unsigned char ccf();
	static unsigned char halt();
	static unsigned char di();
	static unsigned char ei();

	static unsigned char load_r16_from_d16(unsigned short opcode);
	static unsigned char load_r16_a(unsigned short opcode);
	static unsigned char inc_r16(unsigned short opcode);
	static unsigned char inc_r8(unsigned short opcode);
	static unsigned char dec_r8(unsigned short opcode);
	static unsigned char load_r_d8(unsigned char& loadTarget);
	static unsigned char rca(unsigned short opcode);
	static unsigned char load_sp_to_a16(unsigned short opcode);
	static unsigned char add_hl_r16(unsigned short opcode);
	static unsigned char load_a_r16(unsigned short opcode);
	static unsigned char dec_r16(unsigned short opcode);
	static unsigned char ra(unsigned short opcode);
	static unsigned char jr_s8();
	static unsigned char jr_s8_flag(unsigned short opcode);
	static unsigned char load_hl_d8();
	static unsigned char load_r_r(unsigned char& loadTarget, unsigned char loadSource);
	static unsigned char load_r_hl(unsigned char& loadTarget, unsigned char loadSource);
	static unsigned char load_hl_r(unsigned char& loadTarget, unsigned char loadSource);
	static unsigned char add_a_r8(unsigned char& loadSource);
	static unsigned char add_a_hl();
	static unsigned char adc_a_r8(unsigned char& loadSource);
	static unsigned char adc_a_hl();
	static unsigned char sub_r8(unsigned char& loadSource);
	static unsigned char sub_hl();
	static unsigned char sbc_r8(unsigned char& loadSource);
	static unsigned char sbc_hl();
	static unsigned char and_r8(unsigned char& loadSource);
	static unsigned char and_hl();
	static unsigned char xor_r8(unsigned char& loadSource);
	static unsigned char or_r8(unsigned char& loadSource);
	static unsigned char cp_r8(unsigned char& loadSource);
	static unsigned char ret_flag(unsigned short opcode);
	static unsigned char pop(unsigned short opcode);
	static unsigned char jp_flag_a16(unsigned short opcode);
	static unsigned char jp_a16();
	static unsigned char call_flag_a16(unsigned short opcode);
	static unsigned char push(unsigned short opcode);
	static unsigned char add_a_d8();
	static unsigned char rst_n(unsigned short opcode);
	static unsigned char ret();
	static unsigned char call_a16();
	static unsigned char adc_a_d8();
	static unsigned char sub_d8();
	static unsigned char reti();
	static unsigned char sbc_d8();
	static unsigned char load_a8_a();
	static unsigned char load_c_a();
	static unsigned char and_d8();
	static unsigned char add_sp_s8(unsigned short opcode);
	static unsigned char jp_hl();
	static unsigned char load_a16_a();
	static unsigned char xor_d8();
	static unsigned char load_a_a8();
	static unsigned char load_a_c();
	static unsigned char or_d8();
	static unsigned char load_hl_from_sp_plus_s8();
	static unsigned char load_sp_from_hl();
	static unsigned char load_a_a16();
	static unsigned char cp_d8();

	// 0xCB opcodes
	static unsigned char rlc_r8(unsigned char& targetRegister);
	static unsigned char rrc_r8(unsigned char& targetRegister);
	static unsigned char rl_r8(unsigned char& targetRegister);
	static unsigned char rr_r8(unsigned char& targetRegister);
	static unsigned char sla_r8(unsigned char& targetRegister);
	static unsigned char sra_r8(unsigned char& targetRegister);
	static unsigned char swap_r8(unsigned char& targetRegister);
	static unsigned char srl_r8(unsigned char& targetRegister);
	static unsigned char bit_n_r8(unsigned char targetBit, unsigned char& targetRegister);
	static unsigned char res_n_r8(unsigned char targetBit, unsigned char& targetRegister);
	static unsigned char set_n_r8(unsigned char targetBit, unsigned char& targetRegister);
};

#endif
