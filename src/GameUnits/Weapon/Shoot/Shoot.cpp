/*
 * Shoot.cpp
 *
 *  Created on: 19/04/2012
 *      Author: agustin
 *
 */

#include "GlobalObjects.h"
#include "GameObject.h"

#include "Shoot.h"

CollisionManager	*Shoot::mCollMngr = 0;
ShootContainer Shoot::sContainer;
CollisionResult		Shoot::mCollResult;
Hit_t				Shoot::mHit;
const Ogre::Real Shoot::COEFF_FACTOR    = 200.0f;


/**
 * Get the first collision object that exists between p1 and p2
 */
const CollisionObject *Shoot::getCollObj(const sm::Vector2 &p1,
		const sm::Vector2 &p2)
{
	mCollMngr->getCollisionObjects(p1,p2,COLL_FLAG, mCollResult);

	// have collision?
	if(mCollResult.empty()) return 0;

	// get the closer object. TODO: aca tendriamos que usar el punto de collision
	// en vez de usar la posicion del objeto
	const CollisionObject *co = mCollResult.back();
	float minDist = p1.squaredDistance(co->getPosition());
	float aux;
	for(int i = mCollResult.size()-2; i >= 0; --i){
		aux = p1.squaredDistance(mCollResult[i]->getPosition());
		if(minDist > aux){
			minDist = aux;
			co = mCollResult[i];
		}
	}

	return co;
}


/**
 * Set the Collision manager to be used
 */
void Shoot::setCollisionManager(CollisionManager *cm)
{
	ASSERT(cm);
	mCollMngr = cm;
}


Shoot::Shoot() :
mNode(0),
mObj(0)
{
	sContainer.addNewObject(this);
}

Shoot::~Shoot()
{
	if(mNode){
		mNode->detachAllObjects();
		if(mNode->getParentSceneNode()){
			mNode->getParentSceneNode()->removeChild(mNode);
		}
		GLOBAL_SCN_MNGR->destroySceneNode(mNode);
		GLOBAL_SCN_MNGR->destroyManualObject(mObj);
	}
}

/**
 * Build the shoot
 * @param	size	The size of the shoot in units
 */
void Shoot::build(Ogre::Real size)
{
	ASSERT(size > 0.0f);
	ASSERT(!mNode);

	// create the manual object and the scene node
	mNode = GLOBAL_SCN_MNGR->createSceneNode();
	mObj = GLOBAL_SCN_MNGR->createManualObject();

	// TODO: tenemos que usar un material aca predeterminado
	mObj->begin("", Ogre::RenderOperation::OT_LINE_STRIP);

	Ogre::Vector3 p1(Ogre::Vector3::ZERO),p2(Ogre::Vector3::ZERO);
	p1.z = size * 0.5f;
	p2.z = -size * 0.5f;

	mObj->position(p1);  // start position
	mObj->position(p2);  // end position

	mObj->end();

	mNode->attachObject(mObj);
}

/**
 * Set the start position of the shoot
 */
void Shoot::setStartPos(const Ogre::Vector3 &p)
{
	ASSERT(mNode);
	mNode->setPosition(p);
}

/**
 * Set the movement direction (this will be the velocity too)
 */
void Shoot::setTranslateVec(const Ogre::Vector3 &t)
{
	mTranslateVec = t;
	mTranslateVec *= COEFF_FACTOR;

}

/**
 * Function called before the object start to bee updated
 */
void Shoot::beforeUpdate(void)
{
	ASSERT(mNode);
	ASSERT(mObj);
	ASSERT(mCollMngr);

	// check if we have to attach to the rootscenenode
	if(!mNode->getParentSceneNode()){
		GLOBAL_SCN_MNGR->getRootSceneNode()->addChild(mNode);
	}
	mNode->lookAt(mNode->getPosition() + mTranslateVec, Ogre::Node::TS_WORLD);
}

/**
 * Function to be implemented.
 * This function is called every time the Manager is updated (every frame...)
 */
void Shoot::update(void)
{
	// get the actual pos and the new position to raytrace..
	const Ogre::Vector3 &pos = mNode->getPosition();
	const sm::Vector2 oldP(pos.x, pos.z);

	// here we have to use the collision manager
	mNode->translate(mTranslateVec * GLOBAL_TIME_FRAME );

	// get the new pos
	sm::Vector2 shootPos(pos.x, pos.z);
//	ASSERT(shootPos.x != oldP.x || shootPos.y != oldP.y);

	// check if is still inside
	if(!mCollMngr->isPointInside(shootPos)){
		// we are outside of the level
		stopUpdate();
		return;
	}

	// get the first collision object
	const CollisionObject *co = getCollObj(oldP, shootPos);
	if(!co) return;

	// co is the closer objet.. that is what impact
	// TODO: implementar esto dependiendo contra quien impactamos
	mHit.power = mHitPower;
	mHit.hitDir.x = mTranslateVec.x;
	mHit.hitDir.y = mTranslateVec.z;

	if(co->userDefined){
		debug("Impact with object!\n");
		// TODO: estamos asegurando que co->userDefined es un collision Object
		GameObject *go = static_cast<GameObject *>(co->userDefined);
		go->beenHit(mHit);
	}

	// stop the update
	stopUpdate();
}

/**
 * Function called when the object stop been updated
 */
void Shoot::updateStopped(void)
{
	// detach it from root scene node
	mNode->getParentSceneNode()->removeChild(mNode);
	sContainer.addNewObject(this);
}
