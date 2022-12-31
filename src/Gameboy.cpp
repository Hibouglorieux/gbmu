#include "Gameboy.hpp"
#include "imgui/imgui_impl_sdl.h"

Mem* Gameboy::gbMem = nullptr;
Clock Gameboy::gbClock = Clock();
int Gameboy::currentState = 0;

int Gameboy::internalLY = 0;
int Gameboy::clockLine = 0;

bool Gameboy::quit = false;
bool Gameboy::bIsCGB = false;
Mem& Gameboy::getMem()
{
	return (*gbMem);
}

Clock& Gameboy::getClock()
{
	return (gbClock);
}

void	Gameboy::init()
{
	Screen::create();
	Cpu::loadBootRom();
	clockLine = 0;
	internalLY = 0;
}

bool Gameboy::loadRom(std::string pathToFile)
{
	gbMem = Mem::loadFromFile(pathToFile);
	if (!gbMem)
		return false;
	bIsCGB = gbMem->isCGB();
	std::cout << (bIsCGB ? "cartridge is CGB" : "cartridge is DMG") << std::endl;
	return gbMem->isValid;
}

bool Gameboy::run()
{
	Loop::loop();
	return true;
}

void Gameboy::clear()
{
	delete gbMem;
	Screen::destroy();
}

bool Gameboy::execFrame(bool step)
{
    while (internalLY >= 0 && internalLY < 10) {
	Gameboy::setState(GBSTATE_V_BLANK);
        if (Cpu::executeLine(step, false)) {
		Cpu::updateLY(1);
		internalLY++;
	}
	if (step) {
		break ;
	}
    }
    if (internalLY == 10 && BIT(M_LCDC, 7)) {
	M_LY = 0x00;
    }
    while (internalLY >= 10 && internalLY < 154) {
		if (Cpu::executeLine(step, true)) {
			Cpu::updateLY(1);
			internalLY++;
		}
		if (step) {
			break ;
		}
	}
	internalLY %= 154;
	return true;
}


void Gameboy::setState(int newState)
{
	if (BIT(M_LCDC, 7) && currentState != newState) {
		if (newState == GBSTATE_V_BLANK) {
			if (BIT(M_LCDC_STATUS, 4)) {
				Cpu::request_interrupt(IT_LCD_STAT);
			}
			Cpu::request_interrupt(IT_VBLANK);
			if (BIT(M_LCDC, 7)) {
				M_LY = 0x90;
			}
		}
		if (newState == GBSTATE_PX_TRANSFERT) {
			Ppu::finalLine = Ppu::doOneLine();
			for (int j = 0; BIT(M_LCDC, 7) && j < PIXEL_PER_LINE; j++) {
				Screen::drawPoint(j, internalLY - 10, Ppu::finalLine[j],
					Screen::pixels, Screen::pitch);
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
	currentState = newState;
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
