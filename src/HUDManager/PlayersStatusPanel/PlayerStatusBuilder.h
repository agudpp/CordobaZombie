/*
 * PlayerStatusBuilder.h
 *
 *  Created on: 13/08/2012
 *      Author: agustin
 *
 */

#ifndef PLAYERSTATUSBUILDER_H_
#define PLAYERSTATUSBUILDER_H_

class PlayerUnit;
class PlayerStatus;

namespace hud {


class PlayerStatusBuilder {
public:
	enum PlayerStatusID {
		PLAYER_TULIAN,
		PLAYER_CORAL,
		PLAYER_CARDENAL,
	};
public:

	/**
	 * Build the Player status associated
	 * @param	id			The id we want to build
	 * @param	unit		The unit associated
	 *TODO
	 */
	PlayerStatus *create(PlayerStatusID id, PlayerUnit *unit);

};

}
#endif /* PLAYERSTATUSBUILDER_H_ */
