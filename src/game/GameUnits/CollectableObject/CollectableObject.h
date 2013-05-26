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
#include <OgreMaterial.h>

#include <DebugUtil/DebugUtil.h>

#include "GameUnits/GameObject.h"
#include "CollectableObjTypes.h"


class CollectableObject : public GameObject
{
public:
	CollectableObject(CollectableType type = CollectableType::COT_INVALID);
	~CollectableObject();

	/**
	 * Get/set the type
	 */
	inline void setCollectableType(CollectableType t);
	inline CollectableType collectableType(void) const;

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

	/**
	 * @brief Build the object with all the information we need
	 * @param entity    If we haven't set an entity we can set it here (if
	 *                  entity == 0, then we will use the current entity)
	 * @param node      If the node wasn't already set, we will use this one,
	 *                  if not, we will use the one that was set.
	 */
	void build(Ogre::Entity *entity = 0, Ogre::SceneNode *node = 0);

	//
	// Inherited methods
	//

    /**
     * This function have to be implemented that reproduce the effects when
     * the object is selected
     */
    virtual void objectSelected(void);

    /**
     * Function called when the object is not anymore selected
     */
    virtual void objectUnselected(void);

    /**
     * Function called when the mouse is over the object / unit
     */
    virtual void mouseOverObject(void);

    /**
     * Function called when the mouse is exit of the unit
     */
    virtual void mouseExitObject(void);

    /**
     * Object been hit by some other object
     * Shouldn't be called this method for this kind of objects for now
     */
    virtual void beenHit(const Hit_t &hit);


private:
	CollectableType mType;
	void *mObj;
	Ogre::MaterialPtr mMaterial;


	// we will use a pre-set material for those selected objects
	static Ogre::MaterialPtr sSelMaterial;
};



inline void
CollectableObject::setCollectableType(CollectableType t)
{
    mType = t;
}
inline CollectableType
CollectableObject::collectableType(void) const
{
    return mType;
}

inline void
CollectableObject::setObject(void *obj)
{
    ASSERT(obj);
    mObj = obj;
}
inline void *
CollectableObject::getObject(void) const
{
    return mObj;
}

#endif /* COLLECTABLEOBJECT_H_ */
