#include "APU.hpp"

unsigned char APU::triggers[4] = {0,0,0,0};

int old_1 = 0;
int old_2 = 0;
int old_3 = 0;
int old_4 = 0;

void APU::tick() {
    // int val_1 = mem[CHANNEL_1_WAVE_HIGH];
    // int val_2 = mem[CHANNEL_2_WAVE_HIGH];
    // int val_3 = mem[CHANNEL_3_WAVE_HIGH];
    // int val_4 = mem[CHANNEL_4_CONTROL];

    // if (BIT(val_1, 7) && !BIT(old_1, 7))
    //     std::cout << "Trigger channel 1\n";
    // if (BIT(val_2, 7) && !BIT(old_2, 7))
    //     std::cout << "Trigger channel 2\n";
    // if (BIT(val_3, 7) && !BIT(old_3, 7))
    //     std::cout << "Trigger channel 3\n";
    // if (BIT(val_4, 7) && !BIT(old_4, 7))
    //     std::cout << "Trigger channel 4\n";

    // std::cout << "Audio : " << std::hex << (int)val_1 << " - " << val_2 << " - " << val_3 << " - " << val_4 << "\n";

    // old_1 = val_1;
    // old_2 = val_2;
    // old_3 = val_3;
    // old_4 = val_4;


    // std::cout << "CHANNEL_4_VOLUME = " << std::dec << (int)mem[CHANNEL_4_VOLUME] << "\n";

}

void APU::checkTrigger(int channel) {
    int currentValue = 0;

    switch (channel)
    {
    case 1:
        currentValue = BIT(mem[NR14], 7);
        break;
    case 2:
        currentValue = BIT(mem[NR24], 7);
        break;
    case 3:
        currentValue = BIT(mem[NR34], 7);
        break;
    case 4:
        currentValue = BIT(mem[NR44], 7);
        break;
    
    default:
        throw "Checking trigger on wrong audio channel";
    }

    if (currentValue && !triggers[channel - 1])
        trigger(channel);
}

void APU::trigger(int channel) {
    std::cout << "Triggering channel " << channel << "\n";
    switch (channel)
    {
    case 1:
        triggerChannel1();
        break;
    case 2:
        triggerChannel2();
        break;
    case 3:
        triggerChannel3();
        break;
    case 4:
        triggerChannel4();
        break;
    default:
        throw "Triggering wrong audio channel";
    }
}

void APU::triggerChannel1() {
    
}

void APU::triggerChannel2() {
    
}

void APU::triggerChannel3() {

}

void APU::triggerChannel4() {
    int length = (mem[NR41] & 0b111111);
    int volume = (mem[NR42] & 0b11110000) >> 4;
    bool expireToLength = BIT(mem[NR44], 6);

    std::cout << "\tChannel 4 length : " << std::dec << length << " avec " << (bool)expireToLength << "\n";
    SDL_AudioSpec desiredSpec;

    desiredSpec.freq = 44100;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 2048;
    desiredSpec.callback = audio_callback;
    desiredSpec.userdata = NULL;

    SDL_AudioSpec obtainedSpec;

    // you might want to look for errors here
    SDL_OpenAudio(&desiredSpec, &obtainedSpec);

    // start play audio
    SDL_PauseAudio(0);
}

APU::APU(/* args */)
{
    SDL_Init(SDL_INIT_AUDIO);
}

APU::~APU()
{
}

void audio_callback(void *_beeper, Uint8 *_stream, int _length) {
    int length = (mem[NR41] & 0b111111);
    int volume = (mem[NR42] & 0b11110000) >> 4;
}