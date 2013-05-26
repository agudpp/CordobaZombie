/*
 * PlayerPickObjectstate.cpp
 *
 *  Created on: 04/06/2012
 *      Author: raul
 */

#include "PlayerPickObjectState.h"
#include "CollectableObjTypes.h"


PlayerPickObjectState::PlayerPickObjectState()
{
}

PlayerPickObjectState::~PlayerPickObjectState()
{
}

void PlayerPickObjectState::doPickObject(PlayerUnit *ref){

	ASSERT(ref->getTargetCollObject());

	// Only if I can pick it up :|
	if(ref->canPickObject()){
		//Hide the entity
		ref->getTargetCollObject()->objectPicked();
		//Atach object to the player
		ref->addCollectObeject(ref->getTargetCollObject());
		//Clear the target collectable object
		ref->setTargetCollObject(0);
	}

	return;
}


// Enter the state
void PlayerPickObjectState::enter(PlayerUnit *ref)
{
	ASSERT(ref)

	if(ref->getUnitPath().isEmpty()){
		ref->changeAnimation(PlayerUnit::ANIM_PLANT_BOMB);
		return;
	}

	// we always run?
	ref->changeAnimation(PlayerUnit::ANIM_RUN);
	ref->getActualAnimation()->setLoop(true);
}

// exit state
void PlayerPickObjectState::exit(PlayerUnit *ref)
{

}

// update state
void PlayerPickObjectState::update(PlayerUnit *ref)
{
	ASSERT(ref);
	// check if we are picking up the object
	if(isPickingObject(ref)){
		// if the animation has ended
		if(ref->hasActualAnimEnd()){
			// we finish
			ref->getFSM().newEvent(PlayerUnit::E_DONE);
		}else{
			// if the animation time is at half of it's total time
			Ogre::AnimationState *anim = ref->getActualAnimation();
			ASSERT(anim);
			if(anim->getLength()*0.5 < anim->getTimePosition() and ref->getTargetCollObject()){
				doPickObject(ref);
			}
		}
		// else we continue waiting
		return;
	}

	// if we are where the object is
	if(!ref->moveThroughPath()){
		ref->changeAnimation(PlayerUnit::ANIM_PLANT_BOMB);
	}
}
