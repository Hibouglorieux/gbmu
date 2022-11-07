/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mem.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:49:02 by nallani           #+#    #+#             */
/*   Updated: 2022/11/07 21:06:13 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MEM_CLASS_H
# define MEM_CLASS_H

class Mem {
public:
	Mem();
	Mem(int size);
	~Mem();
	unsigned char& operator[](int i);
private:
	unsigned char* internalArray;
};

#endif
