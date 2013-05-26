/*
 * CivilIdleState.h
 *
 *  Created on: 16/07/2012
 *      Author: agustin
 */

#ifndef CIVILIDLESTATE_H_
#define CIVILIDLESTATE_H_

#include "FSM/IState.h"
#include "CivilUnit.h"

class CivilIdleState : public ia::IState<CivilUnit *>
{
public:
	CivilIdleState();
	virtual ~CivilIdleState();

	// Enter the state
	void enter(CivilUnit *ref);

	// exit state
	void exit(CivilUnit *ref);

	// update state
	void update(CivilUnit *ref);
};

#endif /* CIVILIDLESTATE_H_ */
