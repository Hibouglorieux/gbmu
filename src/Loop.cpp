/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 22:44:23 by lmariott          #+#    #+#             */
/*   Updated: 2022/12/19 17:37:49 by nallani          ###   ########.fr       */
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

bool Loop::loop()
{
	std::array<int, PIXEL_PER_LINE> finalLine;
	int clockDiff = 0;

	while (true)
	{
		/* Render clear */
		Screen::clear();
		Gameboy::setState(GBSTATE_V_BLANK);
		for (int i = 0 ; i < 10 ; i++) {
			clockDiff = (Cpu::executeClock(114 - clockDiff) - (114 - clockDiff)); // V-BLANK first as LY=0x90 at start
			Cpu::updateLY(1);
		}
		for (int i = 0 ; i < 144 ; i++) {
			Gameboy::setState(GBSTATE_OAM_SEARCH);
			clockDiff = (Cpu::executeClock(20 - clockDiff) - (20 - clockDiff));
			finalLine = Ppu::doOneLine();
			Gameboy::setState(GBSTATE_PX_TRANSFERT);
			clockDiff = (Cpu::executeClock(43 - clockDiff) - (43 - clockDiff));
			Gameboy::setState(GBSTATE_H_BLANK);
			clockDiff = (Cpu::executeClock(51 - clockDiff) - (51 - clockDiff));
			Cpu::updateLY(1);
			/* Drawing time */
			for (int j = 0 ; j < PIXEL_PER_LINE ; j++) {
				Screen::drawPoint(j, i, finalLine[j]);
			}
		}
		Ppu::resetWindowCounter();
		Screen::drawVRam();
		Screen::drawBG();
		/* Manage events */
		Gameboy::pollEvent();
		/* Render present */
		Screen::update();
		/* Sleep : TODO calculate compute time to have a frame rate ~60fps*/
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
		//Cpu::printFIFO(Cpu::fifo);	
	}
	return (true);
}
