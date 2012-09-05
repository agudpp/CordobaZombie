/*
 * ZombieBeenHitState.h
 *
 *  Created on: 28/02/2012
 *      Author: agustin
 *
 */

#ifndef ZOMBIEBEENHITSTATE_H_
#define ZOMBIEBEENHITSTATE_H_


#include "IState.h"
#include "ZombieUnit.h"

class ZombieBeenHitState : public IState<ZombieUnit *> {
public:
	ZombieBeenHitState();
	virtual ~ZombieBeenHitState();

	// Enter the state
	void enter(ZombieUnit *);

	// exit state
	void exit(ZombieUnit *);

	// update state
	void update(ZombieUnit *);
};

#endif /* ZOMBIEBEENHITSTATE_H_ */
