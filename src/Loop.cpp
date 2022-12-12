/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 22:44:23 by lmariott          #+#    #+#             */
/*   Updated: 2022/12/09 02:44:07 by lmariott         ###   ########.fr       */
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
	int clockDiff = 0;

	while (true)
	{
		/* Render clear */
		Screen::clear();
		Gameboy::setState(GBSTATE_V_BLANK);
		Cpu::updateLY(10);
		clockDiff = (Cpu::executeClock(1140 - clockDiff) - (1140 - clockDiff)); // V-BLANK first as LY=0x90 at start
	    Ppu::run(clockDiff);
		Screen::drawVRam();
		Screen::drawBG();
		/* Manage events */
		Gameboy::pollEvent();
		/* Render present */
		Screen::update();
		/* Sleep : TODO calculate compute time to have a frame rate ~60fps*/
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
	}
	return (true);
}
