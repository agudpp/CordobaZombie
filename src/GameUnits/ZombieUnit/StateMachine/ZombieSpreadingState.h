/*
 * ZombieSpreadingState.h
 *
 *  Created on: 04/07/2012
 *      Author: raul
 *
 */

#ifndef ZOMBIESPREADINGSTATE_H_
#define ZOMBIESPREADINGSTATE_H_


#include "FSM/IState.h"
#include "ZombieUnit.h"

class ZombieSpreadingState  : public ia::IState<ZombieUnit *> {
public:
	ZombieSpreadingState();
	virtual ~ZombieSpreadingState();

	// Enter the state
	void enter(ZombieUnit *);

	// exit state
	void exit(ZombieUnit *);

	// update state
	void update(ZombieUnit *);
private:

};

#endif
