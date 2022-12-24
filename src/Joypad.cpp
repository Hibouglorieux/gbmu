#include "Joypad.hpp"
#include "Gameboy.hpp"
#include <iostream>

// TODO We need to have 8 bit for each input
// so we cannot write into memory directly, need a temp buffer,
// we need to update everytime 0xFF00 bit 0-3 with respect to bit 4-5
// the update of bit 4-5 is done by sw ROM
// so, at every write to bit 4 or 5, cpy bit 0-3 accordingly

unsigned char Joypad::input = 0;

unsigned char Joypad::get()
{
	return (input);
}

void Joypad::handleEvent(SDL_Event *ev)
{
	if (ev->type == SDL_KEYUP) {
		switch (ev->key.keysym.sym) {
			case SDLK_DOWN:
				input |= JOYPAD_MSK_DOWN;
				break;
			case SDLK_UP:
				input |= JOYPAD_MSK_UP;
				break;
			case SDLK_LEFT:
				input |= JOYPAD_MSK_LEFT;
				break;
			case SDLK_RIGHT:
				input |= JOYPAD_MSK_RIGHT;
				break;
			case SDLK_KP_0: // A
				input |= JOYPAD_MSK_A;
				break;
			case SDLK_KP_3: // B
				input |= JOYPAD_MSK_B;
				break;
			case SDLK_KP_1: // Start
				input |= JOYPAD_MSK_START;
				break;
			case SDLK_KP_7: // Select
				input |= JOYPAD_MSK_SELECT;
				break;
			default:
				break;
		}
	}
	if (ev->type == SDL_KEYDOWN) {
		switch (ev->key.keysym.sym) {
			case SDLK_DOWN:
				input &= ~JOYPAD_MSK_DOWN;
				break;
			case SDLK_UP:
				input &= ~JOYPAD_MSK_UP;
				break;
			case SDLK_LEFT:
				input &= ~JOYPAD_MSK_LEFT;
				break;
			case SDLK_RIGHT:
				input &= ~JOYPAD_MSK_RIGHT;
				break;
			case SDLK_KP_0: // A
				input &= ~JOYPAD_MSK_A;
				break;
			case SDLK_KP_3: // B
				input &= ~JOYPAD_MSK_B;
				break;
			case SDLK_KP_1: // Start
				input &= ~JOYPAD_MSK_START;
				break;
			case SDLK_KP_7: // Select
				input &= ~JOYPAD_MSK_SELECT;
				break;
			default:
				break;
		}
	}
	if ((mem[0xFF00] & (1 << 5))) {
		// Action button (priority TODO unsure)
		mem.supervisorWrite(0xFF00, mem[0xFF00] | ((input >> 4) & 0x0F));
	}
	else if ((mem[0xFF00] & (1 << 4))) {
		// Direction buttons
		mem.supervisorWrite(0xFF00, mem[0xFF00] | ((input >> 4) & 0x0F));
	}
}
