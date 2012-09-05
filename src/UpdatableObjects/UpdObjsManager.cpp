/*
 * UpdObjsManager.cpp
 *
 *  Created on: 18/04/2012
 *      Author: agustin
 *
 */

#include "DebugUtil.h"
#include "UpdatableObject.h"
#include "UpdObjsManager.h"

/**
 * Add a new objetc
 */
void UpdObjsManager::addObject(UpdatableObject *obj)
{
	ASSERT(obj);
#ifdef DEBUG
	for(int i = mObjects.size()-1; i >= 0; --i){
		ASSERT(obj != mObjects[i]);
	}
#endif

	// add it
	obj->setID(mObjects.size());
	mObjects.push_back(obj);
	obj->beforeUpdate();
}

/**
 * Remove an objetc
 */
void UpdObjsManager::removeObject(UpdatableObject *obj)
{
	ASSERT(obj);
	ASSERT(mObjects.size() > obj->getID());
	ASSERT(mObjects[obj->getID()] == obj);

	// remove it
	mObjects[obj->getID()] = mObjects[mObjects.size()-1];
	mObjects[obj->getID()]->setID(obj->getID());
	mObjects.pop_back();
	obj->updateStopped();
}

/**
 * check if have an object
 */
bool UpdObjsManager::haveObject(UpdatableObject *obj)
{
	ASSERT(obj);
	if((mObjects.size() <= obj->getID()) || (mObjects[obj->getID()] != obj))
		return false;

	return true;
}

/**
 * Update all the objects
 */
void UpdObjsManager::updateAllObjects(void)
{
	for(int i = mObjects.size()-1; i >= 0; --i){
		mObjects[i]->update();
	}
}
