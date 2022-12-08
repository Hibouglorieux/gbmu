/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clock.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 14:55:38 by nallani           #+#    #+#             */
/*   Updated: 2022/12/08 22:29:40 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Clock.hpp"

Clock::Clock(std::function<void()> f)
{
	//TODO change that, after boot rom state
	LYCallback = f;
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
	//TODO do it properly
	unsigned int tmp = clock / 114;

	if (tmp > clockCount)
	{
		clockCount += 1;
		LYCallback();
	}
	if (clock >= 17556) // 144 line + V-BLANK
	{
		clock = clock - 17556;
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

Clock::operator int()
{
	return clock;
}
