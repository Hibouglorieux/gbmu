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
#include <cstdio>
#include "../includes/Screen.hpp"
#include "../includes/Ppu.hpp"
#include "../includes/imgui.h"
#include "../includes//imgui_impl_sdlrenderer.h"
#include <SDL2/SDL.h>
//#include "../includes/define.hpp"
#include "../includes/imgui_impl_sdl.h"


#define QUAD_COLOR 0b11
std::vector<SDL_Point> Screen::pt(16);

SDL_Texture * Screen::VRam_texture = nullptr;
SDL_Texture * Screen::BG_texture = nullptr;
SDL_Texture * Screen::Ppu_texture = nullptr;
SDL_Window* Screen::DBG_win = nullptr;
SDL_Renderer* Screen::DBG_rend = nullptr;

int scale = 4;
int scaleBG = 2;

SDL_Window*	Screen::get(void)
{
	return (Screen::DBG_win);
}

void	Screen::destroy(void)
{
    ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

    SDL_DestroyTexture(VRam_texture);
    SDL_DestroyTexture(Ppu_texture);
    SDL_DestroyTexture(BG_texture);
	SDL_DestroyRenderer(DBG_rend);
	SDL_DestroyWindow(DBG_win);

	SDL_Quit();
	std::exit(0); // TODO clean properly
}

void	Screen::update(void)
{
//	SDL_RenderPresent(renderer);
//	SDL_RenderPresent(vRamRenderer);
//	SDL_RenderPresent(backgroundRenderer);
//    SDL_RenderPresent(DBG_rend);
}

void	Screen::clear(void)
{
//	SDL_SetRenderDrawColor(renderer,0, 0, 0, 255);
//	SDL_SetRenderDrawColor(vRamRenderer,0, 0, 0, 255);
//	SDL_SetRenderDrawColor(backgroundRenderer,0, 0, 0, 255);
//	SDL_RenderClear(renderer);
//	SDL_SetRenderDrawColor(DBG_rend,0, 0, 0, 255);

//	SDL_RenderClear(DBG_rend);
//	SDL_RenderClear(vRamRenderer);
//	SDL_RenderClear(backgroundRenderer);
}

void Draw(struct TilePixels color, int y_max, int x_max, SDL_Renderer *renderer, int index ) {

    int x_offset = (index % y_max) * 9; //WHY 9
    int y_offset = (index / y_max) * 9;
    for (int y = 0; y < x_max; y++) {
        for (int x = 0; x < x_max; x++) {
            Screen::drawPoint(x + x_offset, y + y_offset, color.getColorLine(y)[x], renderer, scale);
        }
    }
}

void	Screen::drawBG()
{
	int BGMap  = BIT(M_LCDC, 3) ? 0x9C00 : 0x9800;
    int BGDataAddress = BIT(M_LCDC, 4) ? 0x8000 : 0x8800;

    SDL_SetRenderTarget(DBG_rend, Screen::BG_texture);
	for (int i = 0; i < 32 * 32; i++) {
		// in order to get the background map displayed we need to fetch the tile to display
		// which is its number (fetched in BGMap which is 32 * 32), then we need
		// to find that data in the VRam, (BGDataAddrress[tileNumber * (size in byte per tile)])
		struct TilePixels tile = TilePixels(BGDataAddress + (mem[BGMap + i] * (8 * 2)), BGP);
        Draw(tile, 32, 8, Screen::DBG_rend, i);
	}
    ImGui::Image((void*)(intptr_t)Screen::BG_texture, ImVec2(1024.0f, 1024.0f));//, uv0, uv1);
    SDL_SetRenderTarget(DBG_rend, nullptr);
}

bool	Screen::drawPpu(int clockDiff, bool updateScreen) {

    SDL_SetRenderTarget(DBG_rend, Screen::Ppu_texture);
//    for (int i = 0; i < 144; i++) {
//        Gameboy::setState(GBSTATE_OAM_SEARCH);
//        clockDiff = (Cpu::executeClock(20 - clockDiff) - (20 - clockDiff));
//        if (BIT(M_LCDC, 7)) {
//				Ppu::finalLine = Ppu::doOneLine(Ppu::finalLine);
//				for (int j = 0 ; BIT(M_LCDC, 7) && j < PIXEL_PER_LINE ; j++) {
//					Screen::drawPoint(j, i, Ppu::finalLine[j], Screen::DBG_rend, 4);
//				}
//				updateScreen = true;
//        }
//        else {
//            updateScreen = false;
//        }
//        Gameboy::setState(GBSTATE_PX_TRANSFERT);
//        clockDiff = (Cpu::executeClock(43 - clockDiff) - (43 - clockDiff));
//        Gameboy::setState(GBSTATE_H_BLANK);
//        clockDiff = (Cpu::executeClock(51 - clockDiff) - (51 - clockDiff));
//        Cpu::updateLY(1);
//        /* Drawing time */
//    }
    Gameboy::setState(GBSTATE_V_BLANK);
    for (int i = 0 ; i < 10 ; i++) {
        clockDiff = (Cpu::executeClock(114 - clockDiff) - (114 - clockDiff)); // V-BLANK first as LY=0x90 at start
        Cpu::updateLY(1);
    }
    if (BIT(M_LCDC, 7)) {
       M_LY = 0x00;
       for (int i = 0 ; i < 144 ; i++) {
           Gameboy::setState(GBSTATE_OAM_SEARCH);
           clockDiff = (Cpu::executeClock(20 - clockDiff) - (20 - clockDiff));
           Ppu::finalLine = Ppu::doOneLine(Ppu::finalLine);
           for (int j = 0 ; BIT(M_LCDC, 7) && j < PIXEL_PER_LINE ; j++) {
               Screen::drawPoint(j, i, Ppu::finalLine[j], DBG_rend);
           }
           updateScreen = true;
           Gameboy::setState(GBSTATE_PX_TRANSFERT);
           clockDiff = (Cpu::executeClock(43 - clockDiff) - (43 - clockDiff));
           Gameboy::setState(GBSTATE_H_BLANK);
           clockDiff = (Cpu::executeClock(51 - clockDiff) - (51 - clockDiff));
           Cpu::updateLY(1);
       }
    } else { updateScreen = false; }
    Ppu::resetWindowCounter();
    ImGui::Image(Screen::Ppu_texture, ImVec2(512.0f, 512.0f));//, uv0, uv1);
    SDL_SetRenderTarget(DBG_rend, nullptr);
    return updateScreen;
}

void	Screen::drawVRam(void)
{
    int vRamAddress = 0x8000;
  
    SDL_SetRenderTarget(DBG_rend, Screen::VRam_texture);
	for (int i = 0; i < 256; i++) {
		struct TilePixels tile = TilePixels(vRamAddress + (i * 8 * 2), BGP);
        Draw(tile, 16, 8, Screen::DBG_rend, i);
	}
    // if (SDL_RenderCopy(DBG_rend, Screen::VRam_texture, nullptr, nullptr)) {
    //     fprintf(stderr, "Erreur SDL_CreateTexture : %s", SDL_GetError());
    //     return;
    // }
    ImGui::Image(Screen::VRam_texture, ImVec2(512.0f, 512.0f));//, uv0, uv1);
    SDL_SetRenderTarget(DBG_rend, nullptr);
}

bool	Screen::drawPoint(int x, int y, int color, SDL_Renderer* targetRenderer, int pixelScale)
{
	int		index = 0;

//	SDL_RendererInfo rendererInfo;
//	SDL_GetRendererInfo(targetRenderer, &rendererInfo);
//	if (x >= rendererInfo.max_texture_width || y >= rendererInfo.max_texture_height
//			|| x < 0 || y < 0) {
//		std::cerr << __func__ << ":" << std::dec << __LINE__ << std::hex << std::endl;
//		return (false);
//	}
	x *= pixelScale;
	y *= pixelScale;
	for (int i = 0 ; i < pixelScale ; i++) {
		for (int j = 0 ; j < pixelScale ; j++) {
			pt[index].x = x + i;
			pt[index].y = y + j;
			index++;
		}
	}
	if (SDL_SetRenderDrawColor(targetRenderer, 255 - color * (255 / 3),
						255 - color * (255 / 3),
						255 - color * (255 / 3) , 255) != 0) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}
	if (SDL_RenderDrawPoints(targetRenderer, pt.data(), pixelScale * pixelScale) != 0) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}
	return (true);
}

bool	Screen::create(void)
{
//	SDL_Init(SDL_INIT_VIDEO);
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// Setup window
	auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	DBG_win = SDL_CreateWindow("Debugger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
//    SDL_SetWindowFullscreen(DBG_win, SDL_WINDOW_FULLSCREEN_DESKTOP);

	// Setup SDL_Renderer instance
    DBG_rend = SDL_CreateRenderer(DBG_win, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (DBG_rend == nullptr)
	{
		SDL_Log("Error creating SDL_Renderer!");
		return -1;
	}

    Screen::Ppu_texture = SDL_CreateTexture(DBG_rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1024, 1024);
  	if (!Screen::Ppu_texture) {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s", SDL_GetError());
        return -1;
    }

  	Screen::VRam_texture = SDL_CreateTexture(DBG_rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1024, 1024);
    if (!Screen::VRam_texture) {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s", SDL_GetError());
        return -1;
    }

    Screen::BG_texture = SDL_CreateTexture(DBG_rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1024 * 2, 1024 * 2);
    if (!Screen::BG_texture) {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s", SDL_GetError());
        return -1;
    }

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
	ImGui_ImplSDL2_InitForSDLRenderer(DBG_win, DBG_rend);
	ImGui_ImplSDLRenderer_Init(DBG_rend);
	return (true);
}

void	Screen::handleEvent(SDL_Event *ev)
{
	if (ev->type == SDL_WINDOWEVENT) {
		switch (ev->window.event) {
			case SDL_WINDOWEVENT_CLOSE:
				Screen::destroy();
				break;
		}
	}
	if (ev->type == SDL_KEYDOWN)
	{
		if (ev->key.keysym.sym == SDLK_ESCAPE)
			Screen::destroy();
	}
}
