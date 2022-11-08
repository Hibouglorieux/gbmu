/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FlagOp.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 16:25:02 by nallani           #+#    #+#             */
/*   Updated: 2022/11/08 16:53:04 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cpu.hpp"

template <typename T>
T	getFlagResult(T a, T b, FlagOp operation)
{
	bool flagVal;
	switch (operation)
	{
		case FlagOp::Reset:
			return 0;
		case FlagOp::Set:
			return 1;
		default:
			exit(-2);
	}
	return flagVal;
}

template <typename T>
T	perform(T a, T b, FlagOp zero, FlagOp subtract, FlagOp halfCarry, FlagOp carry)
{
	setZeroFlag(getFlagResult(a, b, zero));
	setSubtractFlag(getFlagResult(a, b, subtract));
	setHalfCarryFlag(getFlagResult(a, b, haldCarry));
	setCarryFlag(getFlagResult(a, b, carry));
}
