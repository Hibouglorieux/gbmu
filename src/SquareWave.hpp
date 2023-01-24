#ifndef SQUARE_WAVE_HPP
#define SQUARE_WAVE_HPP

#include "Mem.hpp"
#include "Gameboy.hpp"
#include "define.hpp"
#include <queue>

typedef struct {
    int length_timer;
    int volumeSweepPace;
    int initialVolume;
    int envelopeDirection;
    int waveLength;
    int waveDuty;
    int length_enable;
    int waveSweepDirection;
    int waveSweepPace;
    int waveSweepSlope;
    
} entry;

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

    float waveLength;
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

    void changeWavelength(float val);
    void triggerChannel();
    void popEntry(entry val);

    const static int samples_per_length = SAMPLING_RATE/64;

    int length_count;
    int volume;
    int volumeReduction;

    std::queue<entry> queue;

    SquareWave(int chan);
    ~SquareWave();
};

#endif