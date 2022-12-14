/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cpu.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:46:19 by nallani           #+#    #+#             */
/*   Updated: 2023/01/05 22:50:59 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CPU_CLASS_H
# define CPU_CLASS_H

#include "Gameboy.hpp"
#include "Utility.hpp"
#include "CpuStackTrace.hpp"
#include <string>
#include <iostream>
#include <iomanip> // setw
#include <utility>
#include "define.hpp"


class Cpu {
public:

	static CpuStackTrace stackTrace;
	static StackData captureCurrentState(std::string customData = "");

	static unsigned char	doMinimumStep();
	static bool isCpuHalted();

	static int executeLine(bool step, bool updateState, bool bRefreshScreen);
	static void loadBootRom();
	static std::pair<unsigned char, int> executeInstruction();
	static void	updateLY(int iter);

	static unsigned char readByte(int incrementPc = true);
	static unsigned short readShort(int incrementPc = true);

	static void setZeroFlag(bool value);
	static void setSubtractFlag(bool value);
	static void setHalfCarryFlag(bool value);
	static void setCarryFlag(bool value);
	static void setFlags(bool zero, bool sub, bool halfCarry, bool carry);
	static bool getZeroFlag();
	static bool getSubtractFlag();
	static bool getHalfCarryFlag();
	static bool getCarryFlag();

	static void	request_interrupt(int i);


	static const unsigned char& getData(int i);
//	static void setData(int i);

	static bool interrupts_master_enable;
	static bool interrupts_flag;
	static bool halted;
	static uint32_t halt_counter;
	static bool handle_interrupts();


	static unsigned short PC;
	static unsigned short SP;
	static unsigned short registers[4];
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
	static void printRegisters();
	// static Clock cpuClock;

	static void debug(int opcode);
private:


	static unsigned char& getTargetRegister(unsigned short opcode);
	static unsigned char getSourceRegister(unsigned short opcode);
	static unsigned char& getSourceRegisterRef(unsigned short opcode);
	static unsigned char getTargetBit(unsigned short opcode);

	static bool getHalfCarry8Bit(unsigned char a, unsigned char b, unsigned char c = 0);
	static bool getHalfCarry16Bit(unsigned short a, unsigned short b);
	static bool getHalfBorrow8Bit(unsigned char a, unsigned char b, unsigned char c = 0);
	static bool getHalfBorrow16Bit(unsigned short a, unsigned short b);
	static bool overFlow(unsigned char a, unsigned char b, unsigned char c = 0);
	static bool underFlow(unsigned char a, unsigned char b, unsigned char c = 0);

	static void logErr(std::string msg);

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
	static unsigned char load_hl_d8();
	static unsigned char rca(unsigned short opcode);
	static unsigned char load_sp_to_a16();
	static unsigned char add_hl_r16(unsigned short opcode);
	static unsigned char load_a_r16(unsigned short opcode);
	static unsigned char dec_r16(unsigned short opcode);
	static unsigned char ra(unsigned short opcode);
	static unsigned char jr_s8();
	static unsigned char jr_s8_flag(unsigned short opcode);
	static unsigned char load_r_r(unsigned char& loadTarget, unsigned char loadSource);
	static unsigned char load_hl_r(unsigned char loadSource);
	static unsigned char load_r_hl(unsigned char& loadTarget);
	static unsigned char add_a_r8(unsigned char reg);
	static unsigned char adc_a_r8(unsigned char reg);
	static unsigned char sub_r8(unsigned char reg);
	static unsigned char sbc_r8(unsigned char reg);
	static unsigned char and_r8(unsigned char reg);
	static unsigned char xor_r8(unsigned char reg);
	static unsigned char or_r8(unsigned char reg);
	static unsigned char cp_r8(unsigned char reg);
	static unsigned char add_a_phl();
	static unsigned char adc_a_phl();
	static unsigned char sub_phl();
	static unsigned char sbc_phl();
	static unsigned char and_phl();
	static unsigned char xor_phl();
	static unsigned char or_phl();
	static unsigned char cp_phl();
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
	static unsigned char add_sp_s8();
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

	static unsigned char rlc_phl();
	static unsigned char rrc_phl();
	static unsigned char rl_phl();
	static unsigned char rr_phl();
	static unsigned char sla_phl();
	static unsigned char sra_phl();
	static unsigned char swap_phl();
	static unsigned char srl_phl();
	static unsigned char bit_n_phl(unsigned char targetBit);
	static unsigned char res_n_phl(unsigned char targetBit);
	static unsigned char set_n_phl(unsigned char targetBit);


	static void internalPush(unsigned short valueToPush);
	static unsigned short internalPop();
};

void do_interrupts(unsigned int addr, unsigned char bit);

#endif
