/*
 * PlayerReloadingState.h
 *
 *  Created on: 29/04/2012
 *      Author: agustin
 */

#ifndef PLAYERRELOADINGSTATE_H_
#define PLAYERRELOADINGSTATE_H_

#include "FSM/IState.h"
#include "PlayerUnit.h"

class PlayerReloadingState : public ia::IState<PlayerUnit *>
{
public:
	PlayerReloadingState();
	virtual ~PlayerReloadingState();

	// Enter the state
	void enter(PlayerUnit *ref);

	// exit state
	void exit(PlayerUnit *ref);

	// update state
	void update(PlayerUnit *ref);
};

#endif /* PLAYERRELOADINGSTATE_H_ */
