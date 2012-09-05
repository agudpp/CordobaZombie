/*
 * CameraAnimUpdater.cpp
 *
 *  Created on: 09/05/2012
 *      Author: agustin
 *
 */

#include "DebugUtil.h"

#include "GlobalObjects.h"
#include "CameraAnimUpdater.h"

CameraAnimUpdater::CameraAnimUpdater() :
mNode(0)
{

}

CameraAnimUpdater::~CameraAnimUpdater()
{

}


void CameraAnimUpdater::setSceneNode(Ogre::SceneNode *n)
{
	ASSERT(n);
	mNode = n;
}

/**
 * Set Animation
 */
void CameraAnimUpdater::startAnimation(Ogre::AnimationState *anim)
{
	ASSERT(anim);
	if(mAnim){
		// we are actually reproducing
		debug("We are already reproducing an animation %s\n",
				mAnim->getAnimationName().c_str());
		return;
	}

	// else we reproduce this one
	mAnim = anim;
	startUpdate();
}

/**
 * Function called before the object start to bee updated
 */
void CameraAnimUpdater::beforeUpdate(void)
{
	ASSERT(mAnim);
	ASSERT(mNode);

	// save the position
	mNode->setInitialState();

	mAnim->setTimePosition(0);
	mAnim->setLoop(false);
	mAnim->setEnabled(true);
}

/**
 * Function to be implemented.
 * This function is called every time the Manager is updated (every frame...)
 */
void CameraAnimUpdater::update(void)
{
	mAnim->addTime(GLOBAL_TIME_FRAME);
	if(mAnim->hasEnded()){
		stopUpdate();
	}
}

/**
 * Function called when the object stop been updated
 */
void CameraAnimUpdater::updateStopped(void)
{
	mAnim->setEnabled(false);
	mAnim = 0;

	// reset the position / orientation / etc
	mNode->resetToInitialState();
}
