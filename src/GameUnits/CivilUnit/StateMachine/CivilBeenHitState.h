/*
 * CivilBeenHitState.h
 *
 *  Created on: 16/07/2012
 *      Author: agustin
 */

#ifndef CIVILBEENHITSTATE_H_
#define CIVILBEENHITSTATE_H_


#include "IState.h"
#include "CivilUnit.h"


class CivilBeenHitState : public IState<CivilUnit *>
{
public:
	CivilBeenHitState();
	virtual ~CivilBeenHitState();

	// Enter the state
	void enter(CivilUnit *ref);

	// exit state
	void exit(CivilUnit *ref);

	// update state
	void update(CivilUnit *ref);
};

#endif /* CIVILBEENHITSTATE_H_ */
