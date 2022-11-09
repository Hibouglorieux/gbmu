/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:52:08 by nallani           #+#    #+#             */
/*   Updated: 2022/11/09 21:31:11 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cpu.hpp"
#include <iostream>
#define DEFAULT_PATH_TO_FILE "./roms/cpu_instrs/individual/01-special.gb"

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;
	if (argc < 2)
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
	Cpu::run();
	return (0);
}
