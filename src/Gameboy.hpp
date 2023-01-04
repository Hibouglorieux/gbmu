/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gameboy.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/08 19:04:02 by lmariott          #+#    #+#             */
/*   Updated: 2023/01/03 17:37:48 by nallani          ###   ########.fr       */
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

#include <fstream>

#define g_clock (Gameboy::getClock())
#define mem (Gameboy::getMem())

class Gameboy {
public:
	static bool quit;
	static Clock gbClock;
	static int	internalLY;
	static int	clockLine;
	static void init();
	static bool loadRom(std::string pathToFile);
	static bool run();
	static Mem& getMem();
	static Clock& getClock();
	static void setState(int newState, bool bRefreshScreen);
	static bool execFrame(bool step, bool bRefreshScreen = true);
	static int getState();
	static void pollEvent();
	static bool bIsCGB;
	static void clear();
	static void saveRam();
//	static void LYCallback();
private:
	static int currentState;
	static Mem* gbMem;
	static std::string path;

};
#endif
