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
	while (!Gameboy::quit)
	{
		/* Render clear */
		Screen::clear();
		Gameboy::setState(GBSTATE_V_BLANK);
		Cpu::request_interrupt(IT_VBLANK);
		Cpu::updateLY(10);
	        Debugger::start();
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
