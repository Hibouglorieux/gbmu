/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InstructionsControl.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 15:52:11 by nallani           #+#    #+#             */
/*   Updated: 2023/01/03 22:45:53 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cpu.hpp"

unsigned char Cpu::nop()
{
	return 1;
}

unsigned char Cpu::stop()
{
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
    Cpu::interrupts_flag = true;
	return 1;
}
