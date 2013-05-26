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

#include "GlobalObjects.h"
#include "Test.h"
#include <DotSceneLoader.h>
#include <Util.h>
#include <iostream>

// handle input
void Test::handleInput(void)
{
	Ogre::Vector3 trans = Ogre::Vector3::ZERO;
	const float factor = 1;
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_T)){
		trans.z += factor;
	}
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_G)){
		trans.z -= factor;
	}
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_F)){
		trans.x += factor;
	}
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_H)){
		trans.x -= factor;
	}
	mEntNode->translate(trans * GLOBAL_TIME_FRAME * 200.0f);

}

Test::Test()
{

}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}


/* Load additional info */
void Test::loadAditionalData(void)
{
	Ogre::Entity *coral = GLOBAL_SCN_MNGR->createEntity("coral.mesh");
	mEntNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	mEntNode->attachObject(coral);

	Ogre::DotSceneLoader dsl;
	dsl.parseDotScene("scene.scene", "Essential", GLOBAL_SCN_MNGR);

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	const Ogre::Vector3 &pos = mBodySceneNode->getPosition();
	std::cout <<  "Pos X: " << pos.x << "\tPos Y: " << pos.y << "\tPos Z: "  <<
			pos.z << std::endl;

	handleInput();
}
