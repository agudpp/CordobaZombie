/*
 * CollectableObject.h
 *
 *  Created on: 29/05/2012
 *      Author: agustin
 */

#ifndef COLLECTABLEOBJECT_H_
#define COLLECTABLEOBJECT_H_

#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreVector3.h>

#include "DebugUtil.h"
#include "CollectableObject.h"


class CollectableObject
{
public:
	CollectableObject(int type = -1);
	~CollectableObject();

	/**
	 * Set the entity
	 */
	void setEntity(Ogre::Entity *ent);

	/**
	 * Get the SceneNode/Entity
	 */
	inline Ogre::SceneNode *getNode(void);
	inline Ogre::Entity *getEntity(void);
	inline void setPosition(const Ogre::Vector3 &pos);

	/**
	 * Get/set the type
	 */
	inline void setType(int t);
	inline int getType(void) const;

	/**
	 * Set/get specified type object
	 */
	inline void setObject(void *obj);
	inline void *getObject(void) const;
	template<typename T>
	inline T getObject(void) const {return static_cast<T>(mObj);}

	/**
	 * Function called when the object are picked or thrown
	 */
	void objectPicked(void);
	void objectThrown(void);


private:
	Ogre::SceneNode		*mNode;
	Ogre::Entity		*mEnt;
	int					mType;
	void				*mObj;
};





/**
 * Get the SceneNode/Entity
 */
inline Ogre::SceneNode *CollectableObject::getNode(void)
{
	return mNode;
}
inline Ogre::Entity *CollectableObject::getEntity(void)
{
	return mEnt;
}
inline void CollectableObject::setPosition(const Ogre::Vector3 &pos)
{
	ASSERT(mNode);
	mNode->setPosition(pos);
}

inline void CollectableObject::setType(int t)
{
	mType = t;
}
inline int CollectableObject::getType(void) const
{
	return mType;
}

inline void CollectableObject::setObject(void *obj)
{
	ASSERT(obj);
	mObj = obj;
}
inline void *CollectableObject::getObject(void) const
{
	return mObj;
}

#endif /* COLLECTABLEOBJECT_H_ */
