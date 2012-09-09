/*
 * PlayerBombState.h
 *
 *  Created on: 14/05/2012
 *      Author: agustin
 */

#ifndef PLAYERBOMBSTATE_H_
#define PLAYERBOMBSTATE_H_


#include "FSM/IState.h"
#include "PlayerUnit.h"


class PlayerBombState : public ia::IState<PlayerUnit *> {
public:
	PlayerBombState();
	virtual ~PlayerBombState();

	// Enter the state
	void enter(PlayerUnit *ref);

	// exit state
	void exit(PlayerUnit *ref);

	// update state
	void update(PlayerUnit *ref);

private:
	inline bool isPlantingBomb(PlayerUnit *ref);
	void doPlantBomb(PlayerUnit *ref);
};

inline bool PlayerBombState::isPlantingBomb(PlayerUnit *ref)
{
	return ref->getActualAnimID() == PlayerUnit::ANIM_PLANT_BOMB;
}



#endif /* PLAYERBOMBSTATE_H_ */
