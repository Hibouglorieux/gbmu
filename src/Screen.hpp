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
#include "imgui/imgui.h"
#include <array>
#include "Ppu.hpp"

class Screen {
public:
	static void handleEvent(SDL_Event *ev);
	static bool create(bool bIsCGB);
	static bool drawPoint(int x, int y, int color, void *pixels, int pitch, int pixelScale);
	static void destroy();
    static void	NewframeTexture();
	static void clear(ImVec4 vec4);

	static void updateMainScreen(const std::array<short, PIXEL_PER_LINE>& lineData,
			unsigned char currentLine);
	static void drawBG();
	static void drawVRam(bool bIsCGB);
	static void drawPalettes();
//	static void display_tile(unsigned short location, unsigned short tileNum, int x, int y);

	static bool createTexture(bool bIsCGB);
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

	static int convertColorFromCGB(int colo, bool bConvertForImGUI = false);

    static bool bDisplayWindow;
private:
};

#endif
