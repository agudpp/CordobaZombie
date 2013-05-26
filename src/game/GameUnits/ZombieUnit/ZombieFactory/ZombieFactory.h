/*
 * ZombieFactory.h
 *
 *  Created on: 23/04/2012
 *      Author: agustin
 *
 */

#ifndef ZOMBIEFACTORY_H_
#define ZOMBIEFACTORY_H_

#include <OgreVector3.h>

#include "QueueContainer.h"
#include "CommonMath.h"

class ZombieUnit;

typedef QueueContainer<ZombieUnit>	ZombieQueue;

class ZombieFactory {
public:
	ZombieFactory();
	~ZombieFactory();

	/**
	 * Set/Get the name of the factory
	 */
	inline void setName(const Ogre::String &name);
	inline const Ogre::String &getName(void) const;

	/**
	 * Set the position of the zombie factory and the direction where the
	 * zombies will be going to
	 * @param	aabb	Box where will be created the zombies
	 * @param	d		Direction
	 * @param	N		the number of zombies that we want to create from this
	 * 					factory
	 * @param	zq		The zombie queue to use.. Probably the best to use is
	 * 					a big Queue where all the zombies will be putted and
	 * 					all the factory retrieves zombies from there
	 */
	void configure(const math::AABB &aabb, const Ogre::Vector3 &d, int N,
			ZombieQueue *zq);

	/**
	 * Function used to create n zombies in the associated position and direction
	 * @param	n	The number of zombies to create
	 */
	void create(int n);

	/**
	 * Function used to push a zombie into the factory
	 */
	void pushZombie(ZombieUnit *z);

	/**
	 * Get the Queue used
	 */
	inline ZombieQueue *getQueue(void);


private:
	math::AABB 			mAABB;
	Ogre::Vector3		mDirection;
	int					mCounter;
	ZombieQueue			*mQueue;
	Ogre::String		mName;

};


inline void ZombieFactory::setName(const Ogre::String &name)
{
	mName = name;
}
inline const Ogre::String &ZombieFactory::getName(void) const
{
	return mName;
}


inline ZombieQueue *ZombieFactory::getQueue(void)
{
	return mQueue;
}


#endif /* ZOMBIEFACTORY_H_ */
