/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FlagOp.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 16:25:03 by nallani           #+#    #+#             */
/*   Updated: 2022/11/08 16:55:58 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FLAGOP_CLASS_H
# define FLAGOP_CLASS_H

enum FlagOp {
	Reset = 0,
	Set = 1,
	Nothing,
	IsZero,
	8BitOverflow,
	16BitOverflow,
	8BitHalfCarry,
	8BitHalfBorrow,
	16BitHalfCarry,
	16BitHalfBorrow,
};

template <typename T>
T	setFlags(T a, T b, FlagOp zero, FlagOp subtract, FlagOp halfCarry, FlagOp carry);

/*
template <typename T>
T	setFlags(T a, T b, T c, FlagOp zero, FlagOp subtract, FlagOp halfCarry, FlagOp carry);
*/

#endif
