/*
 * InputActionObject.h
 *
 *  Created on: 04/08/2012
 *      Author: agustin
 *
 */

#ifndef INPUTACTIONOBJECT_H_
#define INPUTACTIONOBJECT_H_

#include <OgreString.h>

#include "CollisionTypedefs.h"

namespace Ogre {
class Vector3;
}
class GameObject;

// Define the InputSystem Action interface
class ISAction {
public:
	/**
	 * Function called when we are handling objects
	 */
	virtual void operator()(GameObject *obj) = 0;

	/**
	 * Function called when we are handling positions
	 */
	virtual void operator()(const Ogre::Vector3 &pos) = 0;
};

class InputActionObject {
public:
	inline InputActionObject();

	/**
	 * Accessors / setters functions
	 */
	inline void setAction(ISAction *a);
	inline ISAction *getAction(void);

	inline void setCollisionType(mask_t t);
	inline mask_t getCollisionType(void);

private:
	ISAction	*mAction;
	mask_t		mType;
};


inline InputActionObject::InputActionObject() :
		mAction(0),
		mType(0)
{
}

/**
 * Accessors / setters functions
 */
inline void InputActionObject::setAction(ISAction *a)
{
	mAction = a;
}
inline ISAction *InputActionObject::getAction(void)
{
	return mAction;
}

inline void InputActionObject::setCollisionType(mask_t t)
{
	mType = t;
}
inline mask_t InputActionObject::getCollisionType(void)
{
	return mType;
}

#endif /* INPUTACTIONOBJECT_H_ */
