/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gameboy.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/08 19:04:02 by lmariott          #+#    #+#             */
/*   Updated: 2023/01/06 20:57:48 by nallani          ###   ########.fr       */
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
#include "Common.hpp"

#include <fstream>

#define g_clock (Gameboy::getClock())
#define mem (Gameboy::getMem())


class Gameboy {
public:
	enum class Step
	{
		full,
		oneInstruction,
		oneLine
	};

	static bool quit;
	static Clock gbClock;
	static uint8_t	internalLY;
	static int	clockLine;
	static bool bIsCGB;
	static bool bIsROM;

	static void init();
    static bool ProcessFile(char * filename);
	static bool loadRom(std::string pathToFile);
	static bool run();
	static Mem& getMem();
	static Clock& getClock();
	static void setState(int newState, bool bRefreshScreen);
	static bool execFrame(Step step, bool bRefreshScreen = true);
	static int getState();
	static void pollEvent();
	static void clear();
	static void saveRam();
	static void changeLCD(bool bActivateLCD);
	static unsigned int frameNb;
	static void saveState();
	static void loadSaveState(std::string path);
//	static void LYCallback();
private:
	static int currentState;
	static bool bLCDWasOff;
	static bool bShouldRenderFrame;
	static Mem* gbMem;
	static std::string path;

};

typedef struct {

	size_t romHash;

	struct {
		// std::string path;
		int currentState;

		bool quit;
		uint8_t	internalLY;
		int	clockLine;
		bool bIsCGB;
	} gameboy;

	struct {
		bool interrupts_master_enable;
		bool interrupts_flag;
		bool halted;
		uint32_t halt_counter;

		unsigned short PC;
		unsigned short SP;
		unsigned short registers[4];
	} cpu;

	struct {
		bool isValid;
	} memory;
	
	struct {
		int type;
		bool hasTimer;

		union mbc_type
		{
			struct {
				bool bEnableRam;
				bool bAdvancedBankingMode;
				unsigned char lowBitsRomBankNumber;
				unsigned char highBitsRomBankNumberOrRam;
			} mbc1;

			struct {
				bool bEnableRam;
				unsigned char romBankNb;
			} mbc2;

			struct {
				bool bEnableRam;
				time_t start;
				rtc rtc_register;
				unsigned char rtcBindNb;

				unsigned char romBankNb;
				unsigned char ramBankNb;
				unsigned char lastVal;
				bool latched;
			} mbc3;

			struct {
				bool bEnableRam;
				unsigned char leastSignificantRomByte;
				bool bit9;
				unsigned char ramBankNb;
			} mbc5;
		} bank;
	} mbc;

	struct {
		int val;
		bool reloadTMA;
		int timaClock;
		int divClock;
		bool cgbMode;
	} clock;

	struct {
		unsigned char windowCounter;
	} ppu;

	struct {
		bool bIsUsingCGBVram;
		unsigned char CGBextraRamBankNb;
		unsigned char BGPalettes[64];
		unsigned char OBJPalettes[64];
	} cgb;
} s_state;


#endif
