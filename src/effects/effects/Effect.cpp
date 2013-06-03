/*
 * Effect.cpp
 *
 *  Created on: 07/08/2012
 *      Author: agustin
 */

#include "Effect.h"

#include <OgreSceneNode.h>

#include "DebugUtil.h"
#include "EffectsManager.h"
#include "GlobalObjects.h"


namespace effects {

EffectsManager		*Effect::mEffectsManager = 0;


void Effect::setEffectManager(EffectsManager *em)
{
	ASSERT(em);
	mEffectsManager = em;
}

Effect::Effect(EffectQueue *q, bool createSceneNode) :
		mSceneNode(0),
		mIndexPosition(0),
		mQueue(q)
{
	ASSERT(q);
	ASSERT(mEffectsManager);
	// create the SceneNode
	if(createSceneNode){
		mSceneNode = GLOBAL_SCN_MNGR->createSceneNode();
	}
}

Effect::~Effect()
{
	if(!mSceneNode) return;
	if(mSceneNode->getParentSceneNode()){
		GLOBAL_SCN_MNGR->getRootSceneNode()->removeChild(mSceneNode);
	}
	GLOBAL_SCN_MNGR->destroySceneNode(mSceneNode);

}


////////////////////////////////////////////////////////////////////////////////
void Effect::start(const Ogre::Vector3 &pos, const Ogre::Vector3 &dir)
{
	ASSERT(mEffectsManager);
	// check if we are already reproducing it
	if(mEffectsManager->hasEffect(this)) return;
	mEffectsManager->addEffect(this);
	beforeUpdate();

	// attach the sceneNode to the parentSceneNode
	if(!mSceneNode->getParentSceneNode()){
		GLOBAL_SCN_MNGR->getRootSceneNode()->addChild(mSceneNode);
	}
	mSceneNode->setPosition(pos);
	mSceneNode->setDirection(dir);
}

////////////////////////////////////////////////////////////////////////////////
void Effect::stop(void)
{
	ASSERT(mEffectsManager);
	if(mSceneNode->getParentSceneNode()){
		GLOBAL_SCN_MNGR->getRootSceneNode()->removeChild(mSceneNode);
	}
	// check if we are already reproducing it
	if(!mEffectsManager->hasEffect(this)) return;
	mEffectsManager->removeEffect(this);
	afterStop();
	mQueue->addNewObject(this);

}

////////////////////////////////////////////////////////////////////////////////
void Effect::show(void)
{
	ASSERT(mSceneNode);
	mSceneNode->setVisible(true);
}

////////////////////////////////////////////////////////////////////////////////
void Effect::hide(void)
{
	ASSERT(mSceneNode);
	mSceneNode->setVisible(false);
}

}
