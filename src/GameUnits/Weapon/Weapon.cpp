/*
 * Weapon.cpp
 *
 *  Created on: 12/03/2012
 *      Author: agustin
 */

#include "Weapon.h"
#include "GameUnit.h"


CollisionResult	Weapon::mCollResult;
ShootContainer	*Weapon::mShootCont = Shoot::getContainer();


////////////////////////////////////////////////////////////////////////////////
void Weapon::setOwner(GameUnit *u)
{
	ASSERT(u);
	mOwner = u;

	/* Try to attach the GameUnit's SceneNode to the SoundAPI.   *
	 * Weapon::setOwner() is supposed to be called by an already *
	 * active Player GameUnit, with a non-NULL SceneNode.        */
	Ogre::SceneNode* node = u->getSceneNode();
	ASSERT(node != 0);
	mSAPI.setSceneNode(node);
}


Weapon::Weapon(int wtype, unsigned short id) :
mEntity(0),
mType(wtype),
mOwner(0),
mAnimVelocityFactor(1.0f),
mID(id)
{
	// TODO Auto-generated constructor stub

}

Weapon::~Weapon()
{
	// TODO Auto-generated destructor stub
}


/**
 * Second shoot always can be performed
 */
void
Weapon::secondShoot(const sm::Vector2 &p)
{
	ASSERT(mOwner);

	if(mOwner->getPosition().squaredDistance(p) > mOwner->getSqrRadius()){
		return;
	}

	// TODO: tendriamos que ver de cambiar los flags aca si queremos por ejemplo
	// poder romper una caja? no es un zombie... asique deberiamos poder pegarle
	GLOBAL_LVL_MNGR->getCollisionManager()->getCollisionObjects(p,
			COL_FLAG_UNIT_ZOMBIE, mCollResult);

	if(mCollResult.empty()){
		debug("No object found\n");
		return;
	}

	// else we have impact something, get the closest
	GameUnit *gu = static_cast<GameUnit *>(mCollResult[0]->userDefined);
	float minDist = gu->getPosition().squaredDistance(p);
	for(int i = mCollResult.size(); i >= 1; --i){
		const float dist = mCollResult[i]->getPosition().squaredDistance(p);
		if(dist < minDist){
			gu = static_cast<GameUnit *>(mCollResult[i]->userDefined);
		}
	}

	// gu is the closer game unit (zombie)... hit the mother fucker
	mHit.hitDir = p;
	mHit.hitDir -= mOwner->getPosition();
	mHit.power = mPower;
	mHit.shooter = 0;
	gu->beenHit(mHit);

	return;
}

Weapon::Connection
Weapon::addCallback(const Signal::slot_type& subscriber)
{
    return mSignal.connect(subscriber);
}

