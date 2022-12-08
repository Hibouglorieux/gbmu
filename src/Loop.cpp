/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 22:44:23 by lmariott          #+#    #+#             */
/*   Updated: 2022/12/08 04:55:38 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Loop.hpp"
#include "Cpu.hpp"
#include "Ppu.hpp"
#include "Screen.hpp"
#include "Joypad.hpp"

#include <chrono>
#include <thread>
#include <iostream>

void Loop::loop()
{
	SDL_Event ev;
	std::array<int, NB_LINES> finalLine;
	int clockDiff = 0;

	while (1)
	{
		clockDiff = (Cpu::executeClock(1140 - clockDiff) - (1140 - clockDiff)); // V-BLANK first as LY=0x90 at start
		for (int i = 0 ; i < 144 ; i++) {
			finalLine = Ppu::doOneLine();
			clockDiff = (Cpu::executeClock(114 - clockDiff) - (114 - clockDiff));
			for (int j = 0 ; j < 160 ; j++) {
				Screen::drawPoint(j, i, finalLine[j]);
			}
		}
		Screen::update();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		for (int i = 0 ; i < 8 ; i++) {
			SDL_PollEvent(&ev);
			Screen::handleEvent(&ev);
			Joypad::handleEvent(&ev);
		}
	}
}
