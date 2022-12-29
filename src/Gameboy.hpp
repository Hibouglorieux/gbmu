/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gameboy.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/08 19:04:02 by lmariott          #+#    #+#             */
/*   Updated: 2022/12/08 22:38:23 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GAMEBOY_CLASS_H
# define GAMEBOY_CLASS_H

#include "Mem.hpp"
#include "Clock.hpp"
#include "Cpu.hpp"
#include "Ppu.hpp"
#include "Loop.hpp"
#include "Screen.hpp"
#include "Joypad.hpp"
#include "define.hpp"

#define g_clock (Gameboy::getClock())
#define mem (Gameboy::getMem())

class Gameboy {
public:
	static bool quit;
	static Mem gbMem;
	static Clock gbClock;
	static void init();
	static bool loadRom(std::string pathToFile);
	static bool run();
	static Mem& getMem();
	static Clock& getClock();
	static void setState(int newState);
	static int getState();
	static void pollEvent();
//	static void LYCallback();
private:
	static int currentState;

};
#endif
