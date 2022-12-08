/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mem.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:49:02 by nallani           #+#    #+#             */
/*   Updated: 2022/12/08 20:40:21 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MEM_CLASS_H
# define MEM_CLASS_H
#include <string>

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
private:
	unsigned char* internalArray;
	unsigned int	memSize;
};

#endif
