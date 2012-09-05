/*
 * ZombieBeenHitState.cpp
 *
 *  Created on: 28/02/2012
 *      Author: agustin
 *
 */

#include "ZombieBeenHitState.h"

ZombieBeenHitState::ZombieBeenHitState()
{

}

ZombieBeenHitState::~ZombieBeenHitState()
{
}


// Enter the state
void ZombieBeenHitState::enter(ZombieUnit *ref)
{
	// first of all we assume that we have the Hit_t was assigned to mLastHit
	//(in the getLastHit()).
	// now we will check the power of the hit kill us or not
	const Hit_t &h = ref->getLastHit();
//	const int decLife = static_cast<int>(h.power);
//
//	ref->setLife(ref->getLife() - decLife);
	if(ref->getLife() <= 0){
		debugRED("ZOMBIE DEAD!\n");
		// we just die... emmit dying event
		ref->getFSM().newEvent(ZombieUnit::E_DYING);
		return;
	}

	// if we already are in beenHit we use the same animation.
	if(ref->getFSM().getLastState() == this) return;

	// we don't die... change the animation depending of the direction
	int r = ref->getLastHitImpactDir();
	switch(r){
	case Hit_t::FRONT:
		// zombie dies to back
		ref->changeAnimation(ZombieUnit::ANIM_HIT_B);
		break;
	case Hit_t::BACK:
		ref->changeAnimation(ZombieUnit::ANIM_HIT_F);
		break;
		// TODO: aca tenemos que ver como elegimos esto, por el momento
		// hacemos la cochinada de elegir que si es derecho entonces viene
		// de atras si es de la izquierda por delante
	case Hit_t::LEFT:
		ref->changeAnimation(ZombieUnit::ANIM_HIT_B);
		break;
	case Hit_t::RIGHT:
		ref->changeAnimation(ZombieUnit::ANIM_HIT_F);
		break;
	default:
		ASSERT(false);
		ref->changeAnimation(ZombieUnit::ANIM_HIT_F);
	}

}

// exit state
void ZombieBeenHitState::exit(ZombieUnit *ref)
{
	// check if we are already attacking some one
	if(ref->getActualTarget() ){
		return;
	}

	// attack the enemy?
	if(!ref->getLastHit().shooter){
		// only turn to the direction
		ref->setDirection(ref->getLastHit().hitDir *-1.0f);
		return;
	}
	// else set the new target
	ref->setActualTartet(ref->getLastHit().shooter);
}

// update state
void ZombieBeenHitState::update(ZombieUnit *ref)
{
	// wait until we finish the animation, once it finish emit E_DONE
	if(ref->hasActualAnimEnd()){
		ref->getFSM().newEvent(ZombieUnit::E_DONE);
		return;
	}
}
