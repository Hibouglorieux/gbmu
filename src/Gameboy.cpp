#include "Gameboy.hpp"

Mem Gameboy::gbMem = Mem();
Clock Gameboy::gbClock = Clock([](){Gameboy::LYCallback();});
int Gameboy::currentState = 0;

void	Gameboy::LYCallback()
{
	mem[0xFF44]++;
	if (mem[0xFF44] > 153) {
		// 144 line + V-BLANK (10 lines)
		mem[0xFF44] = 0;
	}
}

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
	return (Loop::loop());
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
