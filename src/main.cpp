/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:52:08 by nallani           #+#    #+#             */
/*   Updated: 2022/12/08 20:39:15 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Gameboy.hpp"
#include <iostream>
#define DEFAULT_PATH_TO_FILE "./roms/cpu_instrs/individual/11-op a,(hl).gb"
// #define DEFAULT_PATH_TO_FILE "./tetris.gb"

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	if (argc == 1 || argc == 3)
	{
		if (!Gameboy::loadRom(DEFAULT_PATH_TO_FILE))
		{
			return -1;
		}
	}
	else
	{
		if (!Gameboy::loadRom(argv[1]))
		{
			return -1;
		}
	}
	std::cout << std::setfill('0') << std::uppercase;
	std::cerr << std::setfill('0') << std::uppercase;
	std::cerr << std::hex;
	std::cout << std::hex;
	Gameboy::init();
	Gameboy::run();
	return (0);
}
