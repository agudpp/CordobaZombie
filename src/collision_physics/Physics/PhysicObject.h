/*
 * PhysicObject.h
 *
 *  Created on: 10/07/2012
 *      Author: agustin
 */

#ifndef PHYSICOBJECT_H_
#define PHYSICOBJECT_H_

#include <OgreVector3.h>
#include <OgreSceneNode.h>
#include <CollisionObject.h>

namespace c_p {

struct PhysicObject
{
	CollisionObject	*collisionObject;
	float			frictionCoeff;
	float			weight;
	Ogre::Vector3	force;
	Ogre::SceneNode	*node;

	PhysicObject() :
		collisionObject(0),
		frictionCoeff(1.0f),
		weight(0),
		force(Ogre::Vector3::ZERO),
		node(0)
	{}
};

}

#endif /* PHYSICOBJECT_H_ */
