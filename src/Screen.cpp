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

SDL_Window* Screen::window = nullptr;
SDL_Renderer* Screen::renderer = nullptr;

SDL_Texture *Screen::texture = nullptr;
SDL_Texture *Screen::BGTexture = nullptr;
SDL_Texture *Screen::VRamTexture = nullptr;

void *Screen::pixels = nullptr;
int Screen::pitch = 0;

void *Screen::BGPixels = nullptr;
int Screen::BGPitch = 0;

void *Screen::VramPixels = nullptr;
int Screen::VramPitch = 0;

int scale = 4;
int scaleBG = 2;

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
	SDL_DestroyTexture(VRamTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
    SDL_Quit();
	//std::exit(0); // TODO clean properly
}

void Screen::TexturetoImage(SDL_Texture * Texture) {
    SDL_SetRenderTarget(renderer, Texture);
   	SDL_UnlockTexture(Texture);
    ImGui::Image((void*)(intptr_t)Texture, ImVec2(200*4, 200*4));
//	SDL_RenderCopy(renderer, Texture, NULL, NULL);
    SDL_SetRenderTarget(renderer, nullptr);
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
}

void Screen::clear(ImVec4 vec4)
{
    ImGui::Render();
    SDL_SetRenderDrawColor(Screen::renderer, vec4.x * 255, vec4.y * 255, vec4.z * 255, vec4.w * 255);
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
   	SDL_RenderPresent(Screen::renderer);
}

void	Screen::drawBG()
{
    int bit = (!DBG::bBGMap)? 3 : 6;
	unsigned int BGMap  = BIT(M_LCDC, bit) ? 0x9C00 : 0x9800;
    unsigned int BGDataAddress = BIT(M_LCDC, 4) ? 0x8000 : 0x8800;

	for (int i = 0; i < 32 * 32; i++) {
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
		struct TilePixels tile = TilePixels(addr + (tileIndex * (8 * 2)), BGP);
		int x_offset = (i % 32) * 9;
		int y_offset = (i / 32) * 9;
		for (int y = 0; y < 8; y++) {
			auto line = tile.getColorLine(y);
			for (int x = 0; x < 8; x++) {
				drawPoint(x + x_offset, y + y_offset, line[x], BGPixels, BGPitch, scaleBG);
			}
		}
	}
}

void	Screen::drawVRam()
{
    unsigned int vRamAddress = 0x8000;

	for (int i = 0; i < 256; i++) {
		struct TilePixels tile = TilePixels(vRamAddress + (i * 8 * 2), BGP);
		int x_offset = (i % 16) * 9;
		int y_offset = (i / 16) * 9;
		for (int y = 0; y < 8; y++) {
			auto line = tile.getColorLine(y);
			for (int x = 0; x < 8; x++) {
				drawPoint(x + x_offset, y + y_offset, line[x], VramPixels, VramPitch, scaleBG);
			}
		}
	}

}

bool Screen::drawPpu(int *clockDiff) {
    int clock = *clockDiff;

    Gameboy::setState(GBSTATE_V_BLANK);
    for (int i = 0; i < 10; i++) {
        clock = (Cpu::executeClock(114 - clock) - (114 - clock)); // V-BLANK first as LY=0x90 at start
        Cpu::updateLY(1);
    }
    if (BIT(M_LCDC, 7)) {
        M_LY = 0x00;
    }
    for (int i = 0; i < 144; i++) {
        Gameboy::setState(GBSTATE_OAM_SEARCH);
//        clock = (Cpu::executeClock(20 - clock) - (20 - clock));
        if (BIT(M_LCDC, 7)) {
            Ppu::finalLine = Ppu::doOneLine();
            for (int j = 0; BIT(M_LCDC, 7) && j < PIXEL_PER_LINE; j++) {
                drawPoint(j, i, Ppu::finalLine[j], pixels, pitch);
            }
        }
        Gameboy::setState(GBSTATE_PX_TRANSFERT);
//        clock = (Cpu::executeClock(43 - clock) - (43 - clock));
        Gameboy::setState(GBSTATE_H_BLANK);
        clock = (Cpu::executeClock(114  - clock) - (114 - clock));
        Cpu::updateLY(1);
        /* Drawing time */
    }
    *clockDiff = clock;
    return true;
}

bool	Screen::createTexture() {
	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
		160 * 4,
		144 * 4);
    if (!texture) {
        std::cerr << "Erreur SDL_CreateTexture Ppu : "<< SDL_GetError() << std::endl;
        return false;
    }

	BGTexture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
		160 * 4,
		144 * 4);
    if (!BGTexture) {
        std::cerr << "Erreur SDL_CreateTexture BG : "<< SDL_GetError() << std::endl;
        return false;
    }

    VRamTexture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
		160 * 4,
		144 * 4);
    if (!VRamTexture) {
        std::cerr << "Erreur SDL_CreateTexture VRam : "<< SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

bool	Screen::drawPoint(int x, int y, int color, void *pixels, int pitch, int pixelScale)
{
	x *= pixelScale;
	y *= pixelScale;
	int colorForSDL = 255 - color * (255 / 3);
	int *p = (int*)pixels;
	for (int i = 0 ; i < pixelScale ; i++) {
		for (int j = 0 ; j < pixelScale ; j++) {
			p[(y + j) * (pitch / 4) + (i + x)] = (colorForSDL << 24) | (colorForSDL << 16) | (colorForSDL << 8) | 0xFF;
		}
	}
	return (true);
}

bool	Screen::create()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
        std::cerr <<"Error SDL_Init! "<< SDL_GetError() << std::endl;
		return false;
	}

	auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("GBMU",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			1980,
			1024,
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

	if (!Screen::createTexture())
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

void	Screen::handleEvent(SDL_Event *ev)
{
	if (ev->type == SDL_WINDOWEVENT) {
		switch (ev->window.event) {
			case SDL_WINDOWEVENT_CLOSE:
				Gameboy::quit = true;
				break;
		}
	}
	if (ev->type == SDL_KEYDOWN)
	{
		if (ev->key.keysym.sym == SDLK_ESCAPE)
			Gameboy::quit = true;
	}
}
