/*
 * PhysicsHelper.h
 *
 *  Created on: 10/07/2012
 *      Author: agustin
 */

#ifndef PHYSICSHELPER_H_
#define PHYSICSHELPER_H_

#include "PhysicObject.h"
#include "GlobalObjects.h"
#include "DebugUtil.h"

class PhysicsHelper
{
	static const float	GRAVITY_FACTOR		=	9.8f;
	static const float	FLOOR_POSITION_Y	=	.0f;
	static const float	NEGLIBILE_FORCE		=	0.1f;	// when we have force
														// lower than this we
														// avoid it
public:
	/**
	 * Update the collision and position of the object if we have to. We will
	 * check all the other objects that obj "collide" using the collisionSystem
	 * and we will move the object down (gravity)
	 * This will update the position of the SceneNode to.
	 * @param	obj		The object to be updated
	 * @return	true	if we have to continue updating the object.
	 * 			false	if the object is not needed to be updated anymore.
	 */
	static bool updateAllObject(PhysicObject &obj);

private:
	/**
	 * Calculate new direction of the object once the object "collide" and
	 * "bounce" against some surface.
	 * @param	obj		The object that will bounce
	 * @param	nsurf	The normal vector of the surface.
	 * @note	This will modify the object properties (including SceneNode)
	 */
	static void updateBounce(PhysicObject &obj, const Ogre::Vector3 &nsurf);

	/**
	 * Update the gravity of the object (modifies the SceneNode)
	 */
	static inline float getGravityForce(PhysicObject &obj);

	/**
	 * Reposition the object to be above the floor (using its radius).
	 */
	static inline void reposFloorPosition(PhysicObject &obj, const float radius);

	/**
	 * Calculate reflecting vector from a vector in and a normal N.
	 * @param	in		Input vector
	 * @param	n		Normal surface vector
	 * @param	result	The resulting vector
	 */
	static inline void getReflectedVector(const Ogre::Vector3 &in,
			const Ogre::Vector3 &n, Ogre::Vector3 &result, const float b);
};


inline float PhysicsHelper::getGravityForce(PhysicObject &obj)
{
	return obj.weight * -GRAVITY_FACTOR * GLOBAL_TIME_FRAME;
}


inline void PhysicsHelper::reposFloorPosition(PhysicObject &obj, const float radius)
{
	ASSERT(obj.node);
	const Ogre::Vector3 &ap = obj.node->getPosition();
	obj.node->setPosition(ap.x, FLOOR_POSITION_Y + radius, ap.z);
}

/**
 * Calculate reflecting vector from a vector in and a normal N.
 * @param	in		Input vector
 * @param	n		Normal surface vector
 * @param	result	The resulting vector
 * @param	b		Bouncing factor (frictionCoeff).
 * 					If b = 1.0f ==> perfect bouncing.
 * 					If b = 0.0f ==> NO bounce.
 */
inline void PhysicsHelper::getReflectedVector(const Ogre::Vector3 &in,
		const Ogre::Vector3 &n, Ogre::Vector3 &result, const float b)
{
//	Vnew = -2*(V dot N)*N + V
//	Vnew = b * ( -2*(V dot N)*N + V )
	result = -2.0f*(in.dotProduct(n))*n + in;
	result *= b;
}

#endif /* PHYSICSHELPER_H_ */
