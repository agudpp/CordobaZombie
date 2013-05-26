/*
 * CivilBeenHitState.cpp
 *
 *  Created on: 16/07/2012
 *      Author: agustin
 */

#include "DebugUtil.h"
#include "GameUnitDefines.h"
#include "CommonMath.h"

#include "CivilBeenHitState.h"


CivilBeenHitState::CivilBeenHitState()
{

}

CivilBeenHitState::~CivilBeenHitState()
{
}


// Enter the state
void CivilBeenHitState::enter(CivilUnit *ref)
{
	// first of all we assume that we have the Hit_t was assigned to mLastHit
	//(in the getLastHit()).
	// now we will check the power of the hit kill us or not
	const Hit_t &h = ref->getLastHit();

	if(ref->getLife() <= 0){
		debugRED("CIVIL DEAD!\n");
		// we just die... emmit dying event
		ref->getFSM().newEvent(CivilUnit::E_DYING);
		return;
	}

	// if we already are in beenHit we use the same animation.
	if(ref->getFSM().getLastState() == this) return;

	// we don't die... change the animation depending of the direction
	int r = ref->getLastHitImpactDir();
	switch(r){
	case Hit_t::FRONT:
		// zombie dies to back
		ref->changeAnimation(CivilUnit::ANIM_HIT_B);
		break;
	case Hit_t::BACK:
		ref->changeAnimation(CivilUnit::ANIM_HIT_F);
		break;
		// TODO: aca tenemos que ver como elegimos esto, por el momento
		// hacemos la cochinada de elegir que si es derecho entonces viene
		// de atras si es de la izquierda por delante
	case Hit_t::LEFT:
		ref->changeAnimation(CivilUnit::ANIM_HIT_B);
		break;
	case Hit_t::RIGHT:
		ref->changeAnimation(CivilUnit::ANIM_HIT_F);
		break;
	default:
		ASSERT(false);
		ref->changeAnimation(CivilUnit::ANIM_HIT_F);
	}

}

// exit state
void CivilBeenHitState::exit(CivilUnit *ref)
{

}

// update state
void CivilBeenHitState::update(CivilUnit *ref)
{
	// wait until we finish the animation, once it finish emit E_DONE
	if(ref->hasActualAnimEnd()){
		if(ref->getFollowingUnit()) {
			ref->getFSM().newEvent(CivilUnit::E_DONE);
		} else {
			ref->getFSM().newEvent(CivilUnit::E_ESCAPE);
		}

		return;
	}
}
