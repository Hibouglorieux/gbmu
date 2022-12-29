#include "Gameboy.hpp"
#include "imgui/imgui_impl_sdl.h"

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

	if (BIT(M_LCDC, 7)) {
		if (newState == GBSTATE_V_BLANK) {
			if (BIT(M_LCDC_STATUS, 4)) {
				Cpu::request_interrupt(IT_LCD_STAT);
			}
			Cpu::request_interrupt(IT_VBLANK);
			if (BIT(M_LCDC, 7)) {
				M_LY = 0x90;
			}
		}
		if (newState == GBSTATE_H_BLANK && BIT(M_LCDC_STATUS, 3)) {
			Cpu::request_interrupt(IT_LCD_STAT);
		}
		if (newState == GBSTATE_OAM_SEARCH && BIT(M_LCDC_STATUS, 5)) {
			Cpu::request_interrupt(IT_LCD_STAT);
		}
		unsigned char lcdcs = M_LCDC_STATUS & ~0x07;
		lcdcs |= newState;
		mem.supervisorWrite(LCDC_STATUS, lcdcs);
	}
}

int Gameboy::getState()
{
	return (currentState);
}

void Gameboy::pollEvent()
{
    SDL_Event event;
	while (SDL_PollEvent(&event)) {
		ImGui_ImplSDL2_ProcessEvent(&event);
        Screen::handleEvent(&event);
		Joypad::handleEvent(&event);
		if (event.type == SDL_QUIT)
			Gameboy::quit = true;
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
				event.window.windowID == SDL_GetWindowID(Screen::window))
			Gameboy::quit = true;
	}
}
