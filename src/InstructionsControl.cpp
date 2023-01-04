/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InstructionsControl.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 15:52:11 by nallani           #+#    #+#             */
/*   Updated: 2023/01/04 19:54:46 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cpu.hpp"

unsigned char Cpu::nop()
{
	return 1;
}

unsigned char Cpu::stop()
{
	Cpu::halted = true;
	Cpu::halt_counter = 1; // init halt counter
	mem[0xFF04] = 0; // reset DIV
	mem[0xFF4D] = 0x80; // request speed change lmariott TBD
//	Cpu::interrupts_master_enable = false;
	return 2;
}

unsigned char Cpu::halt()
{
	Cpu::halted = true;
	return 1;
}

unsigned char Cpu::di()
{
	Cpu::interrupts_master_enable = false;
    Cpu::interrupts_flag = false;
	return 1;
}

// The effect of EI is delayed by one instruction. This means that EI followed immediately by DI does not allow interrupts between the EI and the DI.
unsigned char Cpu::ei()
{
	// this is because if the IME is already working, there is no need to verify with a delay to enable it
	if (interrupts_master_enable != true)
		Cpu::interrupts_flag = true;
	return 1;
}
