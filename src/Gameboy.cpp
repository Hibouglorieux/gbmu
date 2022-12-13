#include "Gameboy.hpp"

#include <chrono>
#include <thread>
#include <iostream>
#include <utility>


Mem Gameboy::gbMem = Mem();
Clock Gameboy::gbClock = Clock();
int Gameboy::currentState = 0;
bool Gameboy::quit = false;

Mem& Gameboy::getMem()
{
	return (gbMem);
}

Clock& Gameboy::getClock()
{
	return (gbClock);
}

void	Gameboy::init()
{
    Gameboy::quit = false;
	Screen::create();
	Cpu::loadBootRom();
}

bool Gameboy::loadRom(std::string pathToFile)
{
	gbMem = Mem(std::move(pathToFile));
	return gbMem.isValid;
}

bool Gameboy::run()
{

    Gameboy::quit = false;
    while (!Gameboy::quit) {
        // SDL_GetError();
        // std::cout << Gameboy::gbClock << std::endl;
	/* Render clear */
		Screen::clear();
		Gameboy::setState(GBSTATE_V_BLANK);
		Cpu::request_interrupts(VBLANK_INT_BIT);
		Cpu::updateLY(10);
        Cpu::run();
        // Cpu::handle_timer();
//		cycle = (Cpu::executeClock(1140 - cycle) - (1140 - cycle)); // V-BLANK first as LY=0x90 at start
	    Ppu::run();
        Cpu::handle_interrupts();
		Screen::drawVRam();
		Screen::drawBG();
		/* Manage events */
		Gameboy::pollEvent();
		/* Render present */
		Screen::update();
		/* Sleep : TODO calculate compute time to have a frame rate ~60fps*/
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return true ; //no need to return ??
}

void Gameboy::setState(int newState)
{
	currentState = newState;
	mem[0xFF41] &= ~GBSTATE_MSK;
	mem[0xFF41] |= newState;
}

int Gameboy::getState()
{
	return (currentState);
}

void Gameboy::pollEvent()
{
	SDL_Event ev;

	for (int i = 0 ; i < 8 ; i++) {
		SDL_PollEvent(&ev);
		Screen::handleEvent(&ev);
		Joypad::handleEvent(&ev);
	}
}
