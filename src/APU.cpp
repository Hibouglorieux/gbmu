#include "APU.hpp"

unsigned char APU::triggers[4] = {0,0,0,0};
int APU::ticks = 0;
SquareWave APU::channel1 = SquareWave(1);
SquareWave APU::channel2 = SquareWave(2);
Waveform APU::channel3 = Waveform(3);
Noise APU::channel4 = Noise(4);

SDL_AudioSpec APU::obtainedSpec;
SDL_AudioSpec APU::desiredSpec;

void addDecibel(unsigned short &ref, int val) {
    int tmp = ref;

    tmp += val;
    // if (tmp > MAX_VOLUME)
    //     tmp = MAX_VOLUME;
    // else if (tmp < 0)
    //     tmp = 0;
    ref = (unsigned short)tmp;
}

void APU::init() {
    SDL_Init(SDL_INIT_AUDIO);
    std::cout << "INIT AUDIO\n";

    desiredSpec.freq = SAMPLING_RATE;
    desiredSpec.format = AUDIO_U16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 2048;
    desiredSpec.callback = APU::sound_callback;
    desiredSpec.userdata = NULL;

    // you might want to look for errors here
    SDL_OpenAudio(&desiredSpec, &obtainedSpec);

    // start play audio
    SDL_PauseAudio(0);
}

void APU::tick(int n) {
    ticks += n;
    if (ticks >= 190) {
        channel2.tick();
        channel1.tick();
        channel3.tick();
        channel4.tick();
    }
    ticks %= 190;
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
    triggers[channel - 1] = currentValue;
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
    
}

APU::APU(/* args */)
{
}

APU::~APU()
{
}

void APU::sound_callback(void *arg, Uint8 *stream, int length) {

    // TODO verify for ptr[i] overflow
    uint16_t *ptr = (uint16_t *)stream;
    // std::cout << "Sound callback\n";

    if (!BIT(mem[0xFF26], 7)) {
        memset(stream, 0, length);
        return ;
    }

    for (int i = 0; i < length/2; i++) {
        ptr[i] = 0;

        // if (channel1.trigger && BIT(mem[0xFF26], 0) && channel1.waveLength <= 0x7FF)
        // {
        //     // Channel 1
        //     int freq = 1048576/(2048 - (int)(channel1.waveLength));
        //     int samples_per_step = SAMPLING_RATE / freq;

        //     float val = (float)(MAX_VOLUME * channel1.initialVolume)/0xF;

        //     addDecibel(ptr[i], BIT(channel1.wave, channel1.step) * (int)val);
        //     channel1.iterations++;
        //     channel1.length_count++;

        //     if (channel1.iterations >= samples_per_step) {
        //         channel1.step = (channel1.step + 1) % 8;
        //         channel1.iterations = 0;
        //     }

        //     if (channel1.length_count >= SquareWave::samples_per_length) {
        //         // Each 64 Hz
        
        //         if (channel1.waveSweepPace && !(ticks % channel1.waveSweepPace)) {

        //             std::cout << "wavelength 1 : " << channel1.waveLength << " avec " << (channel1.waveLength / pow(2, channel1.waveSweepSlope)) << "\n"; 
        //             if (channel1.waveSweepDirection)
        //                 // channel1.changeWavelength(channel1.waveLength);
        //                 channel1.changeWavelength(channel1.waveLength + (channel1.waveLength / pow(2, channel1.waveSweepSlope)));
        //             else
        //                 // channel1.changeWavelength(channel1.waveLength);
        //                 channel1.changeWavelength(channel1.waveLength - (channel1.waveLength / pow(2, channel1.waveSweepSlope)));
        //             std::cout << "wavelength 2 : " << channel1.waveLength << "\n";
        //         }

        //         if (channel1.volumeSweepPace && !(ticks % channel1.volumeSweepPace)) {
        //             std::cout << "volume 1 : " << channel1.initialVolume << "\n";
        //             if (channel1.envelopeDirection) {
        //                 // channel1.volumeSweepValue++;
        //                 channel1.initialVolume++;
        //                 if (channel1.initialVolume > 15)
        //                     channel1.initialVolume = 15;
                        
        //             }
        //             else {
        //                 // channel1.volumeSweepValue--;

        //                 channel1.initialVolume--;
        //                 if (channel1.initialVolume < 0)
        //                     channel1.initialVolume = 0;

        //             }
        //             std::cout << "volume 1 : " << channel1.initialVolume << "\n";
        //         }



        //         if (channel1.lengthEnable) {
        //             channel1.current_length_timer--;
                    
        //             if (!channel1.current_length_timer)
        //                 channel1.trigger = false;
        //         }
        //         channel1.length_count = 0;
        //         ticks++;
        //     }
        // }

        if (channel2.trigger && BIT(mem[0xFF26], 1))
        {
            // Channel 2
            int freq = 1048576/(2048 - channel2.waveLength);
            int samples_per_step = SAMPLING_RATE / freq;

            float val = (float)(MAX_VOLUME * channel2.initialVolume)/0xF + (float)(channel2.volumeSweepValue * 0xFF);

            addDecibel(ptr[i], BIT(channel2.wave, channel2.step) * (int)val);
            channel2.iterations++;
            channel2.length_count++;

            if (channel2.iterations >= samples_per_step) {
                channel2.step = (channel2.step + 1) % 8;
                channel2.iterations = 0;
            }

            if (channel2.length_count >= SquareWave::samples_per_length) {
                // Each 64 Hz

                if (channel2.volumeSweepPace && !(ticks % channel2.volumeSweepPace)) {
                    if (channel2.envelopeDirection) {
                        channel2.initialVolume++;
                        if (channel2.initialVolume > 0xF)
                            channel2.initialVolume = 0xF;

                    }
                    else {
                        channel2.initialVolume--;
                        if (channel2.initialVolume < 0)
                            channel2.initialVolume = 0;
                    }
                }

                if (channel2.lengthEnable) {
                    channel2.current_length_timer--;
                    
                    if (!channel2.current_length_timer)
                        channel2.trigger = false;
                }
                channel2.length_count = 0;
                ticks++;
            }
        }

        

        if (channel3.trigger && channel3.DACenable)// && BIT(mem[0xFF26], 2))
        {
            // Channel 3
            int freq = 2097152/(2048 - channel3.wavelength);
            int samples_per_step = SAMPLING_RATE / freq;
            float volumeFactor = (channel3.volume * MAX_VOLUME)/(4 * 0xFF);

            static char tmp = 0;
            if (!(tmp % 2))
                addDecibel(ptr[i], (channel3.waveform[channel3.step] & 0x0F) * (int)volumeFactor);
            else
                addDecibel(ptr[i], (channel3.waveform[channel3.step] & 0xF0) * (int)volumeFactor);
            tmp++;
            
            channel3.iterations++;
            channel3.length_count++;

            if (channel3.iterations >= samples_per_step) {
                channel3.iterations = 0;
                channel3.step = (channel3.step + 1) % 16;
            }

            if (channel3.length_count >= Waveform::samples_per_length) {
                channel3.length_count = 0;

                if (channel3.length_enable) {
                    channel3.current_length_timer--;

                    if (!channel3.current_length_timer)
                        channel3.trigger = false;
                }

                ticks++;
            }

            
        }

        if (channel4.trigger && BIT(mem[0xFF26], 3))
        {
            // Channel 4
            int freq = 262144 / (channel4.clockDivider * pow(2, channel4.clockShift));

            int samples_per_step = SAMPLING_RATE / freq;
            float volumeFactor = (channel4.initial_volume * MAX_VOLUME)/0xFFFF;

            addDecibel(ptr[i], channel4.sample * (int)volumeFactor);

            channel4.iterations++;
            channel4.length_count++;

            if (channel4.iterations >= samples_per_step) {
                channel4.iterations = 0;
                channel4.sample = rand();
            }

            if (channel4.length_count >= Noise::samples_per_length) {
                channel4.length_count = 0;
                channel4.current_length_timer--;
                if (!channel4.current_length_timer)
                    channel4.trigger = false;
            }
        }

        // std::cout << std::dec << (int)ptr[i] << "\n";
    }
    // std::cout << "\n";
}