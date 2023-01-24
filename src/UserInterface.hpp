/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserInterface.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 22:45:13 by lmariott          #+#    #+#             */
/*   Updated: 2023/01/05 16:13:17 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_INTERFACE_CLASS_H
# define USER_INTERFACE_CLASS_H

/*
** All user interface stuff
** We use ImgUI
** The input for joypad is handle by Joypad
** The sub window, including PPU, are in Screen
** This module contain the main loop function
*/

#include <SDL2/SDL.h>
#include "imgui/imgui.h"
#include <array>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

class UserInterface
{
public:
	static bool 		loop();
	static void 		showGameboyWindow();
	static void 		showSubWindows();
	static bool		create();
	static void		destroy();
	static SDL_Window*	uiWindow;
	static SDL_Renderer*	uiRenderer;
    	static void		TexturetoImage(SDL_Texture *);
    	static void		fileExplorer();
	static void		newFrame();
	static void		clear(ImVec4 vec4);
	static void		handleEvent(SDL_Event *ev);

	static void		errorWindow();
	static void		throwError(const char *, bool);
	static bool		bIsError;
	static bool		bIsFatalError;
	static std::string	errMsg;

private:
	static std::string	romFolderPath;
	static bool		showVram;
	static bool		showBG;
	static bool		showSprite;
	static bool		showPalettes;
	static bool		showHexdump;
	static bool		showRegisters;
	static bool		shouldStop;
	static double		frameTime;
	static double		fpsRefreshTime;
	static unsigned char	frameCount;
	const static double	refreshingRate;
};

#endif
