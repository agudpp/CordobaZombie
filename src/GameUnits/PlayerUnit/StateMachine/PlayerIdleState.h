/*
 * PlayerIdleState.h
 *
 *  Created on: 03/03/2012
 *      Author: agustin
 */

#ifndef PLAYERIDLESTATE_H_
#define PLAYERIDLESTATE_H_


#include "FSM/IState.h"
#include "PlayerUnit.h"


class PlayerIdleState : public ia::IState<PlayerUnit *>
{
public:
	PlayerIdleState();
	virtual ~PlayerIdleState();

	// Enter the state
	void enter(PlayerUnit *ref);

	// exit state
	void exit(PlayerUnit *ref);

	// update state
	void update(PlayerUnit *ref);
};

#endif /* PLAYERIDLESTATE_H_ */
