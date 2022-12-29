
#ifndef JOYPAD_CLASS_H
# define JOYPAD_CLASS_H

#include <SDL2/SDL.h>
//#include "define.hpp"

class Joypad {
public:
	static void handleEvent(SDL_Event *ev);
	static unsigned char get();
	static void refresh();
private:
	static unsigned char input;
};

#endif
