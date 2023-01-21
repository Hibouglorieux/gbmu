/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:52:08 by nallani           #+#    #+#             */
/*   Updated: 2023/01/07 19:43:21 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Gameboy.hpp"
#include <iostream>

#define DEFAULT_PATH_TO_FILE "./roms/42roms/Legend of Zelda, The - Oracle of Ages.gbc"

int main(int argc, char** argv)
{
	bool loadState = false;
	(void) argc;
	(void) argv;

	std::cout << std::setfill('0') << std::uppercase;
	std::cerr << std::setfill('0') << std::uppercase;
	std::cerr << std::hex;
	std::cout << std::hex;
	if (argc == 1 || argc > 3)
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
		if (argc == 3)
			loadState = true;
	}
	Gameboy::init();

	try {
		if (loadState) {
			Gameboy::loadSaveState(argv[2]);
		}
		Gameboy::launchUserInterface();
	}
	catch (const char* e) {
		std::cerr << e << std::endl;
	}
	// Gameboy::clear();
	return (0);
}
