/*
 * PlayerAttackingState.h
 *
 *  Created on: 20/03/2012
 *      Author: agustin
 *
 */

#ifndef PLAYERATTACKINGSTATE_H_
#define PLAYERATTACKINGSTATE_H_



#include "IState.h"
#include "PlayerUnit.h"


class PlayerAttackingState : public IState<PlayerUnit *> {
public:
	PlayerAttackingState();
	virtual ~PlayerAttackingState();

	// Enter the state
	void enter(PlayerUnit *ref);

	// exit state
	void exit(PlayerUnit *ref);

	// update state
	void update(PlayerUnit *ref);
};

#endif /* PLAYERATTACKINGSTATE_H_ */
