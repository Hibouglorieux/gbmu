/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MBC.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/28 22:13:43 by nallani           #+#    #+#             */
/*   Updated: 2022/12/28 22:49:52 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MBC_CLASS_H
# define MBC_CLASS_H

class MBC {
public:
	MBC(void) = default;
	virtual ~MBC(void){}
	virtual unsigned char writeInRom(unsigned short addr, unsigned char value) = 0;
	virtual unsigned char getRomBank(unsigned short addr) = 0;
	virtual unsigned char getRamBank() = 0;
	static MBC* createMBC(unsigned char mbcCode);
};

class RomOnly: public MBC {
public:
	RomOnly() = default;
	virtual unsigned char writeInRom(unsigned short addr, unsigned char value);
	virtual unsigned char getRomBank(unsigned short addr);
	virtual unsigned char getRamBank();
};

class MBC1: public MBC {
public:
	MBC1() {bAdvancedBankingMode = false; bEnableRam = false;
		lowBitsRomBankNumber = 0; highBitsRomBankNumberOrRam = 0;}
	virtual unsigned char writeInRom(unsigned short addr, unsigned char value);
	virtual unsigned char getRomBank(unsigned short addr);
	virtual unsigned char getRamBank();
private:
	bool bAdvancedBankingMode;
	bool bEnableRam;
	unsigned char lowBitsRomBankNumber;
	unsigned char highBitsRomBankNumberOrRam;
};

class MBC2: public MBC {
public:
	MBC2() = default;
	virtual unsigned char writeInRom(unsigned short addr, unsigned char value);
	virtual unsigned char getRomBank(unsigned short addr);
	virtual unsigned char getRamBank();
private:
};

class MBC3: public MBC {
public:
	MBC3() = default;
	virtual unsigned char writeInRom(unsigned short addr, unsigned char value);
	virtual unsigned char getRomBank(unsigned short addr);
	virtual unsigned char getRamBank();
private:
	bool register1;
};

class MBC5: public MBC {
public:
	MBC5() = default;
	virtual unsigned char writeInRom(unsigned short addr, unsigned char value);
	virtual unsigned char getRomBank(unsigned short addr);
	virtual unsigned char getRamBank();
};

#endif
