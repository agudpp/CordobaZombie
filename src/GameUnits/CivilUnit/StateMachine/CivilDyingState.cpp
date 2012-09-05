/*
 * CivilDyingState.cpp
 *
 *  Created on: 16/07/2012
 *      Author: agustin
 */

#include "CivilDyingState.h"
#include "DebugUtil.h"
#include "CivilDefs.h"


CivilDyingState::CivilDyingState()
{

}

CivilDyingState::~CivilDyingState()
{

}

// Enter the state
void CivilDyingState::enter(CivilUnit *ref)
{
	// first check which animation have to reproduce depending on the
	// last hit
	int r = ref->getLastHitImpactDir();
	switch(r){
	case Hit_t::FRONT:
		// civil dies to back
		ref->changeAnimation(CivilUnit::ANIM_DIE_B);
		break;
	case Hit_t::BACK:
		ref->changeAnimation(CivilUnit::ANIM_DIE_F);
		break;
	case Hit_t::LEFT:
		ref->changeAnimation(CivilUnit::ANIM_DIE_R);
		break;
	case Hit_t::RIGHT:
		ref->changeAnimation(CivilUnit::ANIM_DIE_L);
		break;
	default:
		ASSERT(false);
		ref->changeAnimation(CivilUnit::ANIM_DIE_B);
	}
}

// exit state
void CivilDyingState::exit(CivilUnit *ref)
{

}


// update state
void CivilDyingState::update(CivilUnit *ref)
{
	// check until the animation finish
	if(ref->hasActualAnimEnd()){
		// ready... call dead function
		ref->dead();
	}
}

