/*
 * CivilIdleState.cpp
 *
 *  Created on: 16/07/2012
 *      Author: agustin
 */

#include <cmath>

#include "DebugUtil.h"
#include "CivilIdleState.h"
#include "CivilDefs.h"


CivilIdleState::CivilIdleState()
{

}

CivilIdleState::~CivilIdleState()
{
}

// Enter the state
void CivilIdleState::enter(CivilUnit *ref)
{
	// random state
	ref->changeAnimation(CivilUnit::ANIM_IDLE_BASE
			+ std::rand() % CIVIL_NUM_ANIM_IDLE);
}

// exit state
void CivilIdleState::exit(CivilUnit *ref)
{

}

// update state
void CivilIdleState::update(CivilUnit *ref)
{
	// if we finish animation start new random idle
	if(ref->hasActualAnimEnd()){
		ref->changeAnimation(CivilUnit::ANIM_IDLE_BASE
					+ std::rand() % CIVIL_NUM_ANIM_IDLE);

#ifdef DEBUG
		if(ref->getActualAnimID() == CivilUnit::ANIM_IDLE_BASE){
			debugWARNING("TODO: Carlox, capaz que alomejor quieras reproducir un "
					"sonido con un grado mas de random, devez en cuando que el "
					"civil haga un sonido?\n");
		}
#endif
	}
}

