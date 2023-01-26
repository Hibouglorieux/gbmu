#include "Noise.hpp"


void Noise::tick() {
    // if (trigger)
    //     return ;

    length_enable = BIT(mem[NR44], 6);

    // TODO implement sweep/envelope
}

void Noise::triggerChannel() {

    to_trigger = false;
    trigger = true;
    length_count = 0;
    length_enable = BIT(mem[NR44], 6);
    length_timer = (mem[NR41] & 0b111111);
    clockShift = (mem[NR43] & 0b11110000) >> 4;
    LFSRwidth = BIT(mem[NR43], 3);
    clockDivider = (mem[NR43] & 0b111);
    current_length_timer = length_timer;
    initial_volume = (mem[NR42] & 0b11110000) >> 4;
    envelopeDirection = BIT(mem[NR42], 3);
    sweepPace = mem[NR42] & 0b111;

    // std::cout << "Channel 4 triggered\n";
    // std::cout << "\tlength enable : " << length_enable << "\n";
    // std::cout << "\tlength timer : " << length_timer << "\n";
    // std::cout << "\tvolume : " << initial_volume << "\n";
    // std::cout << "\tvolume sweep direction : " << envelopeDirection << "\n";
    // std::cout << "\tvolume sweep pace : " << sweepPace << "\n";
}

Noise::Noise(int chan)
: channel(chan), initial_volume(0), trigger(false), to_trigger(false), iterations(0), sample(rand() % 101), length_count(0)
{
    std::cout << "Noise channel " << chan << " was created\n";
    if (chan != 4)
        throw "Wrong channel was specified for Noise channel";
}

Noise::~Noise()
{
}