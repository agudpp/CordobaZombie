/*
 * SpecialAction.h
 *
 *  Created on: 27/06/2012
 *      Author: agustin
 */

#ifndef SPECIALACTION_H_
#define SPECIALACTION_H_

#include <OgreVector3.h>

#include "UpdatableObject.h"

class SpecialAction : public UpdatableObject
{
public:
	SpecialAction();
	virtual ~SpecialAction();

	/**
	 * Set the position where we will execute the special action (this is the
	 * position selected by the user when click it).
	 * @param	pos		The position
	 */
	inline void setPosition(const Ogre::Vector3 &pos);

	/**
	 * Function called before the object start to bee updated
	 */
	virtual void beforeUpdate(void) = 0;

	/**
	 * Function to be implemented.
	 * This function is called every time the Manager is updated (every frame...)
	 */
	virtual void update(void) = 0;

	/**
	 * Function called when the object stop been updated
	 */
	virtual void updateStopped(void) = 0;

protected:
	Ogre::Vector3		mPosition;

};


////////////////////////////////////////////////////////////////////////////////
inline void SpecialAction::setPosition(const Ogre::Vector3 &pos)
{
	mPosition = pos;
}

#endif /* SPECIALACTION_H_ */
