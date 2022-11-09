/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mem.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:49:02 by nallani           #+#    #+#             */
/*   Updated: 2022/11/09 21:01:49 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MEM_CLASS_H
# define MEM_CLASS_H
#include <string>

class Mem {
public:
	Mem();
	Mem(int size);
	Mem(std::string pathToRom);
	Mem(const Mem& rhs);
	const Mem& operator=(const Mem& rhs);
	~Mem();
	unsigned char& operator[](unsigned int i);
	bool isValid;
private:
	unsigned char* internalArray;
	unsigned int	memSize;
};

#endif
