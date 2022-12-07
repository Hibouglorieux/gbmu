/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 22:44:23 by lmariott          #+#    #+#             */
/*   Updated: 2022/12/07 06:12:48 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Loop.hpp"
#include "Cpu.hpp"
#include "Screen.hpp"
#include "Joypad.hpp"

#include <chrono>
#include <thread>
#include <iostream>

void Loop::loop()
{
	SDL_Event ev;
	int c = 0;

	while (1)
	{
		for (int i = 0 ; i < 144 ; i++) {
			for (int j = 0 ; j < 160 ; j++) {
				Screen::drawPoint(j, i, c++, i, j);
			}
		}
		std::cout << "input=" << (int)Joypad::get() << std::endl;
		Screen::update();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		for (int i = 0 ; i < 8 ; i++) {
			SDL_PollEvent(&ev);
			Screen::handleEvent(&ev);
			Joypad::handleEvent(&ev);
		}
	}
}
