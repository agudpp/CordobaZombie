/*
 * ZombieFollowSoundState.h
 *
 *  Created on: 2/07/2012
 *      Author: raul
 *
 */

#ifndef ZOMBIEFOLLOWSOUNDSTATE_H
#define ZOMBIEFOLLOWSOUNDSTATE_H


#include "FSM/IState.h"
#include "ZombieUnit.h"


class ZombieFollowSoundState  : public ia::IState<ZombieUnit *> {
public:
	ZombieFollowSoundState();
	virtual ~ZombieFollowSoundState();

	// Enter the state
	void enter(ZombieUnit *);

	// exit state
	void exit(ZombieUnit *);

	// update state
	void update(ZombieUnit *);

};


#endif /*ZOMBIEFOLLOWSOUNDSTATE_H*/
