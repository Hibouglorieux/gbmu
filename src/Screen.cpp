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

#define QUAD_COLOR 0b11

SDL_Window* Screen::window = NULL;
SDL_Renderer* Screen::renderer = NULL;

SDL_Window* Screen::vRamWindow = NULL;
SDL_Renderer* Screen::vRamRenderer = NULL;

SDL_Window* Screen::backgroundWindow = NULL;
SDL_Renderer* Screen::backgroundRenderer = NULL;

SDL_Texture *Screen::texture = NULL;
SDL_Texture *Screen::BGTexture = NULL;
SDL_Texture *Screen::VRamTexture = NULL;

void *Screen::pixels = NULL;
int Screen::pitch = 0;

void *Screen::BGPixels = NULL;
int Screen::BGPitch = 0;

void *Screen::VramPixels = NULL;
int Screen::VramPitch = 0;

int scale = 4;
int scaleBG = 2;

SDL_Window*	Screen::get(void)
{
	return (window);
}

void	Screen::destroy(void)
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(vRamRenderer);
	SDL_DestroyWindow(vRamWindow);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(backgroundRenderer);
	SDL_DestroyWindow(backgroundWindow);
	SDL_Quit();
	// Cpu::printFIFO(Cpu::fifo);
	std::exit(0); // TODO clean properly
}

void	Screen::update(void)
{
	SDL_UnlockTexture(texture);
	SDL_UnlockTexture(BGTexture);
	SDL_UnlockTexture(VRamTexture);

	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderCopy(vRamRenderer, VRamTexture, NULL, NULL);
	SDL_RenderCopy(backgroundRenderer, BGTexture, NULL, NULL);

	SDL_RenderPresent(renderer);
	SDL_RenderPresent(vRamRenderer);
	SDL_RenderPresent(backgroundRenderer);
}

void	Screen::clear(void)
{
	SDL_SetRenderDrawColor(renderer,0, 0, 0, 255);
	SDL_SetRenderDrawColor(vRamRenderer,0, 0, 0, 255);
	SDL_SetRenderDrawColor(backgroundRenderer,0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderClear(vRamRenderer);
	SDL_RenderClear(backgroundRenderer);
}

void	Screen::drawBG()
{
	unsigned int BGMap  = BIT(M_LCDC, 3) ? 0x9C00 : 0x9800;
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

void	Screen::drawVRam(void)
{
    unsigned int vRamAddress = 0x8000;

	for (int i = 0; i < 256; i++) {
		struct TilePixels tile = TilePixels(vRamAddress + (i * 8 * 2), BGP);
		int x_offset = (i % 16) * 9;
		int y_offset = (i / 16) * 9;
		for (int y = 0; y < 8; y++) {
			auto line = tile.getColorLine(y);
			for (int x = 0; x < 8; x++) {
				drawPoint(x + x_offset, y + y_offset, line[x], VramPixels, VramPitch);
			}
		}
		
	}
}

bool	Screen::createTexture() {
	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING, 
		160 * 4,
		144 * 4);
	BGTexture = SDL_CreateTexture(backgroundRenderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING, 
		160 * 4,
		144 * 4);
	VRamTexture = SDL_CreateTexture(vRamRenderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING, 
		160 * 4,
		144 * 4);
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

bool	Screen::create(void)
{
	SDL_Init(SDL_INIT_VIDEO);

	vRamWindow = SDL_CreateWindow("VRAM",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			16 * 8 * scale + 16 * scale,
			16 * 8 * scale + 16 * scale,
			0);

	if (!vRamWindow) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}
	vRamRenderer = SDL_CreateRenderer(vRamWindow, -1, 0);
	if (!vRamRenderer) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}

	backgroundWindow = SDL_CreateWindow("BGMap",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			(32) * 8 * scaleBG + 32 * scaleBG,
			(32) * 8 * scaleBG + 32 * 2 * scaleBG,
			0);

	if (!backgroundWindow) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}
	backgroundRenderer = SDL_CreateRenderer(backgroundWindow, -1, 0);
	if (!backgroundRenderer) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}

	window = SDL_CreateWindow("GBMU",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			160 * 4,
			144 * 4,
			0);

	if (!window) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}


	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}

	SDL_RenderClear(renderer);
	// if (SDL_SetRenderDrawColor(renderer, 33,  200 , 33 , 255) != 0) {
	// 	std::cerr << __func__ << ":" << __LINE__ << std::endl;
	// 	return (false);
	// }
	// if (SDL_RenderClear(renderer) != 0) {
	// 	std::cerr << __func__ << ":" << __LINE__ << std::endl;
	// 	return (false);
	// }
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
