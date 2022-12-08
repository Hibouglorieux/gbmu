/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 22:45:13 by lmariott          #+#    #+#             */
/*   Updated: 2022/12/08 18:28:32 by lmariott         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define GBSTATE_OAM_SEARCH 1
#define GBSTATE_PX_TRANSFERT 2
#define GBSTATE_H_BLANK 3
#define GBSTATE_V_BLANK 4

class Loop {
public:
	static void loop();
	static int getState();
private:
	static bool shouldStop;
	static double frameTime;
	static double fpsRefreshTime;
	static unsigned char frameCount;
	const static double refreshingRate;
	static int	state;
};
