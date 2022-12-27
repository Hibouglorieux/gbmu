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
	static bool drawPoint(int x, int y, int color, void *pixels, int pitch, int pixelScale = 4);
	static void destroy();
	static void update();
	static void clear();

	static void drawBG();
	static void drawVRam();
	static void display_tile(unsigned short location, unsigned short tileNum, int x, int y);

	static bool createTexture();

	static SDL_Window* get();
	static SDL_Texture*		texture;
	static void*			pixels;
	static int				pitch;

	static SDL_Texture*		BGTexture;
	static void*			BGPixels;
	static int				BGPitch;

	static SDL_Texture*		VRamTexture;
	static void*			VramPixels;
	static int				VramPitch;
private:
	static SDL_Window*		window;
	static SDL_Renderer*	renderer;
	static SDL_Window*		vRamWindow;
	static SDL_Renderer*	vRamRenderer;
	static SDL_Window*		backgroundWindow;
	static SDL_Renderer*	backgroundRenderer;
};

#endif
