/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:52:08 by nallani           #+#    #+#             */
/*   Updated: 2022/12/07 06:12:34 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cpu.hpp"
#include "Loop.hpp"
#include "Screen.hpp"
#include <iostream>
#define DEFAULT_PATH_TO_FILE "./roms/cpu_instrs/individual/11-op a,(hl).gb"

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

//	Screen::create();

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
	// Loop::loop();
	return (0);
}
