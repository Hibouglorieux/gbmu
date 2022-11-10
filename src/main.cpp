/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:52:08 by nallani           #+#    #+#             */
/*   Updated: 2022/11/10 18:24:27 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cpu.hpp"
#include <iostream>
#define DEFAULT_PATH_TO_FILE "./roms/cpu_instrs/individual/04-op r,imm.gb"

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;
	if (argc == 1 || argc == 3)
	{
		if (!Cpu::loadRom(DEFAULT_PATH_TO_FILE))
		{
			return -1;
		}
	}
	else
	{
		if (!Cpu::loadRom(argv[1]))
		{
			return -1;
		}
	}
	std::cout << std::setfill('0') << std::uppercase;
	std::cerr << std::setfill('0') << std::uppercase;
	std::cerr << std::hex;
	std::cout << std::hex;
	Cpu::loadBootRom();
	if (argc == 3)
		Cpu::run(std::stoi(argv[1]), std::stoi(argv[2]));
	else
		Cpu::run();
	return (0);
}
