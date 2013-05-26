/*
 * RemoteBombTrigger.h
 *
 *  Created on: 17/07/2012
 *      Author: agustin
 */

#ifndef REMOTEBOMBTRIGGER_H_
#define REMOTEBOMBTRIGGER_H_

#include "RemoteBomb.h"
#include "DebugUtil.h"

class RemoteBombTrigger
{
public:
	RemoteBombTrigger(RemoteBomb *bomb) : mBomb(bomb){};
	~RemoteBombTrigger();

	/**
	 * Trigger the bomb
	 */
	inline void triggerBomb(void);

	/**
	 * Bomb was triggered
	 */
	inline bool bombWasTriggered(void) const;

private:
	RemoteBomb	*mBomb;
};

/**
 * Trigger the bomb
 */
inline void RemoteBombTrigger::triggerBomb(void)
{
	ASSERT(mBomb);
	mBomb->blow();
}

/**
 * Bomb was triggered
 */
inline bool RemoteBombTrigger::bombWasTriggered(void) const
{
	ASSERT(mBomb);
	return mBomb->hasExploded();
}

#endif /* REMOTEBOMBTRIGGER_H_ */
