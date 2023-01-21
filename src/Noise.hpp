#ifndef NOISE_HPP
#define NOISE_HPP

#include "Mem.hpp"
#include "Gameboy.hpp"
#include "define.hpp"


class Noise
{
public:
    int channel;

    // NR41
    int length_timer;

    int current_length_timer;

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


    bool to_trigger;
    int iterations;

    int sample;

    void tick();

    const static int samples_per_length = SAMPLING_RATE/256;

    int length_count;

    Noise(int chan);
    ~Noise();
};


#endif