/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CpuStackTrace.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/20 16:33:15 by nallani           #+#    #+#             */
/*   Updated: 2022/12/20 20:13:12 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CpuStackTrace.hpp"
#include <iostream>
#include <iomanip>

CpuStackTrace::CpuStackTrace()
{
	printSize = 30;
	maxSize = 10;
	PCBreak = 0;
	opcodeBreak = 0xFD;
	breakActive = false;
}

CpuStackTrace::~CpuStackTrace()
{
}

void CpuStackTrace::print()
{
	for (int i = queue.size() - 1; i >= 0; i--)
	{
		queue[i].print();
		std::cout << std::endl;
	}
}

void CpuStackTrace::add(StackData stackData)
{
	queue.insert(queue.begin(), stackData);
	if (queue.size() > maxSize)
		queue.pop_back();
	if (breakActive && (stackData.PC == PCBreak || stackData.opcode == opcodeBreak || queue.size() > printSize))
	{
		print();
		exit(0);
	}
}

StackData::StackData()
{
}

void StackData::print()
{
	std::cout << std::setfill('0');
	std::cout << "PC: 0x" << std::setw(4) << PC << "\topcode: 0x" << std::setw(4) <<  opcode << std::endl;
	std::cout << "SP: 0x" << std::setw(4) << SP << "\tAF: 0x" << std::setw(4) << AF << std::endl;
	std::cout << "BC: 0x" << std::setw(4) << BC << "\tDE: 0x" << std::setw(4) << DE << std::endl;
	if (!customData.empty())
		std::cout << customData << std::endl;
}
