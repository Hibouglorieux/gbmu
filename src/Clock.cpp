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

bool Clock::cgbMode = false;
int Clock::timaClock = 0;
int Clock::divClock = 0;

Clock::Clock()
{
	/* Init DIV at 0 */
	mem[0xFF04] = 0;
	/* Init TIMA at 0 */
	mem[0xFF05] = 0;
	clock = 0;

	// TODO LMA whats this ?
	// clock = 32916 / 2;
}

Clock::~Clock()
{
}

#define M_DIV	mem[0xFF04]
#define M_TIMA	mem[0xFF05]
#define M_TMA	mem[0xFF06]
#define M_TAC	mem[0xFF07]

// NOTE : addValue is in cycle in this context
int&	Clock::operator+=(int addValue)
{
	static int clocks_array[4] = {1024, 16, 64, 256};
	int timaFreqDivider = 0;

	clock += addValue * 4;

	/* Handle DIV */
	divClock += addValue * 4;
	M_DIV += divClock / 256;
	divClock %= 256;

	/* Handle TIMA */
	if ((M_TAC & (1 << 2)) != 0) {
		/* Calculate TIMA frequency divider TODO CGB Mode is not handled */
		timaFreqDivider = clocks_array[M_TAC & 3];
		timaClock += addValue * 4;
		if ((int)M_TIMA + timaClock / timaFreqDivider > 0xFF) {
			/* Request Interrupt */
			mem[0xFF0F] |= (1 << 2);
			/* Reset TIMA to TMA and add the rest */
			M_TIMA = (uint8_t)((int)M_TMA + (256 - ((int)mem[0xFF05] + timaClock / timaFreqDivider)));
			timaClock %= timaFreqDivider;
		}
		else {
			M_TIMA += timaClock / timaFreqDivider;
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
