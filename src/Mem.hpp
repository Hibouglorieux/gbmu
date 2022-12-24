/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mem.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:49:02 by nallani           #+#    #+#             */
/*   Updated: 2022/12/24 04:30:16 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MEM_CLASS_H
# define MEM_CLASS_H
#include <string>

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
	Mem(std::string pathToRom);
	Mem(const Mem& rhs);
	const Mem& operator=(const Mem& rhs);
	~Mem();
	MemWrap operator[](unsigned int i);
	const MemWrap operator[](unsigned int i) const;
	bool isValid;
	void supervisorWrite(unsigned int addr, unsigned char value);
private:
	unsigned char* internalArray;
	unsigned int	memSize;
	// TODO regroup all readonly/unused bytes
	// in register in this map and use it
	// inside memWrap
	//std::map<short, unsigned char> readOnlyBytes;
};

#endif
