/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 22:45:13 by lmariott          #+#    #+#             */
/*   Updated: 2022/12/08 23:07:37 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOOP_CLASS_H
# define LOOP_CLASS_H

#define GBSTATE_MSK 0x03
#define GBSTATE_H_BLANK 0
#define GBSTATE_V_BLANK 1
#define GBSTATE_OAM_SEARCH 2
#define GBSTATE_PX_TRANSFERT 3

class Loop {
public:
	static bool loop();

private:
    static bool showVram;
    static bool showBG;
    static bool showHexdump;
    static bool showRegisters;
	static bool shouldStop;
	static double frameTime;
	static double fpsRefreshTime;
	static unsigned char frameCount;
	const static double refreshingRate;
};

#endif
