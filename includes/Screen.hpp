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
#include "imgui.h"

class Screen {
public:
	static void handleEvent(SDL_Event *ev);
	static bool create();
	static bool drawPoint(int x, int y, int color, void *pixels, int pitch, int pixelScale = 4);
	static void destroy();
    static void	NewframeTexture();
	static void clear(ImVec4 vec4);

	static void drawBG();
	static void drawVRam();
    static bool drawPpu(int *clockDiff);
//	static void display_tile(unsigned short location, unsigned short tileNum, int x, int y);

	static bool createTexture();
    static void TexturetoImage(SDL_Texture *);

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
	static SDL_Window*		window;
	static SDL_Renderer*	renderer;
private:
};

#endif
