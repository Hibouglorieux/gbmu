/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clock.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 14:55:38 by nallani           #+#    #+#             */
/*   Updated: 2022/12/09 05:35:22 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Clock.hpp"
#include "Gameboy.hpp"
#include "define.hpp"

bool Clock::cgbMode = false;
int Clock::timaClock = 0;
int Clock::divClock = 0;

Clock::Clock()
{
	/* Init DIV at 0 */
	Mem::write_u8(DIV, 0); //   mem[DIV] = 0;
	/* Init TIMA at 0 */
	Mem::write_u8(DIV, 0); //   mem[TIMA] = 0;
	clock = 0;

	// TODO LMA whats this ?
	// clock = 32916 / 2;
}

Clock::~Clock()
{
}



int&	Clock::operator+=(int addValue)
{
	int timaFreqDivider = 1024;

	clock += addValue;

	/* Handle DIV */
	divClock += addValue;
    Mem::write_u8(DIV, Mem::read_u8(DIV) + divClock / 256); //   mem[DIV] = 0;
	divClock %= 256;

	/* Handle TIMA */
    uint8_t tac = Mem::read_u8(TAC);
	if ((tac & (1 << 2)) != 0) {
		/* Calculate TIMA frequency divider TODO CGB Mode is not handled */
		if ((tac & 0x3) == 1) {
			timaFreqDivider = 16;
		} else if ((tac & 0x3) == 2) {
			timaFreqDivider = 64;
		} else if ((tac & 0x3) == 3) {
			timaFreqDivider = 256;
		}
		timaClock += addValue;
        int tima = (Mem::read_u8(TIMA));
		if (tima + timaClock / timaFreqDivider > 0xFF) {
			/* Request Interrupt */
			Cpu:: request_interrupts(TIMER_INT_BIT); // mem[0xFF0F] |= (1 << 2); // TODO request interrupts fct
			/* Reset TIMA to TMA and add the rest */
            Mem::write_u8(TIMA, Mem::read_u8(TMA) + (256 -  Mem::read_u8(TIMA) + timaClock / timaFreqDivider)); //			M_TIMA = (int)M_TMA + (256 - ((int)mem[0xFF05] + timaClock / timaFreqDivider));
			timaClock %= timaFreqDivider;
		}
		else {
            Mem::write_u8(TIMA, tima + timaClock / timaFreqDivider); //			M_TIMA += timaClock / timaFreqDivider;
			timaClock %= timaFreqDivider;
		}
	}
	if (clock >= 17556) {
		// 144 line + V-BLANK
		// We reset the counter here, we dont need to do it at this value but why not
		clock = clock - 17556;
	}
	return clock;
}

Clock::operator int()
{
	return clock;
}
