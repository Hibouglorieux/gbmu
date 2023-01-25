/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gameboy.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/08 19:04:02 by lmariott          #+#    #+#             */
/*   Updated: 2023/01/12 02:25:27 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GAMEBOY_CLASS_H
# define GAMEBOY_CLASS_H

/*
** Gameboy class, one for control all
** It load the binary into Mem.
** It update the different state of the gameboy
** It handle various handy thing: step-by-step, rendering control,
** save/load state
** It handle the hdma hblank
** It handle execution of the gameboy and call :
**	- Ppu for draw a line
**	- Cpu for execution next instruction and handle interruption
**	- Joypad for input
**	- Mem for access memory
**	- Clock for handle timing between line/frame
**	- ...
*/

#include "Mem.hpp"
#include "Clock.hpp"
#include "Cpu.hpp"
#include "Ppu.hpp"
#include "UserInterface.hpp"
#include "Screen.hpp"
#include "Joypad.hpp"
#include "define.hpp"
#include "Common.hpp"
#include <fstream>

// Theses are used by all class to access mem and clock
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

	static bool		quit;
	static Clock		gbClock;
	static uint8_t		internalLY;
	static int		clockLine;
	static float		clockRest;
	static bool		bIsCGB;
	static unsigned int	frameNb;

	static bool		bIsInit;
	static bool		bIsPathValid;

	static bool		lcdcWasOff;

	static bool		launchUserInterface();
	static Mem&		getMem();
	static Clock&		getClock();

	// Loading
	static void		init();
	static bool		loadRom();

	// Save state
	static void		saveState();
	static void		saveRam();
	static void		loadSaveState();

	// Status of execution
	static void		updateLY(int iter);
	static void		setState(int newState, bool bRefreshScreen);
	static int		getState();

	// Execution
	static bool		execFrame(Step step, bool bRefreshScreen = true);
	static int		executeLine(bool step, bool updateState,
					bool bRefreshScreen);

	// Events : Screen, Joypad
	static void		pollEvent();
	static void		clear();

	// Rendering ctrl
	static void		changeLCD(bool bActivateLCD);

	// Hdma H-BLANK
	static void 		doHblankHdma();
	static std::string	path;

private:
	static int		currentState;
	static bool		bLCDWasOff;
	static bool		bShouldRenderFrame;
	static Mem*		gbMem;

};

// Save State structure
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
		bool IME;
		bool setIMEFlag;
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
