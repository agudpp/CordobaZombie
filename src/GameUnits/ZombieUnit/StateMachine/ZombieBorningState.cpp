/*
 * ZombieBorningState.cpp
 *
 *  Created on: 28/02/2012
 *      Author: agustin
 */

#include "ZombieBorningState.h"

ZombieBorningState::ZombieBorningState()
{

}

ZombieBorningState::~ZombieBorningState()
{

}


// Enter the state
void ZombieBorningState::enter(ZombieUnit *ref)
{
	// do nothing?
}

// exit state
void ZombieBorningState::exit(ZombieUnit *ref)
{
	// do nothing...?
}

// update state
void ZombieBorningState::update(ZombieUnit *ref)
{
	// probably we dont want to do anything...
	ref->getFSM().newEvent(ZombieUnit::E_DONE);
	return;
}
