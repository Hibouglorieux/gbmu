/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hdma.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/06 01:22:58 by nathan            #+#    #+#             */
/*   Updated: 2023/01/12 02:02:01 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef HDMA_CLASS_H
# define HDMA_CLASS_H
#include <cstdint>

class Hdma {
public:
	static void writeInHdma(uint16_t dstAddr, uint16_t srcAddr, uint8_t newValue);
	static int update();
	static int updateHBlank();
private:
	static uint16_t src;
	static uint16_t dst;
	static uint16_t len;
	static bool bIsWritting;
	static bool bIsInHBlankMode;
	static bool bJustStarted;
};

#endif
