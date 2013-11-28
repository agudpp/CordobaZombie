/*
 * Linkers.h
 *
 *  Namespace providing inter-class binding facilities,
 *  mainly used for setting callback functions.
 *
 *  Created on: Nov 10, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef LINKERS_H_
#define LINKERS_H_



namespace cz {

class MainPlayer;
class HUD;

namespace linker {

void linkMainPlayerWithHUD(MainPlayer& mp, HUD& hud);

}

}


#endif /* LINKERS_H_ */
