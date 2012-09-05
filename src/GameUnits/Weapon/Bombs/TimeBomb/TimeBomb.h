/*
 * TimeBomb.h
 *
 *  Created on: 02/05/2012
 *      Author: agustin
 */

#ifndef TIMEBOMB_H_
#define TIMEBOMB_H_

#include "Bomb.h"

class TimeBomb : public Bomb
{
public:
	TimeBomb();
	virtual ~TimeBomb();

	/**
	 * Set the time that this bomb have to wait to blow up
	 */
	void setBlowTime(float t);

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
	float			mBlowTime;
};

#endif /* TIMEBOMB_H_ */
