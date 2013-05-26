/*
 * RemoteBomb.h
 *
 *  Created on: 17/07/2012
 *      Author: agustin
 */

#ifndef REMOTEBOMB_H_
#define REMOTEBOMB_H_


#include "Bomb.h"
#include "CollisionTypedefs.h"

class RemoteBomb : public Bomb
{
public:
	RemoteBomb();
	virtual ~RemoteBomb();

	/**
	 * Blow the bomb now
	 */
	void blow(void);

	/**
	 * Check if the bomb has exploded
	 */
	inline bool hasExploded(void) const;

	/**
	 * Function called before the object start to bee updated
	 */
	virtual void beforeUpdate(void){};

	/**
	 * Function to be implemented.
	 * This function is called every time the Manager is updated (every frame...)
	 */
	virtual void update(void){};

	/**
	 * Function called when the object stop been updated
	 */
	virtual void updateStopped(void){};

private:
	bool		mExploded;
};

inline bool RemoteBomb::hasExploded(void) const
{
	return mExploded;
}





#endif /* REMOTEBOMB_H_ */
