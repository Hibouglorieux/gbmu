/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InstructionsMisc.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 21:48:16 by nallani           #+#    #+#             */
/*   Updated: 2022/11/08 21:49:36 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cpu.hpp"

unsigned char Cpu::daa()
{
	/*
    // Opcode: 0x27
    // Symbol: DAA
    // Number of Bytes: 1
    // Number of Cycles: 1
    // Flags: Z - 0 CY
    // Description
    // Adjust the accumulator (register A) to a binary-coded decimal (BCD) number after BCD addition and subtraction operations.
    // https://stackoverflow.com/questions/8119577/z80-daa-instruction
    // http://z80-heaven.wikidot.com/instructions-set:daa
    let a = cpu.get_r8(Register8BitIdentifier::A);
    //think about subtract being same than addition but on the other side
    let mut magic = if cpu.get_half_carry_flag() || (!cpu.get_operation_flag() && (a & 0xF) > 9) {0x6} else {0};

    if cpu.get_carry_flag() || (!cpu.get_operation_flag() && a > 0x99)
    {
        magic |= 0x60;
        cpu.set_carry_flag(true);
    }

    let res = if cpu.get_operation_flag() {Op::Sub(a , magic).perform(cpu)} else {Op::Add(a, magic).perform(cpu)};
    cpu.set_r8(Register8BitIdentifier::A, res);

    cpu.set_zero_flag(cpu.get_r8(Register8BitIdentifier::A) == 0);
    cpu.set_half_carry_flag(false);

	*/
    return 1;
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
