/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 22:44:23 by lmariott          #+#    #+#             */
/*   Updated: 2022/12/26 15:22:37 by nallani          ###   ########.fr       */
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
	const std::chrono::microseconds frameTime(1'000'000 / 60);
	// TODO unsure about updateScreen ? Do we update everytime ?
	bool updateScreen = 1;
	std::array<int, PIXEL_PER_LINE> finalLine;
	int clockDiff = 0;

	Screen::createTexture();
	while (true)
	{
		auto beginFrameTime = std::chrono::system_clock::now();
		/* Render clear */
		// if (updateScreen) {
		// 	Screen::clear();
		// }

		if (SDL_LockTexture(Screen::texture, NULL, &Screen::pixels, &Screen::pitch)) {
			throw "Could not lock texture\n";
		}


		Gameboy::setState(GBSTATE_V_BLANK);
		for (int i = 0 ; i < 10 ; i++) {
			clockDiff = (Cpu::executeClock(114 - clockDiff) - (114 - clockDiff)); // V-BLANK first as LY=0x90 at start
			Cpu::updateLY(1);
		}
		if (BIT(M_LCDC, 7)) {
			M_LY = 0x00;
		}
		for (int i = 0 ; i < 144 ; i++) {
			Gameboy::setState(GBSTATE_OAM_SEARCH);
			clockDiff = (Cpu::executeClock(20 - clockDiff) - (20 - clockDiff));
			if (BIT(M_LCDC, 7)) {
				finalLine = Ppu::doOneLine();
				for (int j = 0 ; BIT(M_LCDC, 7) && j < PIXEL_PER_LINE ; j++) {
					Screen::drawPoint(j, i, finalLine[j]);
				}
				updateScreen = 1;
			}
			else {
				updateScreen = 0;
			}
			Gameboy::setState(GBSTATE_PX_TRANSFERT);
			clockDiff = (Cpu::executeClock(43 - clockDiff) - (43 - clockDiff));
			Gameboy::setState(GBSTATE_H_BLANK);
			clockDiff = (Cpu::executeClock(51 - clockDiff) - (51 - clockDiff));
			Cpu::updateLY(1);
			/* Drawing time */
		}
		if (updateScreen) {
			Ppu::resetWindowCounter();
			/*
			printf("VRAM");
			for (int i = 0 ; i < 0x200 ; i++) {
				if (i % 32 == 0) {
					printf("\n%04x: ", 0x8000 + i);
				}
				printf("%02x ", (uint8_t)mem[0x8000 + i]);
			}
			*/
			// Screen::drawVRam();
			// Screen::drawBG();
		}
		/* Manage events */
		Gameboy::pollEvent();
		/* Render present */
		if (updateScreen) {
			Screen::update();
		}
		std::chrono::microseconds timeTakenForFrame = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - beginFrameTime);

		/* Sleep : TODO calculate compute time to have a frame rate ~60fps*/
		if (timeTakenForFrame.count() < frameTime.count())
		{
			//std::cout << "sleeping for: " << std::dec << (frameTime - timeTakenForFrame).count() << std::hex << " microseconds" << std::endl;
			std::this_thread::sleep_for(frameTime - timeTakenForFrame);
		}
		else
		{
			//std::cout << "no need for sleep because frame took: " << std::dec << (timeTakenForFrame).count() << std::hex << " microseconds" << std::endl;
		}
	}
	return (true);
}
