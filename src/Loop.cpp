/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 22:44:23 by lmariott          #+#    #+#             */
/*   Updated: 2022/12/24 04:27:43 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Loop.hpp"
#include "../includes/Cpu.hpp"
#include "../includes/Screen.hpp"

#include "../includes/Ppu.hpp"

#include "../includes/Debugger.hpp"
//#include "../includes/define.hpp"

#include <chrono>
#include <thread>
#include <iostream>

bool Loop::loop()
{
    int clockdiff = 0;
	// TODO unsure about updateScreen ? Do we update everytime ?
	bool updateScreen = true;
	int clockDiff = 0;
	while (!Gameboy::quit)
	{
		/* Render clear */
		if (updateScreen) {
			Screen::clear();
		}
		Gameboy::setState(GBSTATE_V_BLANK);
		Cpu::request_interrupt(IT_VBLANK);
		Cpu::updateLY(10);
        updateScreen = Debugger::start(clockdiff, updateScreen);
		/* Manage events */
		Gameboy::pollEvent();
		/* Render present */
//		Screen::update();
		for (int i = 0 ; i < 10 ; i++) {
			clockDiff = (Cpu::executeClock(114 - clockDiff) - (114 - clockDiff)); // V-BLANK first as LY=0x90 at start
			Cpu::updateLY(1);
		}
		if (BIT(M_LCDC, 7)) {
			M_LY = 0x00;
		}
		if (updateScreen) {
			Ppu::resetWindowCounter();
		}
		/* Manage events */
		Gameboy::pollEvent();
//		/* Render present */
//		if (updateScreen) {
//			Screen::update();
//		}
		/* Sleep : TODO calculate compute time to have a frame rate ~60fps*/
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
		//Cpu::printFIFO(Cpu::fifo);	
	}
	return (true);
}
