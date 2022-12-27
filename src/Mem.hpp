/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mem.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:49:02 by nallani           #+#    #+#             */
/*   Updated: 2022/12/26 21:41:40 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MEM_CLASS_H
# define MEM_CLASS_H
#include <string>
#include <map>
#include <vector>

# define BIT(val, bit) ((val & (1 << bit)) >> bit)
# define SET(val, bit) {val |= (1 << bit);}
# define RES(val, bit) {val &= ~(1 << bit);}

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
	Mem();
	Mem(int size);
	Mem(const std::string& pathToRom);
	const Mem& operator=(const Mem& rhs);
	~Mem();
	MemWrap operator[](unsigned int i);
	const MemWrap operator[](unsigned int i) const;
	bool isValid;
	void supervisorWrite(unsigned int addr, unsigned char value);


	std::string getTitle();
	bool	isCGB();
	int		getCartridgeType();

	mutable bool bIsAdvancedBankingMode;
	mutable bool bEnableRam;
	mutable unsigned char romBankNumber;
	mutable unsigned char ramBankNumber;

	static const std::map<unsigned short, unsigned char> readOnlyBits;
    std::vector<unsigned char*>	extraRamBanks;
    std::vector<unsigned char*>	romBanks;

private:
	void init();
	unsigned char*	internalArray;

	unsigned char&	getRefWithBanks(unsigned short addr) const;
	unsigned int	memSize;
	int 			getRomBanksNb(char romSizeCode);
	int 			getExtraRamBanksNb(char ramSizeCode);
};

#endif
