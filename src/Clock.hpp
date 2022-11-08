/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clock.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 14:55:36 by nallani           #+#    #+#             */
/*   Updated: 2022/11/08 15:47:39 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CLOCK_CLASS_H
# define CLOCK_CLASS_H

class Clock {
public:
	Clock(void);
	virtual ~Clock(void);
	int operator+(int addValue);
	int operator-(int subValue);
	int& operator+=(int addValue);
	int& operator-=(int subValue);
	int& operator=(int value);
private:
	int _clock;
};

#endif
