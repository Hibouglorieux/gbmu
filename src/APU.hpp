#ifndef APU_HPP
#define APU_HPP

#include <SDL2/SDL_audio.h>
#include <SDL2/SDL.h>
#include "define.hpp"
#include "Gameboy.hpp"

#define SOUND_ON_OFF 0xFF26
#define SOUND_PANNING 0xFF25
#define SOUND_MASTER_VOLUME 0xFF24

// Sound Channel 1 — Pulse with wavelength sweep
#define CHANNEL_1_ON_BIT 0

#define NR10 0xFF10 // FF10 — NR10: Channel 1 sweep
#define NR11 0xFF11 // FF11 — NR11: Channel 1 length timer & duty cycle
#define NR12 0xFF12 // FF12 — NR12: Channel 1 volume & envelope
#define NR13 0xFF13 // FF13 — NR13: Channel 1 wavelength low [write-only]
#define NR14 0xFF14 // FF14 — NR14: Channel 1 wavelength high & control

// Sound Channel 2 — Pulse
#define CHANNEL_2_ON_BIT 1

#define NR21 0xFF16 // FF16 — NR21: Channel 1 length timer & duty cycle
#define NR22 0xFF17 // FF17 — NR22: Channel 1 volume & envelope
#define NR23 0xFF18 // FF18 — NR23: Channel 1 wavelength low [write-only]
#define NR24 0xFF19 // FF19 — NR24: Channel 1 wavelength high & control

// Sound Channel 3 — Wave output
#define CHANNEL_3_ON_BIT 2

#define NR30 0xFF1A // FF1A — NR30: Channel 3 DAC enable
#define NR31 0xFF1B // FF1B — NR31: Channel 3 length timer [write-only]
#define NR32 0xFF1C // FF1C — NR32: Channel 3 output level
#define NR33 0xFF1D // FF1D — NR33: Channel 3 wavelength low [write-only]
#define NR34 0xFF1E // FF1E — NR34: Channel 3 wavelength high & control
#define CHANNEL_3_WAVE_PATTERN 0xFF30 // FF30–FF3F — Wave pattern RAM

// Sound Channel 4 — Noise
#define CHANNEL_4_ON_BIT 3

#define NR41 0xFF20 // FF20 — NR41: Channel 4 length timer [write-only]
#define NR42 0xFF21 // FF21 — NR42: Channel 4 volume & envelope
#define NR43 0xFF22 // FF22 — NR43: Channel 4 frequency & randomness
#define NR44 0xFF23 // FF23 — NR44: Channel 4 control

class APU
{
private:
    static unsigned char triggers[4];
public:
    static void tick();

    static void checkTrigger(int channel);
    static void trigger(int channel);

    static void triggerChannel1();
    static void triggerChannel2();
    static void triggerChannel3();
    static void triggerChannel4();


    APU(/* args */);
    ~APU();
};

void audio_callback(void *_beeper, Uint8 *_stream, int _length);

#endif