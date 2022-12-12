#include "Gameboy.hpp"

#include <chrono>
#include <thread>
#include <iostream>


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
	gbMem = Mem(pathToFile);
	return gbMem.isValid;
}

bool Gameboy::run()
{
    unsigned int cycle = 0;
    unsigned int cpu_cycle = 0;

    Gameboy::quit = false;
    while (!Gameboy::quit) {
	/* Render clear */
		Screen::clear();
//		Gameboy::setState(GBSTATE_V_BLANK);
//		Cpu::updateLY(10);
        cpu_cycle = Cpu::run();
//		cycle = (Cpu::executeClock(1140 - cycle) - (1140 - cycle)); // V-BLANK first as LY=0x90 at start
	    Ppu::run(cpu_cycle);
        Cpu::handle_interrupts();
        Cpu::handle_timer(cpu_cycle);
		Screen::drawVRam();
		Screen::drawBG();
		/* Manage events */
		Gameboy::pollEvent();
		/* Render present */
		Screen::update();
        cycle += cpu_cycle;
		/* Sleep : TODO calculate compute time to have a frame rate ~60fps*/
//		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
    }
    return true ; //no need to return ??
}



//void Gameboy::setState(int newState)
//{
//	currentState = newState;
//	mem[0xFF41] &= ~GBSTATE_MSK;
//	mem[0xFF41] |= newState;
//}


//int Gameboy::getState()
//{
//	return (currentState);
//}

void Gameboy::pollEvent()
{
	SDL_Event ev;

	for (int i = 0 ; i < 8 ; i++) {
		SDL_PollEvent(&ev);
		Screen::handleEvent(&ev);
		Joypad::handleEvent(&ev);
	}
}
