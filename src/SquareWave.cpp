#include "SquareWave.hpp"

void SquareWave::tick() {

    switch (channel)
    {
    case 1:
        channel_1_tick();
        break;
    case 2:
        channel_2_tick();
        break;
    default:
        throw "Trying to tick wrong SquareWave channel";
    }
}

void SquareWave::channel_2_tick() {
    // if (trigger)
    //     return;

    lengthEnable = BIT(mem[NR24], 6);
    waveLength = ((mem[NR24] & 0b111) << 8) | mem[NR23];
    waveDuty = (mem[NR21] & 0b11000000) >> 6;
    length_timer = (mem[NR21] & 0b00111111);
    initialVolume = (mem[NR22] & 0b11110000) >> 4;
    envelopeDirection = BIT(mem[NR22], 3);
    volumeSweepPace = (mem[NR22] & 0b00000111);
    // std::cout << "CHANNEL 2 TICK : " << lengthEnable << " et " << volumeSweepPace << " avec " << envelopeDirection << "\n";

    
    if (to_trigger && !trigger) {
        std::cout << "Channel2 triggered : " << lengthEnable << "\n";

        volumeSweepValue = 0;
        wavelengthSweepValue = 0;
        current_length_timer = length_timer;
        to_trigger = false;
        trigger = true;
    }

    switch (waveDuty)
    {
    case 0:
        wave = 0b00000001;
        break;
    case 1:
        wave = 0b10000001;
        break;
    case 2:
        wave = 0b10000111;
        break;
    case 3:
        wave = 0b11100111;
        break;
    
    default:
        throw "Wrong waveDuty specified for SquareWave";
    }

    // TODO implement sweep/envelope
}


void SquareWave::channel_1_tick() {
    // if (trigger)
    //     return ;

    waveSweepDirection = BIT(mem[NR10], 3);
    waveSweepPace = (mem[NR10] & 0b01110000);
    waveSweepSlope = (mem[NR10] & 0b00000111);
    waveDuty = (mem[NR11] & 0b11000000) >> 6;
    length_timer = (mem[NR11] & 0b00111111);
    initialVolume = (mem[NR12] & 0b11110000) >> 4;
    envelopeDirection = BIT(mem[NR12], 3);
    volumeSweepPace = (mem[NR12] & 0b00000111);
    waveLength = ((mem[NR14] & 0b111) << 8) | mem[NR13];
    lengthEnable = BIT(mem[NR14], 6);
    volumeSweepValue = 0;
    
    if (to_trigger && !trigger) {
        std::cout << "Channel1 triggered\n";

        wavelengthSweepValue = 0;
        current_length_timer = length_timer;
        to_trigger = false;
        trigger = true;
    }


    switch (waveDuty)
    {
    case 0:
        wave = 0b00000001;
        break;
    case 1:
        wave = 0b10000001;
        break;
    case 2:
        wave = 0b10000111;
        break;
    case 3:
        wave = 0b11100111;
        break;
    
    default:
        throw "Wrong waveDuty specified for SquareWave";
    }

    // TODO implement sweep/envelope
}

SquareWave::SquareWave(int chan)
: channel(chan), step(0), initialVolume(0), trigger(false), to_trigger(false), iterations(0), length_count(0)
{
    std::cout << "SquareWave channel " << chan << " was created\n";
    if (chan != 1 && chan != 2)
        throw "Wrong channel was specified for SquareWave channel";
    ticks = 0;
}

SquareWave::~SquareWave()
{
}
