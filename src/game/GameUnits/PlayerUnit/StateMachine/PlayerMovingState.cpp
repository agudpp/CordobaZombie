/*
 * PlayerMovingstate.cpp
 *
 *  Created on: 03/03/2012
 *      Author: agustin
 */

#include "PlayerMovingState.h"

PlayerMovingState::PlayerMovingState()
{
}

PlayerMovingState::~PlayerMovingState()
{
}

// Enter the state
void PlayerMovingState::enter(PlayerUnit *ref)
{
	if(ref->getUnitPath().isEmpty()){
		ref->getFSM().newEvent(PlayerUnit::E_DONE);
		return;
	}

	// we always run?
	ref->changeAnimation(PlayerUnit::ANIM_RUN);
	ref->getActualAnimation()->setLoop(true);


	// set the IAState
	ref->getIAState().event = IAEP_MOVING;
	ref->getIAState().goingTo =
			ref->getUnitPath().getPointPath()[ref->getUnitPath().getPathSize()-1];
}

// exit state
void PlayerMovingState::exit(PlayerUnit *ref)
{

}

// update state
void PlayerMovingState::update(PlayerUnit *ref)
{
	// TODO: alomejor queremos chequear aca si detectamos algun zombie
	// para atacarlo o seguimos derecho (dependiendo el AttackingBehavior?)
	// por ahora solo seguimos el path


	// Follow path logic
	sm::Vector2 move, aux;

	if(!ref->followPathSteerVec(aux)){
		// stop... emmit event done
		ref->getFSM().newEvent(PlayerUnit::E_DONE);
		return;
	}
	move += aux;

//	// we are moving... we are in a group?
//	if(ref->isInGroup()){
//		// calculate the steering vector of group moving
//		ref->separationSteerVec(aux);
//		move += aux;
//		ref->cohesionSteerVec(aux);
//		move += aux;
//		// Alignment we will not use this by now
//
//	}

	// check the collision steering vector
	if(!ref->obtainCollisionObjects().empty()){
		ref->repellingSteerVec(aux, ref->getCollideObjects());
		move += aux;
	}

	ref->move(move);
}
