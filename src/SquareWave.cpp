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

    waveLength = ((mem[NR24] & 0b111) << 8) | mem[NR23];
    waveDuty = (mem[NR21] & 0b11000000) >> 6;
    lengthEnable = BIT(mem[NR24], 6);
    // std::cout << "CHANNEL 2 TICK : " << lengthEnable << " et " << volumeSweepPace << " avec " << envelopeDirection << "\n";

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

void SquareWave::triggerChannel() {

    if (channel == 2) {
        length_timer = (mem[NR21] & 0b00111111);
        volumeSweepPace = (mem[NR22] & 0b00000111);
        initialVolume = (mem[NR22] & 0b11110000) >> 4;
        envelopeDirection = BIT(mem[NR22], 3);
        waveLength = ((mem[NR24] & 0b111) << 8) | mem[NR23];
        waveDuty = (mem[NR21] & 0b11000000) >> 6;
        

    }
    else {
        length_timer = (mem[NR11] & 0b00111111);
        volumeSweepPace = (mem[NR12] & 0b00000111);
        initialVolume = (mem[NR12] & 0b11110000) >> 4;
        envelopeDirection = BIT(mem[NR12], 3);
        waveLength = ((mem[NR14] & 0b111) << 8) | mem[NR13];
        waveDuty = (mem[NR11] & 0b11000000) >> 6;

        waveSweepDirection = BIT(mem[NR10], 3);
        waveSweepPace = (mem[NR10] & 0b01110000);
        waveSweepSlope = (mem[NR10] & 0b00000111);

        std::cout << "Channel " << channel << " triggered : " << waveLength << "\n";
        std::cout << "\tlength enable : " << lengthEnable << "\n";
        std::cout << "\tlength timer : " << length_timer << "\n";
        std::cout << "\tvolume : " << initialVolume << "\n";
        std::cout << "\tvolume sweep pace : " << volumeSweepPace << "\n";
        // std::cout << "\tvolume sweep direction : " << envelopeDirection << "\n";
    }
    
    volume = initialVolume;
    volumeSweepValue = 0;
    wavelengthSweepValue = 0;
    current_length_timer = length_timer;
    to_trigger = false;
    trigger = true;
}


void SquareWave::channel_1_tick() {
    // if (trigger)
    //     return ;

    
    // length_timer = (mem[NR11] & 0b00111111);
    // initialVolume = (mem[NR12] & 0b11110000) >> 4;
    // envelopeDirection = BIT(mem[NR12], 3);
    // volumeSweepPace = (mem[NR12] & 0b00000111);
    lengthEnable = BIT(mem[NR14], 6);


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

void SquareWave::changeWavelength(float val) {
    waveLength = val;
    if (channel == 1) {
        mem[NR13] = ((int)val & 0xFF);
        mem[NR14] |= ((int)val >> 8) & 0b111;
    } else {
        mem[NR23] = ((int)val & 0xFF);
        mem[NR24] |= ((int)val >> 8) & 0b111;
    }
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
