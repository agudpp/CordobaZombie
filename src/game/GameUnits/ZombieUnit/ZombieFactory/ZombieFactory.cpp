/*
 * ZombieFactory.cpp
 *
 *  Created on: 23/04/2012
 *      Author: agustin
 *
 */

#include <OgreMath.h>

#include "DebugUtil.h"
#include "ZombieUnit.h"
#include "ZombieFactory.h"


ZombieFactory::ZombieFactory() :
mQueue(0),
mName("Unnamed")
{

}

ZombieFactory::~ZombieFactory()
{

}


/**
 * Set the position of the zombie factory and the direction where the
 * zombies will be going to
 * @param	p	Position
 * @param	d	Direction
 */
void ZombieFactory::configure(const math::AABB &aabb,
		const Ogre::Vector3 &d, int N, ZombieQueue *zq)
{
	mAABB = aabb;
	mDirection = d;
	mCounter = N;
	mQueue = zq;
}

/**
 * Function used to create n zombies in the associated position and direction
 * @param	n	The number of zombies to create
 */
void ZombieFactory::create(int n)
{
	ZombieUnit *z = 0;
	for(int i = 0; (i < n) && (mCounter > 0); ++i){
		z = mQueue->getAvailableObject();
		if(!z){
			debug("No more zombies available\n");
			return;
		}
		const float x = Ogre::Math::RangeRandom(mAABB.tl.x, mAABB.br.x);
		const float y = Ogre::Math::RangeRandom(mAABB.br.y, mAABB.tl.y);
		// else.. we configure the zombie
		z->setPosition(math::Vector2(x,y));
		z->setDirection(mDirection);

		// advise that is alive again
		z->borning();
		--mCounter;
	}
}

/**
 * Function used to push a zombie into the factory
 */
void ZombieFactory::pushZombie(ZombieUnit *z)
{
	ASSERT(z);
	// we simply push the zombie into the queue
	mQueue->addNewObject(z);
}

