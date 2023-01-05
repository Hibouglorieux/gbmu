/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MBC.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/28 22:13:43 by nallani           #+#    #+#             */
/*   Updated: 2022/12/29 17:27:57 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MBC_CLASS_H
# define MBC_CLASS_H

class MBC {
public:
	MBC(void) = default;
	virtual ~MBC(void){}
	virtual unsigned char writeInRom(unsigned short addr, unsigned char value) = 0;
	virtual unsigned short getRomBank(unsigned short addr) = 0;
	virtual unsigned char getRamBank() = 0;
	virtual unsigned short getRamUpperAddress() = 0;
	virtual unsigned char getType() = 0;
	static MBC* createMBC(unsigned char mbcCode);
};

class RomOnly: public MBC {
public:
	RomOnly() = default;
	virtual unsigned char writeInRom(unsigned short addr, unsigned char value);
	virtual unsigned short getRomBank(unsigned short addr);
	virtual unsigned char getRamBank();
	virtual unsigned short getRamUpperAddress() {return 0;}
	virtual unsigned char getType() { return 0; }
};

class MBC1: public MBC {
public:
	MBC1() {bAdvancedBankingMode = false; bEnableRam = false;
		lowBitsRomBankNumber = 0; highBitsRomBankNumberOrRam = 0;}
	virtual unsigned char writeInRom(unsigned short addr, unsigned char value);
	virtual unsigned short getRomBank(unsigned short addr);
	virtual unsigned short getRamUpperAddress() {return 0xBFFF;}
	virtual unsigned char getRamBank();
	virtual unsigned char getType() { return 1; }
private:
	bool bAdvancedBankingMode;
	bool bEnableRam;
	unsigned char lowBitsRomBankNumber;
	unsigned char highBitsRomBankNumberOrRam;
};

class MBC2: public MBC {
public:
	MBC2() {romBankNb = 0; bEnableRam = false;}
	virtual unsigned char writeInRom(unsigned short addr, unsigned char value);
	virtual unsigned short getRomBank(unsigned short addr);
	virtual unsigned char getRamBank();
	virtual unsigned short getRamUpperAddress() {return 0xA1FF;}
	virtual unsigned char getType() { return 2; }
private:
	unsigned char romBankNb;
	bool bEnableRam;
};

//  RTC S   Seconds   0-59 (0-3Bh)
//  RTC M   Minutes   0-59 (0-3Bh)
//  RTC H   Hours     0-23 (0-17h)
//  RTC DL  Lower 8 bits of Day Counter (0-FFh)
//  RTC DH  Upper 1 bit of Day Counter, Carry Bit, Halt Flag
//     	Bit 0  Most significant bit of Day Counter (Bit 8)
//      Bit 6  Halt (0=Active, 1=Stop Timer)
//      Bit 7  Day Counter Carry Bit (1=Counter Overflow)
typedef struct {
	unsigned char seconds;
	unsigned char minutes;
	unsigned char hours;
	unsigned char DL;
	unsigned char DH;
} rtc;

class MBC3: public MBC {
public:
	MBC3() {romBankNb = 0; bEnableRam = false;
	ramBankNb = 0; latched = false; lastVal = 0;}
	virtual unsigned char writeInRom(unsigned short addr, unsigned char value);
	virtual unsigned short getRomBank(unsigned short addr);
	virtual unsigned char getRamBank();
	virtual unsigned short getRamUpperAddress() {return 0xBFFF;}
	virtual unsigned char getType() { return 3; }
	rtc getCurrentTime();
	rtc time;
	unsigned char rtcBindNb;
private:
	unsigned char romBankNb;
	unsigned char ramBankNb;
	bool bEnableRam;
	unsigned char lastVal;
	bool latched;
};

class MBC5: public MBC {
public:
	MBC5() {bEnableRam = 0; leastSignificantRomByte = 0;
	bit9 = 0; ramBankNb = 0;}
	virtual unsigned char writeInRom(unsigned short addr, unsigned char value);
	virtual unsigned short getRomBank(unsigned short addr);
	virtual unsigned char getRamBank();
	virtual unsigned short getRamUpperAddress() {return 0xBFFF;}
	virtual unsigned char getType() { return 5; }
private:
	bool bEnableRam;
	unsigned char leastSignificantRomByte;
	bool bit9;
	unsigned char ramBankNb;

};

#endif
