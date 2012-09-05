/*
 * Test.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreSceneNode.h>
#include <iostream>
#include <assert.h>
#include <OgreSceneQuery.h>
#include <OgreRay.h>
#include <OgreManualObject.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreString.h>

#include "GlobalObjects.h"
#include "Test.h"


Test::Test()
{
	setUseDefaultInput(false);
	mMouseCursor.setImage("cursor.png");
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight());

}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}

// handle input
void Test::handleInput(void)
{
	if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) {
		// we have to exit
		mStopRunning = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// CAMERA
//	float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
	Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;
	Ogre::Real zoom = 0.0f;

	// HERE WE DEFINE THE KEYS USED TO MOVE THE CAMERA, WE WILL USE THE
	// ARROWS TO MOVE THE CAMERA
	// NOTE: we are getting the cameraPosition and modifying the position
	// without using translation, this is because we want to move always
	// in the same axis whatever be the direction of the camera.


	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	if(mKeyboard->isKeyDown(OIS::KC_LEFT) || mKeyboard->isKeyDown(OIS::KC_A) ||
			lMouseState.X.abs <= 0)
	{
		mTranslationVec.x -= 1.0f;
	}
	if(mKeyboard->isKeyDown(OIS::KC_RIGHT) || mKeyboard->isKeyDown(OIS::KC_D) ||
			lMouseState.X.abs >= lMouseState.width)
	{
		mTranslationVec.x += 1.0f;
	}
	if(mKeyboard->isKeyDown(OIS::KC_Q))
	{
		zoom += 1;
	}
	if(mKeyboard->isKeyDown(OIS::KC_E))
	{
		zoom -= 1;
	}
	if(mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W) ||
			lMouseState.Y.abs <= 0)
	{
		mTranslationVec.z -= 1.0f;
	}
	if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) ||
			lMouseState.Y.abs >= lMouseState.height)
	{
		mTranslationVec.z += 1.0f;
	}

	if(mTranslationVec != Ogre::Vector3::ZERO)
	{
		mCamController.moveCamera(mTranslationVec);
	}
	if(zoom != 0.0f){
		mCamController.zoomCamera(zoom);
	}

	if(mKeyboard->isKeyDown(OIS::KC_R))
	{
		mCamController.rotateCameraY(Ogre::Radian(-1));
	}
	if(mKeyboard->isKeyDown(OIS::KC_T))
	{
		mCamController.rotateCameraY(Ogre::Radian(1));
	}

	if(mKeyboard->isKeyDown(OIS::KC_Z))
	{
		mCamController.rotateCameraX(Ogre::Radian(-1));
	}
	if(mKeyboard->isKeyDown(OIS::KC_X))
	{
		mCamController.rotateCameraX(Ogre::Radian(1));
	}

}


/* Load additional info */
void Test::loadAditionalData(void)
{
	//	GLOBAL_CAMERA->getParentSceneNode()->detachAllObjects();
	GLOBAL_CAMERA->detachFromParent();
	mCamController.setCamera(GLOBAL_CAMERA);

	Ogre::AxisAlignedBox box;
	box.setMaximum(2500, 1800, 2500);
	box.setMinimum(-2500,0,-2500);


	mCamController.setCameraMoveZone(box);
	mCamController.setMaxRotateX(Ogre::Radian(0), Ogre::Radian((-Ogre::Math::PI-0.1f)/2.0f));
	mCamController.setCamPos(Ogre::Vector3(500,500,500));
	mCamController.setRotationX(Ogre::Radian(-Ogre::Math::PI/2.0f));
	mCamController.setCameraVelocity(5.0f);


}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	handleInput();
}
