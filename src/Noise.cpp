#include "Noise.hpp"


void Noise::tick() {
    // if (trigger)
    //     return ;

    length_timer = (mem[NR41] & 0b111111);
    current_length_timer = length_timer;

    initial_volume = (mem[NR42] & 0b11110000) >> 4;
    envelopeDirection = BIT(mem[NR42], 3);
    sweepPace = mem[NR42] & 0b111;

    clockShift = (mem[NR43] & 0b11110000) >> 4;
    LFSRwidth = BIT(mem[NR43], 3);
    clockDivider = (mem[NR43] & 0b111);

    length_enable = BIT(mem[NR44], 6);

    length_count = 0;


    if (to_trigger && !trigger) {
        std::cout << "Channel 4 triggered\n";

        to_trigger = false;
        trigger = true;
    }

    // TODO implement sweep/envelope
}

Noise::Noise(int chan)
: channel(chan), initial_volume(0), trigger(false), to_trigger(false), iterations(0), sample(rand()), length_count(0)
{
    std::cout << "Noise channel " << chan << " was created\n";
    if (chan != 4)
        throw "Wrong channel was specified for Noise channel";
}

Noise::~Noise()
{
}