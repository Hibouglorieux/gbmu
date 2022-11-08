/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Instructions16BitArithmetic.cpp                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 16:03:50 by nallani           #+#    #+#             */
/*   Updated: 2022/11/08 16:23:18 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cpu.hpp"

unsigned char Cpu::dec_r16(unsigned short opcode)
{
	// Opcode: [0x0B, 0x1B, 0x2B, 0x3B]
	// Symbol: DECL
	// Operands: [(BC), (DE), (HL), (SP)]
	// Number of Bytes: 1
	// Number of Cycles: 2
	// Flags: - - - -
	// Description
	// Decrement the contents of register pair reg by 1.

	unsigned short* reg;
	switch (opcode)
	{
		case 0x0B:
			reg = &BC;
			break;
		case 0x1B:
			reg = &DE;
			break;
		case 0x2B:
			reg = &HL;
			break;
		case 0x3B:
			reg = &SP;
			break;
		default:
			logErr("dec_r16 received wrong opcode");
	}

	*reg--;

	return 2;
}

unsigned char Cpu::inc_r16(unsigned short opcode)
{
	// Opcode: [0x03, 0x13, 0x23, 0x33]
	// Symbol: INCL
	// Operands: [(BC), (DE), (HL), (SP)]
	// Number of Bytes: 1
	// Number of Cycles: 2
	// Flags: - - - -
	// Description
	// Increment the contents of register pair reg by 1.

	unsigned short* reg;
   	switch (opcode)
	{
		case 0x03:
			reg = BC;
			break;
		case 0x13:
			reg = DE;
			break;
		case 0x23:
			reg = HL;
			break;
		case 0x33:
			reg = SP;
			break;
		default:
			logErr("inc_r16 received wrong opcode");
	}

	*reg++;

	return 2;
}

unsigned char Cpu::add_hl_r16(unsigned short opcode)
{
	// Opcode: [0x09, 0x19, 0x29, 0x39]
	// Symbol: ADDL
	// Operands: [(BC), (DE), (HL), (SP)]
	// Number of Bytes: 1
	// Number of Cycles: 2
	// Flags: - 0 16-bit 16-bit
	// Description :
	// Add the contents of register pair reg to the contents of register pair HL, and store the results in register pair HL.

	unsigned short* reg;
	switch (opcode)
	{
		case 0x09:
			reg = BC;
			break;
		case 0x19:
			reg = DE;
			break;
		case 0x29:
			reg = HL;
			break;
		case 0x39:
			reg = SP;
			break;
		default:
			logErr("add_hl_r16 received wrong opcode");
	}
	let a: u16 = cpu.get_r16(reg);
	let b: u16 = cpu.get_r16(Register16BitIdentifier::HL);
	let res = Op::Add(a, b).perform_with_flags(
			cpu,
			&[
			FlagOp::Nothing,
			FlagOp::Clear,
			FlagOp::CarryBorrow(11),
			FlagOp::Overflow,
			],
			);
	cpu.set_r16(Register16BitIdentifier::HL, res);

	return 2;
}

unsigned char Cpu::add_sp_s8(unsigned short opcode)
{
	// Opcode: [0xE8]
	// Symbol: ADDL
	// Operands: [(SP, S8)]
	// Number of Bytes: 2
	// Number of Cycles: 4
	// Flags: 0 0 16-bit 16-bit
	// Description
	// Add the contents of the 8-bit signed (2's complement) immediate operand s8 and the stack pointer SP and store the results in SP.

	/*
	   let a = cpu.read_signed_byte() as i32;
	   let b = cpu.get_sp() as i32;
	   let res: u16 = (a.wrapping_add(b)) as u16;

	   cpu.set_carry_flag(cpu.get_r16(Register16BitIdentifier::SP) > res);
	   cpu.set_operation_flag(false);
	   cpu.set_half_carry_flag(false);
	   cpu.set_zero_flag(res == 0);
	   return 4;
	   */

	let s8 = cpu.read_signed_byte();
	let sp = cpu.get_sp() as u16;
	let res = Op::Add(sp, s8 as u16).perform(cpu);
	// needed to be evaluated as u8
	Op::Add(s8 as u8, (sp & 0xFF) as u8).perform_with_flags(
			cpu,
			&[
			FlagOp::Clear,
			FlagOp::Clear,
			FlagOp::CarryBorrow(3),
			FlagOp::Overflow,
			],
			);
	cpu.set_sp(res);

	return 4;
}
