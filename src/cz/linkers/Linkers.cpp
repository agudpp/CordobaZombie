/*
 * Linkers.cpp
 *
 *  Namespace providing inter-class binding facilities,
 *  mainly used for setting callback functions.
 *
 *  Created on: Nov 10, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#include <functional>  // std::bind

#include <main_player/MainPlayerWeaponEvent.h>
#include <main_player/MainPlayer.h>
#include <hud/HUD.h>
#include "Linkers.h"

namespace cz {

namespace linker {

void
linkMainPlayerWithHUD(MainPlayer& mp, HUD& hud)
{
	mp.setEventCallback(std::bind(&HUD::updateWeapon,
								  &hud,
								  std::placeholders::_1,
								  std::placeholders::_2));
}


}

}
