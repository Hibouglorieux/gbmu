#ifndef NOISE_HPP
#define NOISE_HPP

#include "Mem.hpp"
#include "Gameboy.hpp"
#include "define.hpp"


class Noise
{
private:
    int channel;

    // NR41
    int length_timer;

    // NR42
    int initial_volume;
    bool envelopeDirection;
    int sweepPace;

    // NR43
    int clockShift;
    bool LFSRwidth;
    int clockDivider;

    // NR44
    bool trigger;
    bool length_enable;

    SDL_AudioSpec obtainedSpec;
    SDL_AudioSpec desiredSpec;


public:
    bool to_trigger;

    void channel_4_tick();

    Noise(int chan);
    ~Noise();
};


#endif