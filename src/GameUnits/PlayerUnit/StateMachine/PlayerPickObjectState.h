/*
 * PlayerPickObjectState.h
 *
 *  Created on: 04/06/2012
 *      Author: raul
 */

#ifndef PLAYERPICKOBJECTSTATE_H_
#define PLAYERPICKOBJECTSTATE_H_


#include "IState.h"
#include "PlayerUnit.h"
#include <OgreAnimation.h>
#include <OgreAnimationState.h>


class PlayerPickObjectState : public IState<PlayerUnit *>
{
public:
	PlayerPickObjectState();
	virtual ~PlayerPickObjectState();

	// Enter the state
	void enter(PlayerUnit *);

	// exit state
	void exit(PlayerUnit *);

	// update state
	void update(PlayerUnit *);
private:
	inline bool isPickingObject(PlayerUnit *ref);
	void doPickObject(PlayerUnit *ref);

};

inline bool PlayerPickObjectState::isPickingObject(PlayerUnit *ref){
	return ref->getActualAnimID() == PlayerUnit::ANIM_PLANT_BOMB;
}

#endif /* PLAYERPICKOBJECTSTATE_H_ */
