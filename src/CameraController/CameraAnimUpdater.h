/*
 * CameraAnimUpdater.h
 *
 *  Created on: 09/05/2012
 *      Author: agustin
 *
 */

#ifndef CAMERAANIMUPDATER_H_
#define CAMERAANIMUPDATER_H_

#include <OgreAnimationState.h>
#include <OgreSceneNode.h>

#include "UpdatableObject.h"

class CameraAnimUpdater : public UpdatableObject {
public:
	CameraAnimUpdater();
	virtual ~CameraAnimUpdater();

	/**
	 * Set the scene node that will be animated
	 */
	void setSceneNode(Ogre::SceneNode *n);

	/**
	 * Set Animation
	 */
	void startAnimation(Ogre::AnimationState *anim);

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

private:
	Ogre::AnimationState	*mAnim;
	Ogre::SceneNode			*mNode;

};


#endif /* CAMERAANIMUPDATER_H_ */
