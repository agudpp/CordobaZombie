/*
 * PlayerBombState.cpp
 *
 *  Created on: 14/05/2012
 *      Author: agustin
 */

#include "GlobalObjects.h"
#include "PlayerDefs.h"
#include "PlayerBombState.h"

PlayerBombState::PlayerBombState()
{

}

PlayerBombState::~PlayerBombState()
{

}


void PlayerBombState::doPlantBomb(PlayerUnit *ref)
{
	Bomb *b = ref->getActualBomb();
	b->getSceneNode()->setPosition(
			ref->getSceneNode()->getPosition());
	b->show();

	// activate the bomb
	b->startUpdate();

	// remove the bomb from the player
	ref->removeBomb(b);
}


// Enter the state
void PlayerBombState::enter(PlayerUnit *ref)
{
	ASSERT(ref->getActualBomb());

	// check if we are in the place
	if(ref->getUnitPath().isEmpty()){
		ref->changeAnimation(PlayerUnit::ANIM_PLANT_BOMB);
		return;
	}

	// we prepare the unit to run to the bomb place

	// we always run?
	ref->changeAnimation(PlayerUnit::ANIM_RUN);
	ref->getActualAnimation()->setLoop(true);


	// set the IAState
	ref->getIAState().event = IAEP_PLANTING_BOMB;
	ref->getIAState().goingTo =
			ref->getUnitPath().getPointPath()[ref->getUnitPath().getPathSize()-1];

}

// exit state
void PlayerBombState::exit(PlayerUnit *ref)
{
}

// update state
void PlayerBombState::update(PlayerUnit *ref)
{
	// check if we are putting the bomb
	if(isPlantingBomb(ref)){
		// wait until the animation finish
		if(ref->hasActualAnimEnd()){
			// we finish
			ref->getFSM().newEvent(PlayerUnit::E_DONE);
		}else{
			// if the animation time is at half of it's total time
			Ogre::AnimationState *anim = ref->getActualAnimation();
			ASSERT(anim);
			if(anim->getLength()*0.5 < anim->getTimePosition() and ref->getActualBomb()){
				doPlantBomb(ref);
			}
		}

		// we continue waiting
		return;
	}
	if(!ref->moveThroughPath()){
		ref->changeAnimation(PlayerUnit::ANIM_PLANT_BOMB);
	}
}

