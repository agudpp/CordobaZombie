/*
 * CivilEscapingState.h
 *
 *  Created on: 16/07/2012
 *      Author: agustin
 */

#ifndef CIVILESCAPINGSTATE_H_
#define CIVILESCAPINGSTATE_H_


#include "FSM/IState.h"
#include "CivilUnit.h"


class CivilEscapingState : public ia::IState<CivilUnit *>
{
public:
	CivilEscapingState();
	virtual ~CivilEscapingState();

	// Enter the state
	void enter(CivilUnit *ref);

	// exit state
	void exit(CivilUnit *ref);

	// update state
	void update(CivilUnit *ref);

private:
	/**
	 * Calculates a random path to go away running
	 */
	void calculateRandomPath(CivilUnit *ref);
};



#endif /* CIVILESCAPINGSTATE_H_ */
