/*
 * CollectableObject.cpp
 *
 *  Created on: 29/05/2012
 *      Author: agustin
 */

#include "GlobalObjects.h"

#include "CollectableObject.h"

CollectableObject::CollectableObject(int type) :
mNode(0),
mEnt(0),
mObj(0),
mType(type)
{
	mNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
}

CollectableObject::~CollectableObject()
{
	ASSERT(mNode);

	if(mEnt)	GLOBAL_SCN_MNGR->destroyEntity(mEnt);
	if(mNode->getParentSceneNode()){
		mNode->getParentSceneNode()->removeAndDestroyChild(mNode->getName());
	} else {
		GLOBAL_SCN_MNGR->destroySceneNode(mNode);
	}

}




void CollectableObject::setEntity(Ogre::Entity *ent)
{
	ASSERT(ent);
	ASSERT(!mEnt);
	mEnt = ent;

	ASSERT(mNode);
	mNode->attachObject(ent);
}


/**
 * Function called when the object are picked or thrown
 */
void CollectableObject::objectPicked(void)
{
	ASSERT(mNode);
	if(mNode->getParentSceneNode()){
		mNode->getParentSceneNode()->removeChild(mNode);
	}
}
void CollectableObject::objectThrown(void)
{
	ASSERT(mNode);
	if(mNode->getParentSceneNode()) return;

	GLOBAL_SCN_MNGR->getRootSceneNode()->addChild(mNode);
}

