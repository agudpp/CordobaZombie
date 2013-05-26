/*
 * PhysicsHelper.cpp
 *
 *  Created on: 10/07/2012
 *      Author: agustin
 */

#include "CollisionManager.h"
#include "LevelManager.h"
#include "CommonMath.h"
#include "PhysicsHelper.h"

#include <cmath>



const float  PhysicsHelper::GRAVITY_FACTOR      =   9.8f;
const float  PhysicsHelper::FLOOR_POSITION_Y    =   .0f;
const float  PhysicsHelper::NEGLIBILE_FORCE     =   0.1f;   // when we have force
                                                            // lower than this we
                                                            // avoid it



////////////////////////////////////////////////////////////////////////////////
bool PhysicsHelper::updateAllObject(PhysicObject &obj)
{
	ASSERT(obj.node);
	ASSERT(obj.collisionObject);

	if(obj.force == Ogre::Vector3::ZERO) return false;

	const float radius = obj.collisionObject->bb.getWidth();

	// check if we have to update the gravity (TODO: estamos usando width para
	// representar el "alto" de la bounding box... cacaso)
	if(obj.node->getPosition().y - radius >= FLOOR_POSITION_Y){
		// need to update the gravity
		obj.force.y += getGravityForce(obj);
	} else {
		// we first will repos the object to be in the floor
		reposFloorPosition(obj, radius);

		// we are impacting with the floor, must set bouncing/friction effect
		Ogre::Vector3 result;
		getReflectedVector(obj.force, Ogre::Vector3::UNIT_Y,result,obj.frictionCoeff);
		obj.force = result;

		// check if we have to cut the force
		if(std::abs(obj.force.x) <= NEGLIBILE_FORCE &&
				std::abs(obj.force.z) <= NEGLIBILE_FORCE &&
				std::abs(obj.force.y) <= NEGLIBILE_FORCE){
			obj.force = Ogre::Vector3::ZERO;

			debugBLUE("\n");
			// repose the object to be up in the floor
			reposFloorPosition(obj, radius);
			return false;
		}

	}

	// check bounce TODO:
	// tenemos que ver si realmente conviene esto o no
//	GLOBAL_LVL_MNGR->getCollisionManager()->get



	// update the position of the node using the force and update the collision
	// object position in the world
	obj.node->translate(obj.force);
	GLOBAL_LVL_MNGR->getCollisionManager()->translateObject(obj.collisionObject,
			math::Vector2(obj.force.x, obj.force.z));

	return true;
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsHelper::updateBounce(PhysicObject &obj, const Ogre::Vector3 &nsurf)
{

}

