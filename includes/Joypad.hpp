
#ifndef JOYPAD_CLASS_H
# define JOYPAD_CLASS_H

#include <SDL2/SDL.h>

# define JOYPAD_MSK_START	0x80
# define JOYPAD_MSK_SELECT 	0x40
# define JOYPAD_MSK_B 		0x20
# define JOYPAD_MSK_A 		0x10
# define JOYPAD_MSK_DOWN	0x08
# define JOYPAD_MSK_UP 		0x04
# define JOYPAD_MSK_LEFT 	0x02
# define JOYPAD_MSK_RIGHT 	0x01

class Joypad {
public:
	static void handleEvent(SDL_Event *ev);
	static unsigned char get();
	static void refresh();
private:
	static unsigned char input;
};

#endif
