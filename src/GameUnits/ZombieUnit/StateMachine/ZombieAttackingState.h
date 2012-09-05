/*
 * ZombieAttackingState.h
 *
 *  Created on: 28/02/2012
 *      Author: agustin
 *
 */

#ifndef ZOMBIEATTACKINGSTATE_H_
#define ZOMBIEATTACKINGSTATE_H_


#include "IState.h"
#include "ZombieUnit.h"

class ZombieAttackingState  : public IState<ZombieUnit *> {
public:
	ZombieAttackingState();
	virtual ~ZombieAttackingState();

	// Enter the state
	void enter(ZombieUnit *);

	// exit state
	void exit(ZombieUnit *);

	// update state
	void update(ZombieUnit *);

private:
	inline void performAttack(ZombieUnit *ref);
};





////////////////////////////////////////////////////////////////////////////////
inline void ZombieAttackingState::performAttack(ZombieUnit *ref)
{
	ASSERT(ref->getActualTarget());
	// TODO: perform the attack here
	Hit_t h;

	h.hitDir = ref->getActualTarget()->getPosition();
	h.hitDir -= ref->getPosition();
	h.power = ref->getAttackPower();
	h.shooter = ref;
	ref->getActualTarget()->beenHit(h);

	// Play attacking sounds
	const Ogre::String* hitSound(ref->mSounds.getRandomSound(ZombieUnit::SS_HIT_CODE));
	if (hitSound) {
		ref->mSAPI.stop();
		ref->mSAPI.play(*hitSound);
	} else {
		debugWARNING("No zombie hit sound.%s", "\n");
	}
}


#endif /* ZOMBIEATTACKINGSTATE_H_ */
