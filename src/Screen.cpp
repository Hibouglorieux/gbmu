/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Screen.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <lmariott@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:46:17 by lmariott          #+#    #+#             */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Screen.hpp"
#include "Cpu.hpp"
#include "Ppu.hpp"
#include "Debugger.hpp"


#define MAIN_SCREEN_SCALE 4
#define VRAM_SCREEN_SCALE 2
#define BG_SCREEN_SCALE 2

SDL_Window* Screen::window = nullptr;
SDL_Renderer* Screen::renderer = nullptr;

SDL_Texture *Screen::texture = nullptr;
SDL_Texture *Screen::BGTexture = nullptr;
SDL_Texture *Screen::SpriteTexture = nullptr;
SDL_Texture *Screen::VRamTexture = nullptr;

void *Screen::pixels = nullptr;
int Screen::pitch = 0;

void *Screen::BGPixels = nullptr;
int Screen::BGPitch = 0;

void *Screen::SpritePixels = nullptr;
int Screen::SpritePitch = 0;

void *Screen::VramPixels = nullptr;
int Screen::VramPitch = 0;

int Screen::mapAddr = 0x9c00;
SDL_Window*	Screen::get()
{
	return (window);
}

void	Screen::destroy()
{
    ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_DestroyTexture(texture);
	SDL_DestroyTexture(BGTexture);
	SDL_DestroyTexture(SpriteTexture);
	SDL_DestroyTexture(VRamTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
    SDL_Quit();
}

void Screen::TexturetoImage(SDL_Texture * Texture) {
    SDL_SetRenderTarget(renderer, Texture);
   	SDL_UnlockTexture(Texture);
	int width;
	int height;
	SDL_QueryTexture(Texture, nullptr, nullptr, &width, &height);
    ImGui::Image((void*)(intptr_t)Texture, ImVec2(width, height));
//	SDL_RenderCopy(renderer, Texture, NULL, NULL);
    SDL_SetRenderTarget(renderer, nullptr);
}

int		Screen::convertColorFromCGB(int colo, bool bConvertForImGUI)
{
	unsigned char r = (colo & 0x1F);
	unsigned char g = ((colo & (0x1F << 5)) >> 5);
	unsigned char b = ((colo & (0x1F << 10)) >> 10);
	r *= 8;
	g *= 8;
	b *= 8;
	//r = 255 - r;
	//g = 255 - g;
	//b = 255 - b;
	if (bConvertForImGUI)
		return (r << IM_COL32_R_SHIFT) | (g << IM_COL32_G_SHIFT) | (b << IM_COL32_B_SHIFT) | (0xFF << 24);
	else
		return (r << 24) | (g << 16) | (b << 8);
}

void	Screen::NewframeTexture()
{
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    SDL_RenderClear(Screen::renderer);

    if (SDL_LockTexture(texture, nullptr, &pixels, &pitch)) {
        throw "Could not lock texture\n";
    }
    if (SDL_LockTexture(VRamTexture, nullptr, &VramPixels, &VramPitch)) {
        throw "Could not lock Vram texture\n";
    }
    if (SDL_LockTexture(BGTexture, nullptr, &BGPixels, &BGPitch)) {
        throw "Could not lock BG texture\n";
    }
    if (SDL_LockTexture(SpriteTexture, nullptr, &SpritePixels, &SpritePitch)) {
        throw "Could not lock BG texture\n";
    }
}

void Screen::clear(ImVec4 vec4)
{
    ImGui::Render();
    SDL_SetRenderDrawColor(Screen::renderer, vec4.x * 255, vec4.y * 255, vec4.z * 255, vec4.w * 255);
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
   	SDL_RenderPresent(Screen::renderer);
}

void	Screen::updateMainScreen(const std::array<short, PIXEL_PER_LINE>& lineData,
		unsigned char currentLine)
{
	for (unsigned char i = 0; i < PIXEL_PER_LINE; i++)
		Screen::drawPoint(i, currentLine, lineData[i],
				pixels, pitch, MAIN_SCREEN_SCALE);
}

void	Screen::drawSprite(void)
{
	const int OAM_Addr = 0xFE00;
	unsigned char spriteHeight = 8; // Always show the sprit in 8x8 mode

	for (int i = 0; i < MAX_SPRITES; i++) {
		struct OAM_entry *entry = (struct OAM_entry *)(&mem[OAM_Addr + i*4]);
		Sprite sprite = Sprite(*entry, spriteHeight);
		// in order to get the background map displayed we need to fetch the tile to display
		// which is its number (fetched in BGMap which is 32 * 32), then we need
		// to find that data in the VRam, (BGDataAddrress[tileNumber * (size in byte per tile)])
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
				drawPoint(x + x_offset, y + y_offset, line[x], SpritePixels, SpritePitch, BG_SCREEN_SCALE);
			}
		}
	}
}

void	Screen::drawBG(int mapAddr)
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
				drawPoint(x + x_offset, y + y_offset, line[x], BGPixels, BGPitch, BG_SCREEN_SCALE);
			}
		}
	}
}

void	Screen::drawPalettes()
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
				const int colorImGUI = convertColorFromCGB(color, true);
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

void	Screen::drawVRam(bool bIsCGB)
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
						drawPoint(x + x_offset, y + y_offset, line[x], VramPixels, VramPitch, VRAM_SCREEN_SCALE);
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
							drawPoint(x + x_offset + Vram * (16 * 9 + 2), y + y_offset, line[x], VramPixels, VramPitch, VRAM_SCREEN_SCALE);
							//std::cout << "drew at x: " << x + x_offset + Vram * (16 * 9 + 2) << " y: " << y + y_offset << std::endl;
						}
					}
				}
			}
	}

}

bool	Screen::createTexture(bool bIsCGB)
{
	texture = SDL_CreateTexture(renderer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING,
			160 * MAIN_SCREEN_SCALE,
			144 * MAIN_SCREEN_SCALE);
	if (!texture) {
		std::cerr << "Erreur SDL_CreateTexture Ppu : "<< SDL_GetError() << std::endl;
		return false;
	}

	BGTexture = SDL_CreateTexture(renderer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING,
			32 * BG_SCREEN_SCALE * 9,
			32 * BG_SCREEN_SCALE * 9);
	if (!BGTexture) {
		std::cerr << "Erreur SDL_CreateTexture BG : "<< SDL_GetError() << std::endl;
		return false;
	}

	SpriteTexture = SDL_CreateTexture(renderer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING,
			32 * BG_SCREEN_SCALE * 9,
			2 * BG_SCREEN_SCALE * 9);
	if (!SpriteTexture) {
		std::cerr << "Erreur SDL_CreateTexture Sprite : "<< SDL_GetError() << std::endl;
		return false;
	}

	VRamTexture = SDL_CreateTexture(renderer,
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING,
			16 * 9 * VRAM_SCREEN_SCALE * (bIsCGB ? 2 : 1) + (bIsCGB ? VRAM_SCREEN_SCALE * 2 : 0),
			24 * 9 * VRAM_SCREEN_SCALE);
	if (!VRamTexture) {
		std::cerr << "Erreur SDL_CreateTexture VRam : "<< SDL_GetError() << std::endl;
		return false;
	}
	return true;
}

bool	Screen::drawPoint(unsigned short x, unsigned short y, const unsigned short& color, void *pixels, int pitch, int pixelScale)
{
	x *= pixelScale;
	y *= pixelScale;
	int colorForSDL;
	if (!Gameboy::bIsCGB)
	{
		colorForSDL = 255 - color * (255 / 3);
		colorForSDL = (colorForSDL << 24) | (colorForSDL << 16) | (colorForSDL << 8);
	}
	else
		colorForSDL = convertColorFromCGB(color);
	int *p = (int*)pixels;
	for (int i = 0 ; i < pixelScale ; i++) {
		for (int j = 0 ; j < pixelScale ; j++) {
			p[(y + j) * (pitch / 4) + (i + x)] = colorForSDL | 0xFF;
		}
	}
	return (true);
}

bool	Screen::create(bool bIsCGB)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		std::cerr <<"Error SDL_Init! "<< SDL_GetError() << std::endl;
		return false;
	}

	auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE);
	window = SDL_CreateWindow("GBMU",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			880,
			824,
			window_flags);
	if (!window) {
		std::cerr <<"Error SDL_CreateWindow! "<< SDL_GetError() << std::endl;
		return (false);
	}


	renderer = SDL_CreateRenderer(window, -1,  SDL_RENDERER_ACCELERATED); //SDL_RENDERER_PRESENTVSYNC
	if (!renderer)
	{
		std::cerr <<"Error SDL_CreateRenderer : "<< SDL_GetError() << std::endl;
		return false;
	}

	if (!Screen::createTexture(bIsCGB))
		return false;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);
	SDL_RenderClear(renderer);
	return (true);
}

#include <fstream>

void	Screen::handleEvent(SDL_Event *ev)
{
	if (ev->type == SDL_WINDOWEVENT) {
		switch (ev->window.event) {
			case SDL_WINDOWEVENT_CLOSE:
				Gameboy::quit = true;
				Gameboy::saveRam();
				break;
		}
	}
	if (ev->type == SDL_KEYDOWN)
	{
		if (ev->key.keysym.sym == SDLK_ESCAPE) {
			Gameboy::quit = true;
			Gameboy::saveRam();
		}
	}
}
