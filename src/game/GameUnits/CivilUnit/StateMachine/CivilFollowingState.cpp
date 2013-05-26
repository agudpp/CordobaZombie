/*
 * CivilFollowingState.cpp
 *
 *  Created on: 16/07/2012
 *      Author: agustin
 */

#include "DebugUtil.h"
#include "CivilFollowingState.h"
#include "GameUnitDefines.h"
#include "CommonMath.h"


CivilFollowingState::CivilFollowingState()
{

}

CivilFollowingState::~CivilFollowingState()
{

}


// Enter the state
void CivilFollowingState::enter(CivilUnit *ref)
{
	GameUnit *fu = ref->getFollowingUnit();
	if(!fu){
		ref->getFSM().newEvent(CivilUnit::E_UNIT_LOST);
		return;
	}
	ref->getUnitPath().clearPath();

	// set the animation depending on the distance
	if(closerUnit(ref)){
		// we are closer to the unit, "we are safe"
		// set some of the idle animations
		ref->changeAnimation(CivilUnit::ANIM_IDLE_BASE
							+ std::rand() % CIVIL_NUM_ANIM_IDLE);
		debugBLUE("Unit Close\n");
	} else {
		// we are far away... run to be closer to the unit
		ref->changeAnimation(CivilUnit::ANIM_RUN_BASE +
				std::rand() % CIVIL_NUM_ANIM_RUN);
		ref->getActualAnimation()->setLoop(true);
		debugBLUE("Unit FAR away\n");
	}
}

// exit state
void CivilFollowingState::exit(CivilUnit *ref)
{

}

// update state
void CivilFollowingState::update(CivilUnit *ref)
{
	// check if we still following the unit
	GameUnit *followUnit = ref->getFollowingUnit();
	if(!followUnit) {
		ref->getFSM().newEvent(CivilUnit::E_UNIT_LOST);
		return;
	}

	// sub states
	const int animID  = ref->getActualAnimID();
	if((animID >= CivilUnit::ANIM_IDLE_BASE) && (animID <
			(CivilUnit::ANIM_IDLE_BASE + CIVIL_NUM_ANIM_IDLE))){
		// we are in idle state
		if(!closerUnit(ref)){
			// we have to start to follow the unit
			ref->changeAnimation(CivilUnit::ANIM_RUN_BASE +
							std::rand() % CIVIL_NUM_ANIM_RUN);
			ref->getActualAnimation()->setLoop(true);
			debugBLUE("Start to run\n");
		} else {
			// change idle anim?
			if(ref->hasActualAnimEnd()){
				ref->changeAnimation(CivilUnit::ANIM_IDLE_BASE
						+ std::rand() % CIVIL_NUM_ANIM_IDLE);
				debugBLUE("IDLE random\n");
			}
		}
	} else if((animID >= CivilUnit::ANIM_RUN_BASE) &&
			(animID < (CivilUnit::ANIM_RUN_BASE + CIVIL_NUM_ANIM_RUN))){
		// we are following the unit
		if(closerUnit(ref)){
			// stop it
			ref->changeAnimation(CivilUnit::ANIM_IDLE_BASE
					+ std::rand() % CIVIL_NUM_ANIM_IDLE);
			return;
		}

		// else we have to go to the unit... Following a path or going directly
		// We will use the same logic as the zombies. If we lost the visibility
		// of the player we will try to go to the last place where it was seen
		// and then try to see it again, if we can see it we continue following
		// it, otherwise we emit a sound or something that we lost it
		math::Vector2 move, aux;
		// check the collision steering vector
		if(!ref->obtainCollisionObjects().empty()){
			ref->repellingSteerVec(move, ref->getCollideObjects());
		}
		if(ref->getUnitPath().isEmpty()){
			// we are moving just directly
			if(!ref->haveLineOfSightTo(followUnit, CIVIL_MAX_VISIBILITY_SQRDIST)){
				// lost the target... calculate the possible path
				if(!ref->getPathTo(followUnit->getPosition())){
					// we cannot move..
					ref->getFSM().newEvent(CivilUnit::E_UNIT_LOST);
					debug("Following unit LOST!\n");
					return;
				} else {
					// we can move to that position
					return;
				}
			} else {
				// we can see the followingUnit... go directly to him
				ref->seekSteerVec(aux, followUnit->getPosition());
				move += aux;
			}
		} else {
			// moving throw path
			if(ref->haveLineOfSightTo(followUnit, CIVIL_MAX_VISIBILITY_SQRDIST)){
				// we see again the following unit... clear the path and follow directly
				ref->getUnitPath().clearPath();
				debug("Seen following unit again\n");
				return;
			}

			// else.. we cannot see the followingUnit... follow the path
			if(!ref->followPathSteerVec(aux)){
				// no more points... ASSERT that we can see the object...
				debug("We lost the followingUnit... Target lost?\n");
				ref->getFSM().newEvent(CivilUnit::E_UNIT_LOST);
				return;
			}
			move += aux;
		}

		// move the unit
		ref->move(move);
	} else {
		debugERROR("Other animation?: %d\n", ref->getActualAnimID());
		ASSERT(false);
	}


}
