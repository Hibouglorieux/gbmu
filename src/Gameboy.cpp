#include "Gameboy.hpp"
#include "imgui/imgui_impl_sdl.h"

Mem* Gameboy::gbMem = nullptr;
Clock Gameboy::gbClock = Clock();
int Gameboy::currentState = 0;

uint8_t Gameboy::internalLY = 0;
int Gameboy::clockLine = 0;

bool Gameboy::quit = false;
bool Gameboy::bIsCGB = false;
std::string Gameboy::path = "";

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
	Cpu::loadBootRom();
	Screen::create(bIsCGB);
	clockLine = 0;
	internalLY = 0;
}

bool Gameboy::loadRom(std::string pathToFile)
{
	gbMem = Mem::loadFromFile(pathToFile);
	Gameboy::path = pathToFile;
	if (!gbMem)
		return false;
	//bIsCGB = gbMem->isCGB();
	bIsCGB = true;
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

bool Gameboy::execFrame(Gameboy::Step step, bool bRefreshScreen)
{
	std::function<bool()> loopFunc = [&]()
	{
		if (Cpu::executeLine(step == Step::oneInstruction, internalLY < 144, bRefreshScreen))
		{
			Cpu::updateLY(1);
			if (M_LY == 0)
			{
				internalLY = 0;
				Ppu::resetWindowCounter();
			}
			else
				internalLY++;
		}
		if (step == Step::oneLine || step == Step::oneInstruction)
			return false;
		return true;
	};

	//TODO this needs rework as it might not take a frame if LCD is disabled
	//but at least it returns when whole 144 lines have been rendered
	while (internalLY < 144)
	{
		if (!loopFunc())
			break;
	}
	while (internalLY >= 144 && internalLY < 154)
	{
		Gameboy::setState(GBSTATE_V_BLANK, bRefreshScreen);
		if (!loopFunc())
			break;
	}
	if (internalLY >= 154)
		Ppu::resetWindowCounter();
	internalLY %= 154;
	return true;
}

void Gameboy::setState(int newState, bool bRefreshScreen)
{
	if (BIT(M_LCDC, 7) && currentState != newState)
	{
		if (newState == GBSTATE_V_BLANK)
		{
			if (BIT(M_LCDC_STATUS, 4))
			{
				//std::cout << "request interrupt VBLANK" << std::endl;
				Cpu::request_interrupt(IT_LCD_STAT);
			}
			Cpu::request_interrupt(IT_VBLANK);
		}
		// should refresh screen
		if (newState == GBSTATE_PX_TRANSFERT)
		{
			if (bRefreshScreen && BIT(M_LCDC, 7))
			{
				Screen::updateMainScreen(Ppu::doOneLine(), internalLY);
			}
		}
		if (newState == GBSTATE_H_BLANK && BIT(M_LCDC_STATUS, 3)) {
			//std::cout << "request interrupt HBLANK" << std::endl;
			Cpu::request_interrupt(IT_LCD_STAT);
		}
		if (newState == GBSTATE_OAM_SEARCH && BIT(M_LCDC_STATUS, 5)) {
			//std::cout << "request interrupt OAM" << std::endl;
			Cpu::request_interrupt(IT_LCD_STAT);
		}
		unsigned char lcdcs = M_LCDC_STATUS & ~0x07;
		lcdcs |= newState;
		mem.supervisorWrite(LCDC_STATUS, lcdcs);
	}
	currentState = newState;
}

void Gameboy::saveRam() {
	std::cout << "Saving game with " << mem.extraRamBanks.size() << " ram banks and timer : " << mem.mbc->hasTimer << "\n";
	
	std::ofstream outfile(path + ".save", std::ios::binary);

	if (mem.mbc->hasTimer) {
		MBC3 *ptr = dynamic_cast<MBC3*>(mem.mbc);
		if (!ptr)
			throw "Could not dynamically cast MBC3 (saveRam function)";
		outfile.write(reinterpret_cast<char *>(&ptr->start), sizeof(time_t));
		std::cout << "Saved timer : " << std::dec << (int)ptr->start << "\n";
	}

	outfile.write(reinterpret_cast<char*>(mem.getInternalArray()), MEM_SIZE);

	for (unsigned char *elem : mem.extraRamBanks) {
		outfile.write(reinterpret_cast<char*>(elem), RAM_BANK_SIZE);
	}
	outfile.close();
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
