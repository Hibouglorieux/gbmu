/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clock.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 14:55:36 by nallani           #+#    #+#             */
/*   Updated: 2022/11/10 15:52:38 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CLOCK_CLASS_H
# define CLOCK_CLASS_H
#include "Mem.hpp"

//TODO add frequencies etc..
class Clock {
public:
	Clock(Mem& cpuMem);
	virtual ~Clock(void);
	int operator+(int addValue);
	int operator-(int subValue);
	int& operator+=(int addValue);
	int& operator-=(int subValue);
	int& operator=(int value);
private:
	int clock;
	unsigned int clockCount;
	Mem& mem;
};

#endif
