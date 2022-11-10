/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clock.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 14:55:38 by nallani           #+#    #+#             */
/*   Updated: 2022/11/10 15:52:16 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Clock.hpp"

Clock::Clock(Mem& cpuMem) : mem(cpuMem)
{
	//TODO change that, after boot rom state
	clock = 32916 / 2;
	clockCount = clock / 114;
}

Clock::~Clock()
{
}

int	Clock::operator+(int addValue)
{
	return clock + addValue;
}

int&	Clock::operator+=(int addValue)
{
	clock += addValue;
	//TODO clear that properly
	unsigned int tmp = clock / 114;

	if (tmp > clockCount)
	{
		clockCount += 1;
		mem[0xFF44]++;
		if (mem[0xFF44] > 153)
			mem[0xFF44] = 0;
	}
	if (clock >= 70224 / 4)
	{
		clock = 0;
		clockCount = 0;
	}
	return clock;
}

int	Clock::operator-(int subValue)
{
	return clock - subValue;
}

int&	Clock::operator-=(int subValue)
{
	clock -= subValue;
	return clock;
}

int&	Clock::operator=(int value)
{
	clock = value;
	return clock;
}
