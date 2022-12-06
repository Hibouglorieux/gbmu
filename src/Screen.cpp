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

#include "Screen.hpp"

Screen::Screen()
{
	internalArray = new unsigned char[160 * 144];
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("GBMU",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			160 * 4,
			144 * 4,
			0);
}

Screen::~Screen()
{
	delete[] internalArray;
}

unsigned char& Screen::operator[](int i)
{
	return internalArray[i];
}

// TODO We need to have 8 bit for each input
// so we cannot write into memory directly, need a temp buffer,
// we need to update everytime 0xFF00 bit 0-3 with respect to bit 4-5
// the update of bit 4-5 is done by sw ROM
// so, at every write to bit 4 or 5, cpy bit 0-3 accordingly
// This function should just update the temp buffer and must be run asynchronous (thread ?)
void	Screen::inputWatcher()
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
			case SDL_KEYDOWN: { // Key press
				switch (ev.key.keysym.sym) {
					case SDLK_DOWN:
						break;
					case SDLK_UP:
						break;
					case SDLK_LEFT:
						break;
					case SDLK_RIGHT:
						break;
					case SDLK_KP_0: // A
						break;
					case SDLK_KP_3: // B
						break;
					case SDLK_KP_1: // Start
						break;
					case SDLK_KP_7: // Select
						break;
					case SDLK_KP_8:
						break;
				}
				break;
			}
			default:
				break;
		}
	}
}
