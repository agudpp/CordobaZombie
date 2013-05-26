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




static double gettimestamp(void)
{
	struct timeval now;

	if(gettimeofday(&now, 0) < 0)
		return 0.0;

	double nowDouble = static_cast<double>(now.tv_sec + (
						static_cast<double>(now.tv_usec)/1000000.0));

	return nowDouble;
}








Test::Test()
{

}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}

// handle input
void Test::handleInput(void)
{

}


/* Load additional info */
void Test::loadAditionalData(void)
{
	mArma = GLOBAL_SCN_MNGR->createEntity("arma.mesh");
	mPlayer = GLOBAL_SCN_MNGR->createEntity("zombie01.mesh");;
	mIdle = mPlayer->getAnimationState("mueve1");
	mAtaca = mPlayer->getAnimationState("mueve2");
	mPlayer->attachObjectToBone("palma", mArma);

	mActualAnim = mIdle;

	GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode()->attachObject(mPlayer);
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
//	handleInput();
//	mTestUnit.update();
	static bool keyPres = false, keyPres2 = false;

	mActualAnim->addTime(GLOBAL_TIME_FRAME);

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_1)){
		if(!keyPres){
			keyPres = true;
			mAtaca->setEnabled(false);
			mIdle->setEnabled(true);
			mIdle->setLoop(true);
			mActualAnim = mIdle;
		}
	} else {
		keyPres = false;
	}

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_2)){
			if(!keyPres2){
				keyPres2 = true;
				mIdle->setEnabled(false);
				mAtaca->setEnabled(true);
				mAtaca->setLoop(true);
				mActualAnim = mAtaca;
			}
		} else {
			keyPres2 = false;
		}


}
