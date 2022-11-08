/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FlagOp.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 16:25:02 by nallani           #+#    #+#             */
/*   Updated: 2022/11/08 17:32:48 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cpu.hpp"

bool Cpu::getHalfCarry8Bit(unsigned char a, unsigned char b)
{
	a &= 0xF;
	b &= 0xF;
	return (a + b) > 0xF;
}

bool Cpu::getHalfCarry16Bit(unsigned short a, unsigned short b)
{
	a &= 0xFFF;
	b &= 0xFFF;
	return (a + b) > 0xFFF;
}

/*
bool Cpu::getHalfBorrow8Bit(unsigned char a, unsigned char b)
{

}
*/
