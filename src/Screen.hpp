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

#ifndef SCREEN_CLASS_H
# define SCREEN_CLASS_H

#include <SDL2/SDL.h>

class Screen {
public:
	static void handleEvent(SDL_Event *ev);
	static bool create();
	static bool drawPoint(int x, int y, int color, SDL_Renderer* targetRenderer = renderer, int pixelScale = 4);
	static void destroy();
	static void update();

	static void updateBG();
	static void updateVRam();
	static void display_tile(unsigned short location, unsigned short tileNum, int x, int y);

	static SDL_Window* get();
private:
	static SDL_Window*		window;
	static SDL_Renderer*	renderer;
	static SDL_Window*		vRamWindow;
	static SDL_Renderer*	vRamRenderer;
	static SDL_Window*		backgroundWindow;
	static SDL_Renderer*	backgroundRenderer;
};

#endif
