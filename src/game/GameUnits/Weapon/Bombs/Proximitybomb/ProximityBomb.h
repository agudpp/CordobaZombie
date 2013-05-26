/*
 * ProximityBomb.h
 *
 *  Created on: 02/05/2012
 *      Author: agustin
 */

#ifndef PROXIMITYBOMB_H_
#define PROXIMITYBOMB_H_

#include "Bomb.h"
#include "CollisionTypedefs.h"
#include "CommonMath.h"


class ProximityBomb : public Bomb
{
	static const int 	PROXIMITY_MASK = c_p::COL_FLAG_UNIT_ZOMBIE;
	static const float	CHECK_TIME;
public:
	ProximityBomb();
	virtual ~ProximityBomb();

	/**
	 * Set the "activation" time (the time that have to pass for the bomb be
	 * "activated").
	 * @param t		The time in seconds
	 *
	 * @note	This function is used to restart the counter too
	 */
	void setActivationTime(float t);

	/**
	 * Function called before the object start to bee updated
	 */
	virtual void beforeUpdate(void);

	/**
	 * Function to be implemented.
	 * This function is called every time the Manager is updated (every frame...)
	 */
	virtual void update(void);

	/**
	 * Function called when the object stop been updated
	 */
	virtual void updateStopped(void);


protected:
	float			mActivationTime;
	float			mAuxTime;
	core::AABB		mProximityQuery;
};

#endif /* PROXIMITYBOMB_H_ */
