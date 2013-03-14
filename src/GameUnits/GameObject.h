/*
 * GameObject.h
 *
 *  Created on: 21/02/2012
 *      Author: agustin
 *
 */

#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_


#include <OgreSceneNode.h>
#include <OgreEntity.h>

#include <vector>

#include "CollisionObject.h"
#include "DebugUtil.h"
#include "CollisionManager.h"
#include "GameUnitDefines.h"
#include "SelectableObject.h"
#include "GlobalConstants.h"

class GameObject : public selection::SelectableObject {
public:
	// We have to set the collision manager used by this classes
	static CollisionManager *collMng;

public:
	GameObject();
	virtual ~GameObject();

	/**
	 * This function have to be implemented that reproduce the effects when
	 * the object is selected
	 */
	virtual void objectSelected(void) = 0;

	/**
	 * Function called when the object is not anymore selected
	 */
	virtual void objectUnselected(void) = 0;

	/**
	 * Function called when the mouse is over the object / unit
	 */
	virtual void mouseOverObject(void) = 0;

	/**
	 * Function called when the mouse is exit of the unit
	 */
	virtual void mouseExitObject(void) = 0;

	/* Object been hit by some other object */
	virtual void beenHit(const Hit_t &hit) = 0;

	/**
	 *  Set/Get the entity associated
	 */
	void setEntity(Ogre::Entity *ent);
	Ogre::Entity *getEntity(void) const {return mEntity;}

	/**
	 * Sets the height of the player in the Y Axis. This value will put the
	 * object h points over the floor
	 */
	inline void setHeight(float height);
	inline const Ogre::Real getHeight(void) const {return mNode->getPosition().y;}

	/**
	 *  Set/Get the scene node used by this entity
	 */
	void setSceneNode(Ogre::SceneNode *node);
	Ogre::SceneNode *getSceneNode(void) const {return mNode;}

	/**
	 * Returns the collision object
	 */
	CollisionObject &getCollisionObject(void) {return mCollObject;}

	/**
	 * Activate/Deactivate the collision checking
	 */
	void setActiveCollision(bool active);

	/**
	 * Set the position of the object in the world (collision and 3D World)
	 */
	void setPosition(const sm::Vector2 &p);
	void setPosition(const Ogre::Vector3 &p);
	inline const sm::Vector2 &getPosition(void) const;

	/**
	 * Translate unit in the world (3D and collision)
	 */
	void translate(const sm::Vector2 &t);
	void translate(const Ogre::Vector3 &t);

	/**
	 * Advance the GameObject in the direction that already have. This is something
	 * like translate but using local axis
	 */
	void advance(const sm::Vector2 &m);
	void advance(const Ogre::Vector3 &m);

	/**
	 * Rotate object over the xz plane (in radians)
	 */
	inline void rotateObject(Ogre::Real r);
	inline void rotateObject(const Ogre::Quaternion &q);
	inline void setDirection(const Ogre::Vector3 &d);
	inline void setDirection(const sm::Vector2 &d);

	/**
	 * Look object at certain point
	 */
	void lookAt(const Ogre::Vector3 &p);
	inline void lookAt(const sm::Vector2 &p);
	/**
	 * Get direction of the object (dir vector or radians of axis y)
	 */
	inline const Ogre::Quaternion &getOrientation(void) const;
	inline const Ogre::Radian getYaw(void) const;
	inline void getDirection(sm::Vector2 &dir) const;
	inline const sm::Vector2 &getDirection(void) const;

	/**
	 * Get the squared radius of the object (calculated from the AABB)
	 */
	inline float getSqrRadius(void) const;

	/**
	 * Perform and Get all the object that collide with this
	 */
	const CollisionResult &obtainCollisionObjects(void);
	inline const CollisionResult &getCollideObjects(void) const;

	/**
	 * Perform a AABB query to get a list of objects with certain mask_flag
	 * using the actual position of the object.
	 * @param mh		The middle height of the AABB to be used to get the objects
	 * @param mw		The middle width of the AABB
	 * @param mask		The mask to be used
	 * @param result	The list of objects that was found
	 * @note 	The AABB constructed will be (obj.pos.y +- mh, obj.pos.x +- mw)
	 */
	void getNearbyObjects(float mh, float mw, mask_t mask,
			std::vector<GameObject *> &result);



#ifdef DEBUG
public:
#else
protected:
#endif
	/**
	 *  Configure the collision object associated
	 *  @param w		The width of the AABB (for the collision system
	 *  				not that it will not rotate the AABB, it will
	 *  				always be axis aligned)
	 *	@param h		The height of the ABB (for the collision system)
	 *	@param mF 		The maskFlag used
	 *	@param mG		The mask group used
	 *	@note This function append the object to the collision manager, so
	 *	we need to call this function before do anything
	 */
	void configCollObj(float w, float h, uint32 mF, uint32 mG);

	/**
	 * Get the height and the width of the bounding box of the entity.
	 */
	void getAABBFromEntity(float &w, float &h);

	/**
	 * Function used to set the height of the player with the floor.
	 * The object will be positioned h points up the floor
	 */
	inline Ogre::Real getPosYAxis(void)
	{
		/*TODO: implementar bien, tenemos que hacer un raycast para ver
		 * a cuanto esta el piso o de alguna otra forma (dada por una matriz)
		 * o como sea que obtengamos la altura...
		 */
		return FLOOR_HEIGHT;
	}

private:
	GameObject(const GameObject &);


	/**
	 * Updates the actual direction
	 */
	inline void updateActualDirection(void);

protected:
	Ogre::Entity				*mEntity;
	Ogre::SceneNode				*mNode;
	CollisionObject				mCollObject;
	bool						mCollisionActive;
	float						mSqrRadius;
	sm::Vector2					mDirection;


	// the result of the collision will be put here for every objects, this is
	// not thread safe!!
	static CollisionResult		mCollisionResult;
	static std::vector<GameObject *>	mGameObjectResult;

};



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
inline const sm::Vector2 &GameObject::getPosition(void) const
{
	return mCollObject.getPosition();
}

////////////////////////////////////////////////////////////////////////////////
void GameObject::rotateObject(Ogre::Real r)
{
	ASSERT(mNode);
	mNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian(r));
	//mNode->yaw(r);
	updateActualDirection();
}
////////////////////////////////////////////////////////////////////////////////
void GameObject::rotateObject(const Ogre::Quaternion &q)
{
	ASSERT(mNode);
	mNode->rotate(q);
	updateActualDirection();
}
void GameObject::setDirection(const Ogre::Vector3 &d)
{
	ASSERT(mNode);
	mNode->setDirection(d);
	updateActualDirection();
}
void GameObject::setDirection(const sm::Vector2 &d)
{
	ASSERT(mNode);
	// TODO: verificar esto.... del mFloorHeight
	mNode->setDirection(d.x, getPosYAxis(), d.y);
	updateActualDirection();
}

////////////////////////////////////////////////////////////////////////////////
inline void GameObject::lookAt(const sm::Vector2 &p)
{
	lookAt(Ogre::Vector3(p.x, getPosYAxis(), p.y));

}


////////////////////////////////////////////////////////////////////////////////
const Ogre::Quaternion &GameObject::getOrientation(void) const
{
	ASSERT(mNode);
	return mNode->getOrientation();
}
////////////////////////////////////////////////////////////////////////////////

const Ogre::Radian GameObject::getYaw(void) const
{
	ASSERT(mNode);
	return mNode->getOrientation().getYaw();
}

////////////////////////////////////////////////////////////////////////////////
inline void GameObject::getDirection(sm::Vector2 &dir) const
{
	dir = mDirection;
}
inline const sm::Vector2 &GameObject::getDirection() const
{
	return mDirection;
}

////////////////////////////////////////////////////////////////////////////////
inline float GameObject::getSqrRadius(void) const
{
	return mSqrRadius;
}

////////////////////////////////////////////////////////////////////////////////
inline const CollisionResult &GameObject::getCollideObjects(void) const
{
	return mCollisionResult;
}

////////////////////////////////////////////////////////////////////////////////
inline void GameObject::updateActualDirection(void)
{
	ASSERT(mNode);
	Ogre::Vector3 d = mNode->getOrientation() * Ogre::Vector3::UNIT_Z;
	mDirection.x = d.x;
	mDirection.y = d.z;
	mDirection.normalize();
}

////////////////////////////////////////////////////////////////////////////////
inline void GameObject::setHeight(float height)
{
	ASSERT(mNode);
	const Ogre::Vector3 &pos = mNode->getPosition();
	mNode->setPosition(pos.x, height, pos.z);
}


#endif /* GAMEOBJECT_H_ */
