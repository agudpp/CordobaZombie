/*
 * Shoot.h
 *
 *  Created on: 19/04/2012
 *      Author: agustin
 *
 */

#ifndef SHOOT_H_
#define SHOOT_H_

#include <OgreSceneNode.h>
#include <OgreManualObject.h>
#include <OgreVector3.h>

#include "UpdatableObject.h"
#include "CollisionManager.h"
#include "QueueContainer.h"
#include "CollisionTypedefs.h"


// Define the container of the shoots
class Shoot;
typedef QueueContainer<Shoot>	ShootContainer;


class Shoot : public UpdatableObject {
	static const Ogre::Real COEFF_FACTOR;
	static const int COLL_FLAG	=	SHOOT_AFFECTABLE_MASK;
public:
	/**
	 * The constructor automatically add this shoot to the container
	 */
	Shoot();
	virtual ~Shoot();

	/**
	 * Build the shoot
	 * @param	size	The size of the shoot in units
	 */
	void build(Ogre::Real size);

	/**
	 * Set the start position of the shoot
	 */
	void setStartPos(const Ogre::Vector3 &p);

	/**
	 * Set the power
	 */
	inline void setHitPower(float p);

	/**
	 * Set the movement direction (this will be the velocity too)
	 */
	void setTranslateVec(const Ogre::Vector3 &t);
	inline const Ogre::Vector3 &getTranslateVec(void) const;

	/**
	 * Function called before the object start to bee updated
	 */
	void beforeUpdate(void);

	/**
	 * Function to be implemented.
	 * This function is called every time the Manager is updated (every frame...)
	 */
	void update(void);

	/**
	 * Function called when the object stop been updated
	 */
	void updateStopped(void);

	/**
	 * Set the Collision manager to be used
	 */
	static void setCollisionManager(CollisionManager *cm);

	/**
	 * Set the Shootcontainer to use
	 */
	static inline ShootContainer *getContainer(void);


protected:
	/**
	 * Get the first collision object that exists between p1 and p2
	 */
	const CollisionObject *getCollObj(const sm::Vector2 &p1,
			const sm::Vector2 &p2);


protected:
	Ogre::SceneNode			*mNode;
	Ogre::ManualObject		*mObj;
	Ogre::Vector3			mTranslateVec;
	float					mHitPower;

	static CollisionResult	mCollResult;
	static CollisionManager	*mCollMngr;
	static ShootContainer sContainer;
	static Hit_t			mHit;
};


inline void
Shoot::setHitPower(float p)
{
	mHitPower = p;
}

inline const Ogre::Vector3 &
Shoot::getTranslateVec(void) const
{
	return mTranslateVec;
}

inline ShootContainer *
Shoot::getContainer(void)
{
    return &sContainer;
}

#endif /* SHOOT_H_ */
