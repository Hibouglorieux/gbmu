/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:52:08 by nallani           #+#    #+#             */
/*   Updated: 2023/01/05 17:25:58 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Gameboy.hpp"
#include <iostream>

#define DEFAULT_PATH_TO_FILE "./roms/42roms/Pokemon - Version Cristal.gbc"

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	std::cout << std::setfill('0') << std::uppercase;
	std::cerr << std::setfill('0') << std::uppercase;
	std::cerr << std::hex;
	std::cout << std::hex;
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
	Gameboy::init();
	try {
		Gameboy::run();
	} catch (const char* e){
		std::cerr << e << std::endl;
	}
	Gameboy::clear();
	return (0);
}
