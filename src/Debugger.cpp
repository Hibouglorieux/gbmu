
#include "Debugger.hpp"
#include <SDL2/SDL.h>
#include "Gameboy.hpp"
#include "define.hpp"
#include "Screen.hpp"
#include "Cpu.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"
#include <cstdio>
#include "imgui/imconfig.h"
#include "Sprite.hpp"
#include "TilePixels.hpp"

SDL_Texture 	*Debugger::BGTexture = nullptr;
void 		*Debugger::BGPixels = nullptr;
int 		Debugger::BGPitch = 0;

SDL_Texture 	*Debugger::SpriteTexture = nullptr;
void 		*Debugger::SpritePixels = nullptr;
int 		Debugger::SpritePitch = 0;

SDL_Texture 	*Debugger::VRamTexture = nullptr;
void 		*Debugger::VramPixels = nullptr;
int 		Debugger::VramPitch = 0;


DebuggerState Debugger::state = DebuggerState::RUNNING;
// DebuggerState Debugger::state = DebuggerState::PAUSED;
int Debugger::fps = 60;
unsigned int Debugger::stopAtFrame = 0;
int 		Debugger::mapAddr = 0x9c00;

void		Debugger::destroyTexture()
{
	SDL_DestroyTexture(BGTexture);
	SDL_DestroyTexture(SpriteTexture);
	SDL_DestroyTexture(VRamTexture);
}

bool	Debugger::createTexture(bool bIsCGB, SDL_Renderer* uiRenderer)
{
	BGTexture = SDL_CreateTexture(uiRenderer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING,
			32 * BG_SCREEN_SCALE * 9,
			32 * BG_SCREEN_SCALE * 9);
	if (!BGTexture) {
		// std::cerr << "Erreur SDL_CreateTexture BG : "<< SDL_GetError() << std::endl;
        	Gameboy::throwError( "Erreur SDL_CreateTexture BG");
		return false;
	}

	SpriteTexture = SDL_CreateTexture(uiRenderer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING,
			32 * BG_SCREEN_SCALE * 9,
			2 * BG_SCREEN_SCALE * 9);
	if (!SpriteTexture) {
		//std::cerr << "Erreur SDL_CreateTexture Sprite : "<< SDL_GetError() << std::endl;
        	Gameboy::throwError( "Erreur SDL_CreateTexture Sprite");
		return false;
	}

	VRamTexture = SDL_CreateTexture(uiRenderer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING,
			16 * 9 * VRAM_SCREEN_SCALE * (bIsCGB ? 2 : 1) + (bIsCGB ? VRAM_SCREEN_SCALE * 2 : 0),
			24 * 9 * VRAM_SCREEN_SCALE);
	if (!VRamTexture) {
		// std::cerr << "Erreur SDL_CreateTexture VRam : "<< SDL_GetError() << std::endl;
        	Gameboy::throwError( "Erreur SDL_CreateTexture VRAM");
		return false;
	}
	lockTexture();
	return true;
}

void	Debugger::lockTexture()
{
    if (SDL_LockTexture(VRamTexture, nullptr, &VramPixels, &VramPitch)) {
        Gameboy::throwError( "Could not lock Vram texture");
    }
    if (SDL_LockTexture(BGTexture, nullptr, &BGPixels, &BGPitch)) {
        Gameboy::throwError( "Could not lock BG texture");
    }
    if (SDL_LockTexture(SpriteTexture, nullptr, &SpritePixels, &SpritePitch)) {
        Gameboy::throwError( "Could not lock BG texture");
    }
}

void Debugger::hexdump() {
	{
	ImGui::Begin("Memory Hexdump:");

//        ImGui::Text("Number of Rom: %d\nCurrent Rombank for 1st slot: %d\nCurrent Rombank for 2nd slot:", mem.mbc->getRomBank(0));
//        ImGui::SameLine();
//        ImGui::Text("Number of Ram: %d\nCurrent Rambank: %d\n");
		// Display contents in a scrolling region
		ImGui::BeginChild("Scrolling");

		int perLine = 16;
		int i;
		unsigned char buff[perLine+1];
		int len = 0xffff+1;

		for (i = 0; i < len; i++) {

			if ((i % perLine) == 0) {

				if (i != 0) {
					ImGui::SameLine();
					ImGui::Text("  %s", buff);
					ImGui::NewLine();
				}
				// Output the offset of current line.
				ImGui::SameLine();
				ImGui::Text("  0x%04x ", i);
			}

			// Now the hex code for the specific character.
			ImGui::SameLine();
			ImGui::Text(" %02X", (int)mem[i]);

			// And buffer a printable ASCII character for later.
			if (std::isprint(mem[i]))
				buff[i % perLine] = mem[i];
			else
				buff[i % perLine] = '.';
			buff[(i % perLine) + 1] = '\0';
		}

		// Pad out last line if not exactly perLine characters.
		while ((i % perLine) != 0) {
			ImGui::SameLine();
			ImGui::Text("   ");
			i++;
		}

		// And print the final ASCII buffer.
		ImGui::SameLine();
		ImGui::Text("  %s", buff);
		ImGui::EndChild();
		ImGui::End();
	}
}

void Debugger::registers() {
    {
        ImGui::Begin("Registers:");
        ImGui::Columns(2, "registers", true);
        ImGui::Separator();
        ImGui::Text("8 bits Register");
        ImGui::NewLine();

        ImGui::Text("A = [0x%02x]   F = [0x%02x]", Cpu::A, Cpu::F);
        ImGui::NextColumn();
        ImGui::Text("16 bits Register");
        ImGui::NewLine();
        ImGui::Separator();
        ImGui::Text("AF = [0x%04X]", Cpu::AF);
        ImGui::Separator();
        ImGui::NextColumn();

        ImGui::Text("B = [0x%02x]   C = [0x%02x]", Cpu::B, Cpu::C);
        ImGui::NextColumn();
        ImGui::Text("BC = [0x%04X]", Cpu::BC);
        ImGui::Separator();
        ImGui::NextColumn();

        ImGui::Text("D = [0x%02x]   E = [0x%02x]", Cpu::D, Cpu::E);
        ImGui::NextColumn();
        ImGui::Text("DE = [0x%04X]", Cpu::DE);
        ImGui::Separator();
        ImGui::NextColumn();

        ImGui::Text("H = [0x%02x]   L = [0x%02x]", Cpu::H, Cpu::L);
        ImGui::NextColumn();
        ImGui::Text("HL = [0x%04X]", Cpu::HL);
        ImGui::Separator();
        ImGui::NextColumn();

        ImGui::Text("       LY = [0x%02X]", (int)M_LY);
        ImGui::Text("       PC = [0x%04X]", Cpu::PC);
        ImGui::Text("       opcode = [0x%02X] %02X %02X", (int)mem[Cpu::PC], (int)mem[Cpu::PC + 1], (int)mem[Cpu::PC + 2]);
        ImGui::NextColumn();
        ImGui::Text("SP = [0x%04X]", Cpu::SP);
        ImGui::Text("frameNumber : %u", Gameboy::frameNb);
        ImGui::NewLine();

        ImGui::NextColumn();
        ImGui::Text("Flags:");
        ImGui::Separator();
        ImGui::Columns(4, "flags");
        ImGui::Text("Zero");
        ImGui::Text(Cpu::getZeroFlag() ? "1" : "0"); ImGui::NextColumn();
        ImGui::Text("Add/Sub");
        ImGui::Text(Cpu::getSubtractFlag() ? "1" : "0"); ImGui::NextColumn();
        ImGui::Text("Half carry");
        ImGui::Text(Cpu::getHalfCarryFlag() ? "1" : "0"); ImGui::NextColumn();
        ImGui::Text("Carry");
        ImGui::Text(Cpu::getCarryFlag() ? "1" : "0"); ImGui::NextColumn();
        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::Columns(1);
        ImGui::Separator();

        ImGui::Text("Interrupts:");
        ImGui::Checkbox("Interupts Master Enable (IME)",&Cpu::IME);
    	unsigned char ie = M_EI;
    	unsigned char iflag = M_IF;
    	ImGui::Text("Interrupt Enable (0xFFFF): 0x%02X\nInterrupt Flag   (0xFF0F): 0x%02X",ie, iflag);
    	ImGui::Separator();
    	ImGui::Columns(6, "flags"); ImGui::NewLine();
        ImGui::Text("IE ="); ImGui::NextColumn();
        ImGui::Text("V-Blank");
    	ImGui::Text(BIT(ie, 0) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Text("LCD Status");
    	ImGui::Text(BIT(ie, 1) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Text("Timer");
    	ImGui::Text(BIT(ie, 2) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Text("Serial");
    	ImGui::Text(BIT(ie, 3) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Text("Joypad");
    	ImGui::Text(BIT(ie, 4) ? "1" : "0"); ImGui::NextColumn();
        ImGui::Text("IF ="); ImGui::NextColumn();
    	ImGui::Text(BIT(iflag, 0) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Text(BIT(iflag, 1) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Text(BIT(iflag, 2) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Text(BIT(iflag, 3) ? "1" : "0"); ImGui::NextColumn();
        ImGui::Text(BIT(iflag, 4) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Columns(1);
    	ImGui::Separator();
        ImGui::End();
    }
}

void Debugger::Sprites() {
	const int OAM_Addr = 0xFE00;

	for (int i = 0; i < SPRITES_IN_OAM; i++) {
		struct OAM_entry *entry = (struct OAM_entry *)(&mem[OAM_Addr + i*4]);
		ImGui::Text("Sprite Index:%02d", i);
		ImGui::Text("tileIndex:%02x ", entry->tileIndex);
            	ImGui::SameLine();
		ImGui::Text("posY:%02x ", entry->posY);
            	ImGui::SameLine();
		ImGui::Text("posX:%02x ", entry->posX);
            	ImGui::SameLine();
		ImGui::Text("attributes:%02x", entry->attributes);
		ImGui::NewLine();
	}
}

void	Debugger::drawSprite(void)
{
	const int OAM_Addr = 0xFE00;
	unsigned char spriteHeight = 8; // Always show the sprit in 8x8 mode

	for (int i = 0; i < SPRITES_IN_OAM; i++) {
		struct OAM_entry *entry = (struct OAM_entry *)(&mem[OAM_Addr + i*4]);
		Sprite sprite = Sprite(*entry, spriteHeight);
		if (entry->getFlipY()) // reverse offset if flipped
			sprite.flipY();
		if (entry->getFlipX())
			sprite.flipX();

		int x_offset = (i % 32) * 9;
		int y_offset = (i / 32) * 9;
		for (int y = 0; y < 8; y++) {
			// fetch the 8 pixel of the sprite in a tmp buffer
			std::array<short, 8> line = sprite.getColoredLine(y);
			for (int x = 0; x < 8; x++) {
				Screen::drawPoint(x + x_offset, y + y_offset, line[x], (int*)SpritePixels,
						SpritePitch, BG_SCREEN_SCALE, Gameboy::bIsCGB);
			}
		}
	}
}

void	Debugger::drawBG(int mapAddr)
{
	unsigned int BGMap  = mapAddr;
    unsigned int BGDataAddress = BIT(M_LCDC, 4) ? 0x8000 : 0x8800;

	for (unsigned short i = 0; i < 32 * 32; i++) {
		// in order to get the background map displayed we need to fetch the tile to display
		// which is its number (fetched in BGMap which is 32 * 32), then we need
		// to find that data in the VRam, (BGDataAddrress[tileNumber * (size in byte per tile)])
		int tileIndex = mem[BGMap + i];
		int addr = BGDataAddress;
		if (BGDataAddress == 0x8800)
		{
			tileIndex = char(tileIndex);
			addr = 0x9000;
		}
		struct TilePixels tile = TilePixels(addr + (tileIndex * (8 * 2)), BGMap + i);
		int x_offset = (i % 32) * 9;
		int y_offset = (i / 32) * 9;
		for (int y = 0; y < 8; y++) {
			auto line = tile.getColorLine(y);
			for (int x = 0; x < 8; x++) {
				Screen::drawPoint(x + x_offset, y + y_offset, line[x], (int*)BGPixels,
						BGPitch, BG_SCREEN_SCALE, Gameboy::bIsCGB);
			}
		}
	}
}

void	Debugger::drawPalettes()
{
	ImGui::Columns(10, "palettes", true);
	auto white = ImGui::ColorConvertU32ToFloat4(0xFFFFFFFF);
	auto black = ImGui::ColorConvertU32ToFloat4(0xFF000000);
	const std::array<unsigned char, 64>& BGPalette = mem.getBGPalettes();
	const std::array<unsigned char, 64>& OBJPalette = mem.getOBJPalettes();
	for (int paletteNb = 0; paletteNb < 8; paletteNb++)
	{
		ImGui::Text("%d:", paletteNb);
		ImGui::NextColumn();
		for (int paletteSelector = 0; paletteSelector < 2; paletteSelector++)
		{
			const std::array<unsigned char, 64>& palette = (paletteSelector == 1 ? OBJPalette : BGPalette);
			for (int colorNb = 0; colorNb < 4; colorNb++)
			{
				const unsigned char& low = palette[paletteNb * 2 * 4 + colorNb * 2];
				const unsigned char& high = palette[paletteNb * 2 * 4 + colorNb * 2 + 1];
				const unsigned short color = (high << 8) | low;
				const int colorImGUI = Screen::convertColorFromCGB(color, true);
				ImVec2 min = ImGui::GetItemRectMin();
				ImVec2 max = ImGui::GetContentRegionMax();
				max.x += min.x;
				max.y = min.y + 16;

				ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(),
						max, colorImGUI);
				ImGui::TextColored(white, "%04X", color);
				ImGui::SameLine();
				ImGui::TextColored(black, "%04X", color);
				ImGui::NextColumn();
			}
			if (paletteSelector == 0)
				ImGui::NextColumn();
		}
		ImGui::Separator();
	}
}

void	Debugger::drawVRam(bool bIsCGB)
{
    unsigned int vRamAddress = 0x8000;

	if (!bIsCGB)
		for (unsigned char xx = 0; xx < 16; xx++) {
			for (unsigned char yy = 0; yy < 24; yy++) {
				struct TilePixels tile = TilePixels(vRamAddress + (xx * 8 * 2 + yy * 16 * 8 * 2), 0); // XXX nallani how to get palette there?
				const unsigned char x_offset = xx * 9;
				const unsigned char y_offset = yy * 9;
				for (int y = 0; y < 8; y++) {
					auto line = tile.getColorLine(y);
					for (int x = 0; x < 8; x++) {
						Screen::drawPoint(x + x_offset, y + y_offset, line[x], (int*)VramPixels,
								VramPitch, VRAM_SCREEN_SCALE, Gameboy::bIsCGB);
					}
				}
			}
		}
	else
	{
		for (unsigned char Vram = 0; Vram < 2; Vram++)
			for (unsigned char xx = 0; xx < 16; xx++) {
				for (unsigned char yy = 0; yy < 24; yy++) {
					struct TilePixels tile = TilePixels(vRamAddress + (xx * 8 * 2 + yy * 16 * 8 * 2), 0, Vram == 0 ? FORCE_DMG_TILEPIXELS : FORCE_CGB_TILEPIXELS); // XXX nallani how to get palette there?
					const unsigned char x_offset = xx * 9;
					const unsigned char y_offset = yy * 9;
					for (int y = 0; y < 8; y++) {
						auto line = tile.getColorLine(y);
						for (unsigned char x = 0; x < 8; x++) {
							Screen::drawPoint(x + x_offset + Vram * (16 * 9 + 2), y + y_offset, line[x],
									(int*)VramPixels, VramPitch, VRAM_SCREEN_SCALE, Gameboy::bIsCGB);
							//std::cout << "drew at x: " << x + x_offset + Vram * (16 * 9 + 2) << " y: " << y + y_offset << std::endl;
						}
					}
				}
			}
	}

}
