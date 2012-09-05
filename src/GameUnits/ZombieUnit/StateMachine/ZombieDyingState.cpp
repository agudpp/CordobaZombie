/*
 * ZombieDyingState.cpp
 *
 *  Created on: 28/02/2012
 *      Author: agustin
 */

#include "ZombieDyingState.h"

ZombieDyingState::ZombieDyingState()
{

}

ZombieDyingState::~ZombieDyingState()
{

}


// Enter the state
void ZombieDyingState::enter(ZombieUnit *ref)
{
	// first check which animation have to reproduce depending on the
	// last hit
	int r = ref->getLastHitImpactDir();
	switch(r){
	case Hit_t::FRONT:
		// zombie dies to back
		ref->changeAnimation(ZombieUnit::ANIM_DIE_B);
		break;
	case Hit_t::BACK:
		ref->changeAnimation(ZombieUnit::ANIM_DIE_F);
		break;
	case Hit_t::LEFT:
		ref->changeAnimation(ZombieUnit::ANIM_DIE_R);
		break;
	case Hit_t::RIGHT:
		ref->changeAnimation(ZombieUnit::ANIM_DIE_L);
		break;
	default:
		ASSERT(false);
		ref->changeAnimation(ZombieUnit::ANIM_DIE_B);
	}
}

// exit state
void ZombieDyingState::exit(ZombieUnit *ref)
{
	// do nothing
}

// update state
void ZombieDyingState::update(ZombieUnit *ref)
{
	// check until the animation finish
	if(ref->hasActualAnimEnd()){
		// ready... call dead function
		ref->dead();
	}
	// else do nothing..
}
