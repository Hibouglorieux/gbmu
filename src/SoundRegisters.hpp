#ifndef SOUND_REGISTERS_HPP
#define SOUND_REGISTERS_HPP


typedef struct {
    unsigned char nr52;
    unsigned char nr51;
    unsigned char nr50;
    struct {
        unsigned char nr10;
        unsigned char nr11;
        unsigned char nr12;
        unsigned char nr13;
        unsigned char nr14;
    } channel1;

    struct {
        unsigned char nr21;
        unsigned char nr22;
        unsigned char nr23;
        unsigned char nr24;
    } channel2;

    struct {
        unsigned char nr31;
        unsigned char nr32;
        unsigned char nr33;
        unsigned char nr34;
    } channel3;

    struct {
        unsigned char nr41;
        unsigned char nr42;
        unsigned char nr43;
        unsigned char nr44;
    } channel4;

} sound_registers;

#endif