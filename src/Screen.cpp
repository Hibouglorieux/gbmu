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

SDL_Window* Screen::DebugWindow = NULL;
SDL_Renderer* Screen::DebugRenderer = NULL;

SDL_Texture* Screen::DebugTexture = NULL;
SDL_Surface* Screen::DebugScreen = NULL;

static unsigned long tile_colors[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};
int scale = 4;

SDL_Window*	Screen::get(void)
{
	return (window);
}

void	Screen::destroy(void)
{
	SDL_DestroyRenderer(DebugRenderer);
	SDL_DestroyWindow(DebugWindow);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	std::exit(0); // TODO clean properly
}

void	Screen::update(void)
{
	SDL_RenderPresent(renderer);
	SDL_RenderPresent(DebugRenderer);
}

void Screen::display_tile(unsigned short location, unsigned short tileNum, int x, int y)
{
	SDL_Rect rect;

	for (int tileY = 0; tileY < 16; tileY += 2) {
		
		unsigned char byte1 = Cpu::getData(location + (tileNum * 16) + tileY);
		unsigned char byte2 = Cpu::getData(location + (tileNum * 16) + tileY + 1);

		for (int bit=7; bit >= 0; bit--) {
            unsigned char hi = !!(byte1 & (1 << bit)) << 1;
            unsigned char lo = !!(byte2 & (1 << bit));

            unsigned char color = hi | lo;

            rect.x = x + ((7 - bit) * scale);
            rect.y = y + (tileY / 2 * scale);
            rect.w = scale;
            rect.h = scale;

            SDL_FillRect(DebugScreen, &rect, tile_colors[color]);
        }
	}
}


void	Screen::updateDebug(void)
{
	int sizePerTile = 2 * 8;
	for (int i = 0; i < 256; i++)
	{
		int tileAddress = 0x8000 + i * sizePerTile;
		for (int y = 0; y < 8; y++)
		{
			std::array<int, 8> tileLine = Ppu::getTilePixels(tileAddress, y, 0xFF47);
			int yOnWindow = (i / 16) * (8 + 1) + y;
			for (int x = 0; x < 8; x++)
			{
				int xOnWindow = (i % (16)) * (8 + 1) + x;
				drawPoint(xOnWindow, yOnWindow, tileLine[x], DebugRenderer);
			}
			drawPoint((i % 16) * (8 + 1) + 8, yOnWindow, QUAD_COLOR, DebugRenderer);
		}
		int yOnWindow = (i / 16) * (8 + 1) + 8;
		for (int x = 0; x < 16 * 9; x++)
			drawPoint(x, yOnWindow, QUAD_COLOR, DebugRenderer);
	}
	/*
	int xDraw = 0;
	int yDraw = 0;
	int tileNum = 0;

	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = DebugScreen->w;
	rect.h = DebugScreen->h;

	SDL_FillRect(DebugScreen, &rect, 0xFF111111);

	unsigned short addr = 0x8000;

	//384 tiles, 24 x 16
    for (int y=0; y<24; y++) {
        for (int x=0; x<16; x++) {
            display_tile(addr, tileNum, xDraw + (x * scale), yDraw + (y * scale));
            xDraw += (8 * scale);
            tileNum++;
        }

        yDraw += (8 * scale);
        xDraw = 0;
    }
	SDL_UpdateTexture(DebugTexture, NULL, DebugScreen->pixels, DebugScreen->pitch);
	SDL_RenderClear(DebugRenderer);
	SDL_RenderCopy(DebugRenderer, DebugTexture, NULL, NULL);
	SDL_RenderPresent(DebugRenderer);
	*/
}

bool	Screen::drawPoint(int x, int y, int color, SDL_Renderer* targetRenderer)
{
	SDL_Point 	pt[16];
	int		index = 0;

	SDL_RendererInfo rendererInfo;
	SDL_GetRendererInfo(targetRenderer, &rendererInfo);
	if (x >= rendererInfo.max_texture_width || y >= rendererInfo.max_texture_height) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}
	x *= 4;
	y *= 4;
	for (int i = 0 ; i < 4 ; i++) {
		for (int j = 0 ; j < 4 ; j++) {
			pt[index].x = x + i;
			pt[index].y = y + j;
			index++;
		}
	}
	if (SDL_SetRenderDrawColor(targetRenderer, color * (255 / 3),  color * (255 / 3) , color * (255 / 3) , 255) != 0) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}
	if (SDL_RenderDrawPoints(targetRenderer, pt, 16) != 0) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}
	return (true);
}

bool	Screen::create(void)
{
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("GBMU",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			160 * 4,
			144 * 4,
			0);

	/*
    DebugScreen = SDL_CreateRGBSurface(0, (16 * 8 * scale) + (16 * scale), 
                                            (32 * 8 * scale) + (64 * scale), 32,
                                            0x00FF0000,
                                            0x0000FF00,
                                            0x000000FF,
                                            0xFF000000);
											*/

	/*
    DebugTexture = SDL_CreateTexture(DebugRenderer,
                                            SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            (16 * 8 * scale) + (16 * scale), 
                                            (32 * 8 * scale) + (64 * scale));
											*/

		
	if (!window) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}


	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}

	DebugWindow = SDL_CreateWindow("VRAM",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			16 * 8 * scale + 16 * scale,
			16 * 8 * scale + 16 * scale,
			0);
	if (!DebugWindow) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}
	if (!DebugWindow) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}

	DebugRenderer = SDL_CreateRenderer(DebugWindow, -1, 0);
	if (!DebugRenderer) {
		std::cerr << __func__ << ":" << __LINE__ << std::endl;
		return (false);
	}
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
}
