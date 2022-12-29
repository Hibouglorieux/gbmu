/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mem.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:49:02 by nallani           #+#    #+#             */
/*   Updated: 2022/12/29 22:50:34 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MEM_CLASS_H
# define MEM_CLASS_H
#include <string>
#include <map>
#include <vector>
#include "MBC.hpp"

class Mem;

class MemWrap {
	public:
		MemWrap(const Mem& parent, unsigned short offset, unsigned char& ref):
			addr(offset), value(ref), memRef(parent) {}
		unsigned char& operator=(unsigned char newValue);
		unsigned char	operator+() {return value;}
		unsigned char* operator&() {return &value;}
		operator unsigned char&() { return value; }
		operator const unsigned char&() const { return value; }
	private:
		const unsigned short addr;
		unsigned char& value;
		const Mem& memRef;
};


class Mem {
public:
	Mem() = delete;
	Mem(const std::string& pathToRom);
	static Mem* loadFromFile(const std::string& pathToRom);
	virtual ~Mem();
	MemWrap operator[](unsigned int i);
	const MemWrap operator[](unsigned int i) const;
	bool isValid;
	void supervisorWrite(unsigned int addr, unsigned char value);
	virtual unsigned char* getCGBVram() {return nullptr;}


	std::string getTitle();
	bool	isCGB();
	int		getCartridgeType();

	static const std::map<unsigned short, unsigned char> readOnlyBits;
    std::vector<unsigned char*>	extraRamBanks;
    std::vector<unsigned char*>	romBanks;
	MBC* mbc;

protected:
	void init();
	unsigned char*	internalArray;
	std::array<unsigned char*, 8> CGBextraRamBanks;

	//std::array<std::array<unsigned char, 0x1000>, 8> CGBextraRamBanks;

	virtual unsigned char&	getRefWithBanks(unsigned short addr) const;
	unsigned int	memSize;
	static int 			getRomBanksNb(char romSizeCode);
	static int 			getExtraRamBanksNb(char ramSizeCode);
};

class CGBMem : public Mem{
public:
	CGBMem(const std::string& pathToRom);
	virtual ~CGBMem();
	virtual unsigned char* getCGBVram() {return CGBVramBank;}

	mutable bool			bIsUsingCGBVram;
	mutable unsigned char	CGBextraRamBankNb;
protected:
	virtual unsigned char&	getRefWithBanks(unsigned short addr) const;
	unsigned char*	CGBVramBank;
};

#endif
