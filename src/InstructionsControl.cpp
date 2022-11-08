/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InstructionsControl.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 15:52:11 by nallani           #+#    #+#             */
/*   Updated: 2022/11/08 21:48:09 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cpu.hpp"

unsigned char Cpu::nop()
{
	return 1;
}

unsigned char Cpu::stop()
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
