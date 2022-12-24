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

#include "Loop.hpp"
#include "Cpu.hpp"
#include "Ppu.hpp"
#include "Screen.hpp"
#include "Joypad.hpp"

#include <chrono>
#include <thread>
#include <iostream>

#define DELAY_TIME (1000.0f/60.0f)
bool Loop::loop()
{
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    // TODO unsure about updateScreen ? Do we update everytime ?
	bool updateScreen = true;
	int clockDiff = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> current, previous, t1,t2;
	previous = std::chrono::high_resolution_clock::now();


	while (true)
	{
        current = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>> (current - previous);
		previous = current;
		/* Render clear */
        t1 = high_resolution_clock::now();
		if (updateScreen) {
			Screen::clear();
		}
		Gameboy::setState(GBSTATE_V_BLANK);
		for (int i = 0 ; i < 10 ; i++) {
			clockDiff = (Cpu::executeClock(114 - clockDiff) - (114 - clockDiff)); // V-BLANK first as LY=0x90 at start
			Cpu::updateLY(1);
		}
		if (BIT(M_LCDC, 7)) {
			M_LY = 0x00;
            for (int i = 0 ; i < 144 ; i++) {
                Gameboy::setState(GBSTATE_OAM_SEARCH);
                clockDiff = (Cpu::executeClock(20 - clockDiff) - (20 - clockDiff));
                Ppu::finalLine = Ppu::doOneLine();
                for (int j = 0 ; BIT(M_LCDC, 7) && j < PIXEL_PER_LINE ; j++) {
                    Screen::drawPoint(j, i, Ppu::finalLine[j]);
                }
                updateScreen = true;
                Gameboy::setState(GBSTATE_PX_TRANSFERT);
                clockDiff = (Cpu::executeClock(43 - clockDiff) - (43 - clockDiff));
                Gameboy::setState(GBSTATE_H_BLANK);
                clockDiff = (Cpu::executeClock(51 - clockDiff) - (51 - clockDiff));
                Cpu::updateLY(1);
            }
        } else {
		    updateScreen = false;
        }
		if (updateScreen) {
			Ppu::resetWindowCounter();
			Screen::drawVRam();
//			Screen::drawBG();
            Screen::update();
		}
        t2 = high_resolution_clock::now();
		Gameboy::pollEvent();

        /* Getting number of milliseconds as a double. */
        duration<double, std::milli> ms_double = t2 - t1;
        std::cout << ms_double.count() << "ms\n";

		/* Sleep : TODO calculate compute time to have a frame rate ~60fps*/
        if (elapsed.count() < DELAY_TIME) {
            std::cout <<"sleep for "<< std::float_t() <<  DELAY_TIME - elapsed.count() << std::endl;
			std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(DELAY_TIME - elapsed.count()));
		} else{
			std::cout <<"overpassed by "<< std::float_t() <<  elapsed.count() - DELAY_TIME << std::endl;
		}
	}
	return (true);
}
