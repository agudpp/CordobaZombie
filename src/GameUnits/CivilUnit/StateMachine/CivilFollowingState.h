/*
 * CivilFollowingState.h
 *
 *  Created on: 16/07/2012
 *      Author: agustin
 */

#ifndef CIVILFOLLOWINGSTATE_H_
#define CIVILFOLLOWINGSTATE_H_

#include "IState.h"
#include "CivilUnit.h"


class CivilFollowingState : public IState<CivilUnit *>
{
public:
	CivilFollowingState();
	virtual ~CivilFollowingState();

	// Enter the state
	void enter(CivilUnit *ref);

	// exit state
	void exit(CivilUnit *ref);

	// update state
	void update(CivilUnit *ref);

private:
	// check if we are closer to the player
	inline bool closerUnit(CivilUnit *ref);
};


inline bool CivilFollowingState::closerUnit(CivilUnit *ref)
{
	return ref->getPosition().squaredDistance(
			ref->getFollowingUnit()->getPosition()) <= CivilUnit::SAFE_SQR_DIST;
}

#endif /* CIVILFOLLOWINGSTATE_H_ */
