#include "Gameboy.hpp"
#include "imgui/imgui_impl_sdl.h"
#include "Hdma.hpp"
#include "Debugger.hpp"

Mem* Gameboy::gbMem = nullptr;
Clock Gameboy::gbClock = Clock();
int Gameboy::currentState = 0;

uint8_t Gameboy::internalLY = 0;
int Gameboy::clockLine = 0;

bool Gameboy::bLCDWasOff = false;
bool Gameboy::bShouldRenderFrame = true;
bool Gameboy::quit = false;
bool Gameboy::bIsCGB = false;
bool Gameboy::bIsROM = false;
std::string Gameboy::path = "";
unsigned int Gameboy::frameNb = 0;

bool bLogFrameNb = false;

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
//	Cpu::loadBootRom();
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
	bIsCGB = gbMem->isCGB();
	std::cout << (bIsCGB ? "cartridge is CGB" : "cartridge is DMG") << std::endl;
    Cpu::loadBootRom();
	return gbMem->isValid;
}

bool Gameboy::run()
{
    Screen::create(bIsCGB);
	clockLine = 0;
	internalLY = 0;
	Loop::loop();
	return true;
}

void Gameboy::clear()
{
	delete gbMem;
	Screen::destroy();
}

void Gameboy::changeLCD(bool bActivateLCD)
{
	if (!bActivateLCD)
	{
		bLCDWasOff = false;
		bShouldRenderFrame = false;
	}
}

bool Gameboy::execFrame(Gameboy::Step step, bool bRefreshScreen)
{
	bShouldRenderFrame = !bLCDWasOff;
	bLCDWasOff = !BIT(M_LCDC, 7);

	if (bLogFrameNb)
		std::cout << "frameNb: " << std::endl;
	frameNb++;
	// render a white screen if LCD is off
	// normal render wont be called since we wont enter pxl transfer state
	if (!bShouldRenderFrame)
		for (int i = 0; i < 144; i++)
			Screen::updateMainScreen(Ppu::getDefaultWhiteLine(), i);

	std::function<bool()> loopFunc = [&]()
	{
		if (Cpu::executeLine(step == Step::oneInstruction, internalLY < 144, bRefreshScreen))
		{
			Cpu::updateLY(1);
			if (M_LY == 0)
			{
				if (BIT(M_LCDC, 7))
					internalLY = 0;
				else
					internalLY++;
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
	//implementation: if lcdc is off then count clocks in order to give the screen
	//(even if it's white), otherwise always return when ly > 144 because of Screen Tearing
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

void Gameboy::doHblankHdma()
{
	int clockHblankForHdma = Hdma::updateHBlank();
	if (clockHblankForHdma)
	{
		// update g_clock/clock here instead of cpu
		// because it has to be done once per hblank
		g_clock += clockHblankForHdma;
		/*
		if (Clock::cgbMode)
			g_clock += 2; // overhead (always 4 clock)
		else
			g_clock += 1; // overhead (always 4 clock)
		*/
		clockLine += clockHblankForHdma;
	}
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
		if (newState == GBSTATE_H_BLANK)
		{
			if (bRefreshScreen)
			{
				// we need to have the ternary because the
				// frame after the lcd was put on is still white
				Screen::updateMainScreen( bShouldRenderFrame ? Ppu::doOneLine() : Ppu::getDefaultWhiteLine()
						, internalLY);
			}
		}
		if (newState == GBSTATE_H_BLANK) {
			// need to check hblank hdma
			doHblankHdma();
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

// static bool interrupts_master_enable;
// static bool interrupts_flag;
// static bool halted;
// static uint32_t halt_counter;

// static unsigned short PC;
// static unsigned short SP;
// static unsigned short registers[4];
// static Clock cpuClock;

void Gameboy::loadSaveState(std::string path) {
	s_state tmp;
	std::cout << "Loading game state\n";
	
	std::vector<unsigned char> content = Mem::readFile(path);
	memcpy(&tmp, content.data(), sizeof(tmp));

	size_t romHash = 0;
	for (size_t i = 0; i < mem.romBanks.size(); i++)
		romHash += ft_hash(mem.romBanks.data()[i], ROM_BANK_SIZE);
	if (romHash != tmp.romHash)
		throw "Could not load save state from a different game\n";

	// Load CPU state
	Cpu::interrupts_master_enable = tmp.cpu.interrupts_master_enable;
	Cpu::interrupts_flag = tmp.cpu.interrupts_flag;
	Cpu::halted = tmp.cpu.halted;
	Cpu::halt_counter = tmp.cpu.halt_counter;
	Cpu::PC = tmp.cpu.PC;
	Cpu::SP = tmp.cpu.SP;
	memcpy(&Cpu::registers, tmp.cpu.registers, sizeof(unsigned short[4]));

	// Load Clock state
	Gameboy::getClock().setClock(tmp.clock.val);
	Gameboy::getClock().setDivClock(tmp.clock.divClock);
	Gameboy::getClock().setReloadTMA(tmp.clock.reloadTMA);
	Gameboy::getClock().setTimaClock(tmp.clock.timaClock);

	// Load PPU state
	Ppu::setWindowCounter(tmp.ppu.windowCounter);

	// Load Gameboy state
	Gameboy::clockLine = tmp.gameboy.clockLine;
	Gameboy::currentState = tmp.gameboy.currentState;
	Gameboy::bIsCGB = tmp.gameboy.bIsCGB;
	Gameboy::internalLY = tmp.gameboy.internalLY;
	// Gameboy::path = tmp.gameboy.path;
	Gameboy::quit = tmp.gameboy.quit;
	
	// Load MBC state
	switch (Gameboy::getMem().mbc->getType())
	{
	case 0:
		break;
	case 1: {
		MBC1 *ptr = dynamic_cast<MBC1*>(Gameboy::getMem().mbc);
		ptr->setAdvancedBankingMode(tmp.mbc.bank.mbc1.bAdvancedBankingMode);
		ptr->setEnableRam(tmp.mbc.bank.mbc1.bEnableRam);
		ptr->setHighBitsRomBankNumberOrRam(tmp.mbc.bank.mbc1.highBitsRomBankNumberOrRam);
		ptr->setLowBitsRomBankNumber(tmp.mbc.bank.mbc1.lowBitsRomBankNumber);
	} break;
	case 2: {
		MBC2 *ptr = dynamic_cast<MBC2*>(Gameboy::getMem().mbc);
		ptr->setEnableRam(tmp.mbc.bank.mbc2.bEnableRam);
		ptr->setRomBankNb(tmp.mbc.bank.mbc2.romBankNb);
	} break;
	case 3: {
		MBC3 *ptr = dynamic_cast<MBC3*>(Gameboy::getMem().mbc);
		ptr->setEnableRam(tmp.mbc.bank.mbc3.bEnableRam);
		ptr->setStart(tmp.mbc.bank.mbc3.start);
		ptr->setRTC(tmp.mbc.bank.mbc3.rtc_register);
		ptr->setRTCBind(tmp.mbc.bank.mbc3.rtcBindNb);
		ptr->setRomBankNb(tmp.mbc.bank.mbc3.romBankNb);
		ptr->setRamBankNb(tmp.mbc.bank.mbc3.ramBankNb);
		ptr->setLastVal(tmp.mbc.bank.mbc3.lastVal);
		ptr->setLatch(tmp.mbc.bank.mbc3.latched);
	} break;
	case 5: {
		MBC5 *ptr = dynamic_cast<MBC5*>(Gameboy::getMem().mbc);
		ptr->setEnableRam(tmp.mbc.bank.mbc5.bEnableRam);
		ptr->setLeastSignificantRomByte(tmp.mbc.bank.mbc5.leastSignificantRomByte);
		ptr->setBit9(tmp.mbc.bank.mbc5.bit9);		
		ptr->setRamBankNb(tmp.mbc.bank.mbc5.ramBankNb);
	} break;
	
	default:
		throw "Incorrect MBC type for save state loading";
	}

	size_t offset = sizeof(s_state);
	
	if (mem.mbc->hasTimer) {
		// Fetching timer save
		MBC3 *ptr = dynamic_cast<MBC3*>(mem.mbc);
		if (!ptr) throw "Could not dynamically cast MBC3 pointer (loading save state)";
		memcpy(&ptr->start, content.data() + offset, sizeof(time_t));
	}

	offset += (mem.mbc->hasTimer ? sizeof(time_t) : 0);

	memcpy(Gameboy::getMem().getInternalArray(), content.data() + offset, MEM_SIZE);

	offset += MEM_SIZE;

	for (auto & extraRamBank : Gameboy::getMem().extraRamBanks) {
		memcpy(extraRamBank, content.data() + offset, RAM_BANK_SIZE);
		offset += RAM_BANK_SIZE;
	}

	CGBMem *ptr = dynamic_cast<CGBMem*>(&Gameboy::getMem());
	if (ptr) {
		// CGB
		ptr->CGBextraRamBankNb = tmp.cgb.CGBextraRamBankNb;
		
		for (int i = 0; i < 64; i++) {
			ptr->BGPalettes[i] = tmp.cgb.BGPalettes[i];
			ptr->OBJPalettes[i] = tmp.cgb.OBJPalettes[i];
		}
		ptr->bIsUsingCGBVram = tmp.cgb.bIsUsingCGBVram;
		
		memcpy(ptr->getCGBVram(), content.data() + offset, 0x2000);
		offset += 0x2000;

		for (int i = 0; i < 8; i++) {
			if (ptr->getCGBExtraRamBanks()[i])
				memcpy(ptr->getCGBExtraRamBanks()[i], content.data() + offset, 0x1000);
			offset += 0x1000;
		}
	}
}

void Gameboy::saveState() {
	std::cout << "Saving game state\n";
	s_state tmp;

	tmp.romHash = 0;
	for (int i = 0; i < mem.romBanks.size(); i++)
		tmp.romHash += ft_hash(mem.romBanks.data()[i], ROM_BANK_SIZE);

	std::cout << "Rom hash : " << std::dec << tmp.romHash << "\n";
	
	// Save CPU state
	tmp.cpu.interrupts_master_enable = Cpu::interrupts_master_enable;
	tmp.cpu.interrupts_flag = Cpu::interrupts_flag;
	tmp.cpu.halted = Cpu::halted;
	tmp.cpu.halt_counter = Cpu::halt_counter;
	tmp.cpu.PC = Cpu::PC;
	tmp.cpu.SP = Cpu::SP;
	memcpy(&tmp.cpu.registers, Cpu::registers, sizeof(Cpu::registers));
	
	// Save clock state
	tmp.clock.divClock = Gameboy::getClock().getDivClock();
	tmp.clock.timaClock = Gameboy::getClock().getTimaClock();
	tmp.clock.reloadTMA = Gameboy::getClock().getReloadTMA();
	tmp.clock.val = Gameboy::getClock().getClock();
	tmp.clock.cgbMode = Gameboy::getClock().cgbMode;

	tmp.mbc.type = Gameboy::getMem().mbc->getType();

	// Save PPU state
	tmp.ppu.windowCounter = Ppu::getWindowCounter();

	// Save Gameboy state
	tmp.gameboy.currentState = Gameboy::currentState;
	// tmp.gameboy.path = Gameboy::path;
	tmp.gameboy.bIsCGB = Gameboy::bIsCGB;
	tmp.gameboy.clockLine = Gameboy::clockLine;
	tmp.gameboy.internalLY = Gameboy::internalLY;
	tmp.gameboy.quit = Gameboy::quit;

	// Save MBC state
	switch (tmp.mbc.type)
	{
	case 0:
		break;
	case 1: {
		MBC1 *ptr = dynamic_cast<MBC1*>(Gameboy::getMem().mbc);
		tmp.mbc.bank.mbc1.bAdvancedBankingMode = ptr->getAdvancedBankingMode();
		tmp.mbc.bank.mbc1.bEnableRam = ptr->getEnableRam();
		tmp.mbc.bank.mbc1.highBitsRomBankNumberOrRam = ptr->getHighLowBitsRomBankNumber();
		tmp.mbc.bank.mbc1.lowBitsRomBankNumber = ptr->getLowBitsRomBankNumber();
	} break;
	case 2: {
		MBC2 *ptr = dynamic_cast<MBC2*>(Gameboy::getMem().mbc);
		tmp.mbc.bank.mbc2.bEnableRam = ptr->getEnableRam();
		tmp.mbc.bank.mbc2.romBankNb = ptr->getRomBankNb();
	} break;
	case 3: {
		MBC3 *ptr = dynamic_cast<MBC3*>(Gameboy::getMem().mbc);
		tmp.mbc.bank.mbc3.bEnableRam = ptr->getEnableRam();
		tmp.mbc.bank.mbc3.lastVal = ptr->getLastVal();
		tmp.mbc.bank.mbc3.latched = ptr->getLatch();
		tmp.mbc.bank.mbc3.ramBankNb = ptr->getRamBankNb();
		tmp.mbc.bank.mbc3.romBankNb = ptr->getRomBankNb();
		tmp.mbc.bank.mbc3.rtc_register = ptr->getRTC();
		tmp.mbc.bank.mbc3.rtcBindNb = ptr->getRTCBind();
		tmp.mbc.bank.mbc3.start = ptr->getStart();
	} break;
	case 5: {
		MBC5 *ptr = dynamic_cast<MBC5*>(Gameboy::getMem().mbc);
		tmp.mbc.bank.mbc5.bEnableRam = ptr->getEnableRam();
		tmp.mbc.bank.mbc5.bit9 = ptr->getBit9();
		tmp.mbc.bank.mbc5.leastSignificantRomByte = ptr->getLeastSignificantRomByte();
		tmp.mbc.bank.mbc5.ramBankNb = ptr->getRamBankNb();
	} break;
	
	default:
		break;
	}


	// std::cout << "\tIME = " << Cpu::interrupts_master_enable << "\n";
	// std::cout << "\tIF = " << Cpu::interrupts_flag << "\n";
	// std::cout << "\tHalted = " << Cpu::halted << "\n";
	// std::cout << "\tHalt_counter = " << Cpu::halt_counter << "\n";
	// std::cout << "\tPC = " << Cpu::PC << "\n";
	// std::cout << "\tSP = " << Cpu::SP << "\n";
	// std::cout << "\tRegisters = " << Cpu::registers[0] << Cpu::registers[1] << Cpu::registers[2] << Cpu::registers[3] << "\n";
	// std::cout << "\tLY = " << (int)M_LY << "\n";
	
	std::ofstream outfile(path + ".state", std::ios::binary);

	CGBMem *ptr = dynamic_cast<CGBMem*>(&Gameboy::getMem());
	if (ptr) {
		// CGB
		tmp.cgb.bIsUsingCGBVram = ptr->bIsUsingCGBVram;
		tmp.cgb.CGBextraRamBankNb = ptr->CGBextraRamBankNb;
		memcpy(tmp.cgb.BGPalettes, ptr->BGPalettes.data(), sizeof(unsigned char) * 64);
		memcpy(tmp.cgb.OBJPalettes, ptr->OBJPalettes.data(), sizeof(unsigned char) * 64);
	}

	outfile.write(reinterpret_cast<char*>(&tmp), sizeof(tmp));


	if (mem.mbc->hasTimer) {
		MBC3 *ptr = dynamic_cast<MBC3*>(mem.mbc);
		if (!ptr)
			throw "Could not dynamically cast MBC3 (saveRam function)";
		outfile.write(reinterpret_cast<char *>(&ptr->start), sizeof(time_t));
	}

	outfile.write(reinterpret_cast<char*>(mem.getInternalArray()), MEM_SIZE);

	for (unsigned char *elem : mem.extraRamBanks) {
		outfile.write(reinterpret_cast<char*>(elem), RAM_BANK_SIZE);
	}

	if (ptr) {
		outfile.write(reinterpret_cast<char*>(ptr->getCGBVram()), 0x2000);

		for (int i = 0; i < 8; i++) {
			if (ptr->getCGBExtraRamBanks()[i])
				outfile.write(reinterpret_cast<char*>(ptr->getCGBExtraRamBanks()[i]), 0x1000);
		}
	}

	outfile.close();
}

void Gameboy::saveRam() {	
	std::ofstream outfile(path + ".save", std::ios::binary);

	if (mem.mbc->hasTimer) {
		MBC3 *ptr = dynamic_cast<MBC3*>(mem.mbc);
		if (!ptr)
			throw "Could not dynamically cast MBC3 (saveRam function)";
		outfile.write(reinterpret_cast<char *>(&ptr->start), sizeof(time_t));
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

bool Gameboy::ProcessFile(char * filename) {
    std::string str = filename;
    auto extension = str.substr(str.find_last_of(".") + 1);
    if(extension == "gb" || extension == "cgb")
        return true;
    return false;
}

void Gameboy::pollEvent()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		ImGui_ImplSDL2_ProcessEvent(&event);
		Screen::handleEvent(&event);
		Joypad::handleEvent(&event);
        if (event.type == SDL_DROPFILE) {
            auto filePath = event.drop.file;
            if (Gameboy::ProcessFile(filePath)) {
                Gameboy::bIsROM = true;
                Gameboy::loadRom(filePath);
            }
            SDL_free(filePath);
       }
		if (event.type == SDL_QUIT)
			Gameboy::quit = true;
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
				event.window.windowID == SDL_GetWindowID(Screen::window))
			Gameboy::quit = true;
	}
}
