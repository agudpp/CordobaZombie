/*
 * PlayerEngagingState.cpp
 *
 *  Created on: 20/03/2012
 *      Author: agustin
 *
 */

#include "PlayerEngagingState.h"

PlayerEngagingState::PlayerEngagingState()
{

}

PlayerEngagingState::~PlayerEngagingState()
{

}


// Enter the state
void PlayerEngagingState::enter(PlayerUnit *ref)
{
	// check if we have an engaging unit
	if(!ref->getActualTarget()){
		ref->getFSM().newEvent(PlayerUnit::E_ENEMY_LOST);
		return;
	}

	// TODO: haciendo esto! (tenemos que tener en cuenta las unidades
	// del grupo (por ejemplo para no tirarle granadas a un zombie
	// si tenemos un personaje cerca y eso...)

	// can attack directly?

	if(ref->haveLineOfSightTo(ref->getActualTarget(),
			PLAYER_MAX_VISIBILITY_SQRDIST)){
		if(ref->canAttackTarget(ref->getActualTarget())){
			// yes, we can attack directly
			ref->getFSM().newEvent(PlayerUnit::E_ENEMY_SEEN);
			return;
		} else {
			// we cannot attack directly.. but we can see it... move right
			// directly
			ref->getUnitPath().clearPath();
			return;
		}
	} else {
		// we cannot see the enemy... just get the path to the unit
		if(!ref->getPathTo(ref->getActualTarget()->getPosition())){
			debug("Cannot move to the enemy position!!!\n");
			ref->getFSM().newEvent(PlayerUnit::E_ENEMY_LOST);
			return;
		}
	}

	// set the running animation
	ref->changeAnimation(PlayerUnit::ANIM_RUN);
	ref->getActualAnimation()->setLoop(true);
//	ref->setVelocity(ref->getVelocity());
}

// exit state
void PlayerEngagingState::exit(PlayerUnit *ref)
{

}

// update state
void PlayerEngagingState::update(PlayerUnit *ref)
{
	GameUnit *target = ref->getActualTarget();

	// if have no more target then advise that we have lost the target?
	if(!target){
		ref->getFSM().newEvent(PlayerUnit::E_ENEMY_LOST);
		return;
	}

	// first check if we are close to the enemy, if we are, then attack them
	if(ref->haveLineOfSightTo(ref->getActualTarget(),
			PLAYER_MAX_VISIBILITY_SQRDIST)){
		// can see the enemy... can attack it?
		if(ref->canAttackTarget(target)){
			// YES we can attack it
			ref->getFSM().newEvent(PlayerUnit::E_ENEMY_SEEN);
			return;
		}

		// else we cannot attack to it... move directly..
		core::Vector2 move, aux;
		// check the collision steering vector
		if(!ref->obtainCollisionObjects().empty()){
			ref->repellingSteerVec(move, ref->getCollideObjects());
		}

		// move straight forward
		ref->seekSteerVec(aux, target->getPosition());
		move += aux;

		ref->move(move);
		return;
	}

	// else we cannot see the enemy... check if we are moving or we have to
	// get the path
	// TODO: Aca tenemos que hacer un updatePath(path,
	//	ref->getActualTarget()->getPosition());... de esta manera actualizamos
	// el path (o pedazo del path para poder corregir si es necesario como llegar
	// al zombie mas rapidamente.

	if(ref->getUnitPath().isEmpty()){
		// calculate the path...?
		if(!ref->getPathTo(ref->getActualTarget()->getPosition())){
			debug("Cannot move to the enemy position!!!\n");
			ref->getFSM().newEvent(PlayerUnit::E_ENEMY_LOST);
			return;
		}
	}

	// now we can move throw the path... check if we have to update it
	if(ref->needUpdatePath()){
		ref->updateTargetPath();
	}

	// now move...

	core::Vector2 move, aux;
	// check the collision steering vector
	if(!ref->obtainCollisionObjects().empty()){
		ref->repellingSteerVec(move, ref->getCollideObjects());
	}
	if(!ref->followPathSteerVec(aux)){
		// no more points... ASSERT that we can see the object...
		debug("We lost the enemiy... Target lost?\n");
		ref->getFSM().newEvent(PlayerUnit::E_ENEMY_LOST);
		return;
	}
	move += aux;

	// move the unit
	ref->move(move);
}

