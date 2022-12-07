/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utility.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nallani <nallani@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/09 21:16:08 by nallani           #+#    #+#             */
/*   Updated: 2022/11/09 21:18:45 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef UTILITY_H
# define UTILITY_H

#include <memory>
#include <string>
#include <stdexcept>

template <typename ... Args>
std::string string_format( const std::string& format, Args ... args );

#include "Utility.tpp"
#endif
