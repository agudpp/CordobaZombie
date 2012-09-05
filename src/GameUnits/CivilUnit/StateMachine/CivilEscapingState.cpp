/*
 * CivilEscapingState.cpp
 *
 *  Created on: 16/07/2012
 *      Author: agustin
 */

#include <cmath>

#include "CivilUnit.h"
#include "DebugUtil.h"
#include "CivilEscapingState.h"
#include "GlobalObjects.h"


void CivilEscapingState::calculateRandomPath(CivilUnit *ref)
{
	int numNodes = std::rand() % (CivilUnit::MAX_NUM_NODES -
			CivilUnit::MIN_NUM_NODES);
	numNodes += CivilUnit::MIN_NUM_NODES;

	ASSERT(GLOBAL_LVL_MNGR->getPathfinderManager());
	// We will get a random path and set it to the zombie unit
	if(GLOBAL_LVL_MNGR->getPathfinderManager()->getRandomPath(ref->getPosition(),
			ref->getPath(), 0.0f, numNodes) != PathfindingManager::NORMAL_PATH){
		// mmm some problem?
		debugERROR("WTF\n");
		return;
	}

	ref->restartPath();
	// set the walking anim
	ref->changeAnimation(CivilUnit::ANIM_RUN_ESCAPE);
	ref->getActualAnimation()->setLoop(true);
}


CivilEscapingState::CivilEscapingState()
{

}

CivilEscapingState::~CivilEscapingState()
{
}


// Enter the state
void CivilEscapingState::enter(CivilUnit *ref)
{
	// calculate a new path
	calculateRandomPath(ref);
}

// exit state
void CivilEscapingState::exit(CivilUnit *ref)
{

}

// update state
void CivilEscapingState::update(CivilUnit *ref)
{
	if(!ref->moveThroughPath()){
		ref->getFSM().newEvent(CivilUnit::E_DONE);
	}
}
