/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clock.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 14:55:36 by nallani           #+#    #+#             */
/*   Updated: 2022/12/09 01:34:41 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CLOCK_CLASS_H
# define CLOCK_CLASS_H

#include "functional"

//TODO add frequencies etc..
class Clock {
public:
	Clock();
	virtual ~Clock(void);
	int operator+(int addValue);
	int operator-(int subValue);
	int& operator+=(int addValue);
	int& operator-=(int subValue);
	int& operator=(int value);
	operator int();
private:
	int clock;
	unsigned int clockCount;
};

#endif
