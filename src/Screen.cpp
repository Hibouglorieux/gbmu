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

int scale = 4;
int scaleBG = 2;

SDL_Window*	Screen::get(void)
{
	return (window);
}

void	Screen::destroy(void)
{
	SDL_DestroyRenderer(vRamRenderer);
	SDL_DestroyWindow(vRamWindow);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(backgroundRenderer);
	SDL_DestroyWindow(backgroundWindow);
	SDL_Quit();
	std::exit(0); // TODO clean properly
}

void	Screen::update(void)
{
// if (SDL_SetRenderDrawColor(renderer, 0, 
	// 					0, 
	// 					0 , 255) != 0) {
	// 	std::cerr << __func__ << ":" << __LINE__ << std::endl;
	// 	return ;
	// }
	SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(vRamRenderer);
	SDL_RenderPresent(backgroundRenderer);
}

void	Screen::updateBG()
{
	for (int y = 0; y < 32; y++)
	{
		for (int tmpX = 0; tmpX < 32 * 8 * scaleBG + 32 * scaleBG; tmpX++)
		{
			if (y)
				drawPoint(tmpX, y * (8 + 1) - 1,
						QUAD_COLOR, backgroundRenderer, scaleBG);
		}
		for (int x = 0; x < 32; x++)
		{
			for (int yy = 0; yy < 8; yy++)
			{
				int yOnWindow = y * (8 + 1) + yy;
				if (x)
					drawPoint(x * (8 + 1) - 1, yOnWindow,
							QUAD_COLOR, backgroundRenderer, scaleBG);

				std::array<int, 8> backgroundLine = Ppu::getBackgroundTile(x, y, yy);
				for (int xx = 0; xx < 8; xx++)
				{
					int xOnWindow = x * (8 + 1) + xx;
					drawPoint(xOnWindow, yOnWindow,
							backgroundLine[xx], backgroundRenderer, scaleBG);
				}
			}
		}
	}
}

void	Screen::updateVRam(void)
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
				drawPoint(xOnWindow, yOnWindow, tileLine[x], vRamRenderer);
			}
			drawPoint((i % 16) * (8 + 1) + 8, yOnWindow, QUAD_COLOR, vRamRenderer);
		}
		int yOnWindow = (i / 16) * (8 + 1) + 8;
		for (int x = 0; x < 16 * 9; x++)
			drawPoint(x, yOnWindow, QUAD_COLOR, vRamRenderer);
	}
}

bool	Screen::drawPoint(int x, int y, int color, SDL_Renderer* targetRenderer, int pixelScale)
{
	std::vector<SDL_Point> pt(pixelScale * pixelScale);
	int		index = 0;

	SDL_RendererInfo rendererInfo;
	SDL_GetRendererInfo(targetRenderer, &rendererInfo);
	if (x >= rendererInfo.max_texture_width || y >= rendererInfo.max_texture_height
			|| x < 0 || y < 0) {
		std::cerr << __func__ << ":" << std::dec << __LINE__ << std::hex << std::endl;
		return (false);
	}
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
	SDL_Init(SDL_INIT_VIDEO);
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
			(32) * 8 * scaleBG + 32 * scaleBG,
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
}
