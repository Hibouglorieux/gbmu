/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mem.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:49:00 by nallani           #+#    #+#             */
/*   Updated: 2022/11/07 21:07:23 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mem.hpp"

Mem::Mem()
{
	internalArray = new unsigned char[16 * 1024 * 1024];
}

Mem::Mem(int size)
{
	internalArray = new unsigned char[size];
}

Mem::~Mem()
{
	delete[] internalArray;
}

unsigned char& Mem::operator[](int i)
{
	return internalArray[i];
}
