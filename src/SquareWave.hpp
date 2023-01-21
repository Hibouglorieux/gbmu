#ifndef SQUARE_WAVE_HPP
#define SQUARE_WAVE_HPP

#include "Mem.hpp"
#include "Gameboy.hpp"
#include "define.hpp"

class SquareWave
{
public:
    int channel;

    int step;

    // NR10 (only channel 1)
    int waveSweepPace;
    bool waveSweepDirection;
    int waveSweepSlope;

    // NR*1
    int waveDuty;
    int length_timer;
    int current_length_timer;

    // NR*2
    int initialVolume;
    int envelopeDirection;
    int volumeSweepPace;

    // NR*3
    // int waveLow;
    
    // NR*4
    bool trigger;
    int lengthEnable;
    // int waveHigh;

    int waveLength;
    char wave;

    int volumeSweepValue;
    int wavelengthSweepValue;

    int ticks;

    bool to_trigger;

    int iterations;

    SDL_AudioSpec obtainedSpec;
    SDL_AudioSpec desiredSpec;

    void tick();
    void channel_2_tick();
    void channel_1_tick();

    const static int samples_per_length = SAMPLING_RATE/64;

    int length_count;

    SquareWave(int chan);
    ~SquareWave();
};

#endif