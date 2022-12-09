/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 22:44:23 by lmariott          #+#    #+#             */
/*   Updated: 2022/12/09 01:58:33 by lmariott         ###   ########.fr       */
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
	std::array<int, NB_LINES> finalLine;
	int clockDiff = 0;

	while (1)
	{
		Gameboy::setState(GBSTATE_V_BLANK);
		Cpu::updateLY(10);
		clockDiff = (Cpu::executeClock(1140 - clockDiff) - (1140 - clockDiff)); // V-BLANK first as LY=0x90 at start
		for (int i = 0 ; i < 144 ; i++) {
			finalLine = Ppu::doOneLine();
			Gameboy::setState(GBSTATE_OAM_SEARCH);
			clockDiff = (Cpu::executeClock(20 - clockDiff) - (20 - clockDiff));
			Gameboy::setState(GBSTATE_PX_TRANSFERT);
			clockDiff = (Cpu::executeClock(43 - clockDiff) - (43 - clockDiff));
			Gameboy::setState(GBSTATE_H_BLANK);
			clockDiff = (Cpu::executeClock(51 - clockDiff) - (51 - clockDiff));
			Cpu::updateLY(1);
			for (int j = 0 ; j < 160 ; j++) {
				Screen::drawPoint(j, i, finalLine[j]);
			}
		}
		Screen::update();
		Screen::updateVRam();
		Screen::updateBG();
		Gameboy::pollEvent();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
	}
	return (true);
}
