/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clock.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 14:55:38 by nallani           #+#    #+#             */
/*   Updated: 2022/11/08 15:49:26 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Clock.hpp"

Clock::Clock()
{
	_clock = 0;
}

Clock::~Clock()
{
}

int	Clock::operator+(int addValue)
{
	return _clock + addValue;
}

int&	Clock::operator+=(int addValue)
{
	_clock += addValue;
	return _clock;
}

int	Clock::operator-(int subValue)
{
	return _clock - subValue;
}

int&	Clock::operator-=(int subValue)
{
	_clock -= subValue;
	return _clock;
}

int&	Clock::operator=(int value)
{
	_clock = value;
	return _clock;
}
