/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Instructions.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 21:08:41 by nallani           #+#    #+#             */
/*   Updated: 2022/11/07 22:12:19 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cpu.hpp"

unsigned char Cpu::nop()
{
	return 0;
}

unsigned char Cpu::stop()
{
	return 0;
}

unsigned char Cpu::daa()
{
	return 0;
}

unsigned char Cpu::cpl()
{
	return 0;
}

unsigned char Cpu::scf()
{
	return 0;
}

unsigned char Cpu::ccf()
{
	return 0;
}

unsigned char Cpu::halt()
{
	return 0;
}

unsigned char Cpu::di()
{
	return 0;
}

unsigned char Cpu::ei()
{
	return 0;
}


unsigned char Cpu::load_r16_from_d16(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::load_r16_a(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::inc_r16(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::inc_r8(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::dec_r8(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::load_r_d8(unsigned char& loadTarget)
{
	return 0;
}

unsigned char Cpu::rca(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::load_sp_to_a16(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::add_hl_r16(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::load_a_r16(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::dec_r16(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::ra(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::jr_s8()
{
	return 0;
}

unsigned char Cpu::jr_s8_flag(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::load_hl_d8()
{
	return 0;
}

unsigned char Cpu::load_r_r(unsigned char& loadTarget, unsigned char loadSource)
{
	return 0;
}

unsigned char Cpu::load_r_hl(unsigned char& loadTarget, unsigned char loadSource)
{
	return 0;
}

unsigned char Cpu::load_hl_r(unsigned char& loadTarget, unsigned char loadSource)
{
	return 0;
}

unsigned char Cpu::add_a_r8(unsigned char& loadSource)
{
	return 0;
}

unsigned char Cpu::add_a_hl()
{
	return 0;
}

unsigned char Cpu::adc_a_r8(unsigned char& loadSource)
{
	return 0;
}

unsigned char Cpu::adc_a_hl()
{
	return 0;
}

unsigned char Cpu::sub_r8(unsigned char& loadSource)
{
	return 0;
}

unsigned char Cpu::sub_hl()
{
	return 0;
}

unsigned char Cpu::sbc_r8(unsigned char& loadSource)
{
	return 0;
}

unsigned char Cpu::sbc_hl()
{
	return 0;
}

unsigned char Cpu::and_r8(unsigned char& loadSource)
{
	return 0;
}

unsigned char Cpu::and_hl()
{
	return 0;
}

unsigned char Cpu::xor_r8(unsigned char& loadSource)
{
	return 0;
}

unsigned char Cpu::or_r8(unsigned char& loadSource)
{
	return 0;
}

unsigned char Cpu::cp_r8(unsigned char& loadSource)
{
	return 0;
}

unsigned char Cpu::ret_flag(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::pop(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::jp_flag_a16(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::jp_a16()
{
	return 0;
}

unsigned char Cpu::call_flag_a16(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::push(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::add_a_d8()
{
	return 0;
}

unsigned char Cpu::rst_n(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::ret()
{
	return 0;
}

unsigned char Cpu::call_a16()
{
	return 0;
}

unsigned char Cpu::adc_a_d8()
{
	return 0;
}

unsigned char Cpu::sub_d8()
{
	return 0;
}

unsigned char Cpu::reti()
{
	return 0;
}

unsigned char Cpu::sbc_d8()
{
	return 0;
}

unsigned char Cpu::load_a8_a()
{
	return 0;
}

unsigned char Cpu::load_c_a()
{
	return 0;
}

unsigned char Cpu::and_d8()
{
	return 0;
}

unsigned char Cpu::add_sp_s8(unsigned short opcode)
{
	return 0;
}

unsigned char Cpu::jp_hl()
{
	return 0;
}

unsigned char Cpu::load_a16_a()
{
	return 0;
}

unsigned char Cpu::xor_d8()
{
	return 0;
}

unsigned char Cpu::load_a_a8()
{
	return 0;
}

unsigned char Cpu::load_a_c()
{
	return 0;
}

unsigned char Cpu::or_d8()
{
	return 0;
}

unsigned char Cpu::load_hl_from_sp_plus_s8()
{
	return 0;
}

unsigned char Cpu::load_sp_from_hl()
{
	return 0;
}

unsigned char Cpu::load_a_a16()
{
	return 0;
}

unsigned char Cpu::cp_d8()
{
	return 0;
}



unsigned char Cpu::rlc_r8(unsigned char& targetRegister)
{
	return 0;
}

unsigned char Cpu::rrc_r8(unsigned char& targetRegister)
{
	return 0;
}

unsigned char Cpu::rl_r8(unsigned char& targetRegister)
{
	return 0;
}

unsigned char Cpu::rr_r8(unsigned char& targetRegister)
{
	return 0;
}

unsigned char Cpu::sla_r8(unsigned char& targetRegister)
{
	return 0;
}

unsigned char Cpu::sra_r8(unsigned char& targetRegister)
{
	return 0;
}

unsigned char Cpu::swap_r8(unsigned char& targetRegister)
{
	return 0;
}

unsigned char Cpu::srl_r8(unsigned char& targetRegister)
{
	return 0;
}

unsigned char Cpu::bit_n_r8(unsigned char targetBit, unsigned char& targetRegister)
{
	return 0;
}

unsigned char Cpu::res_n_r8(unsigned char targetBit, unsigned char& targetRegister)
{
	return 0;
}

unsigned char Cpu::set_n_r8(unsigned char targetBit, unsigned char& targetRegister)
{
	return 0;
}

