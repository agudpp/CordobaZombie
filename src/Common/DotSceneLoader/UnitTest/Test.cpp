/*
 * Test.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreSceneNode.h>
#include <iostream>
#include <assert.h>


#include "Test.h"
#include "GlobalObjects.h"

Test::Test() :
	mTime(0)
{
	// TODO Auto-generated constructor stub

}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}


/* Load additional info */
void Test::loadAditionalData(void)
{
	Ogre::DotSceneLoader dsl;
	mNode = Common::GlobalObjects::sceneManager->createSceneNode();

	dsl.parseDotScene("escena_vidrio.scene", "Popular", Common::GlobalObjects::sceneManager, mNode);

	Common::GlobalObjects::sceneManager->getRootSceneNode()->addChild(mNode);

/*
	mAnimTrack = Common::GlobalObjects::sceneManager->getAnimationState("Dummy01_mueve");
	assert(mAnimTrack);

	// habilitamos la animacion y hacemos que sea loop
	mAnimTrack->setEnabled(true);
	mAnimTrack->setLoop(true);


	// attach the a boll to the "Camera01 Node"
	Ogre::Vector3 pNode;
	Ogre::SceneNode *aux;
	aux = Common::GlobalObjects::sceneManager->createSceneNode();
	aux->attachObject(Common::GlobalObjects::sceneManager->createEntity("node6",Ogre::SceneManager::PT_CUBE));

	//mCameraScnNode->getParent()->removeChild(mCameraScnNode);
	Common::GlobalObjects::sceneManager->getSceneNode("Dummy01")->addChild(aux);

//	Common::GlobalObjects::sceneManager->getSceneNode("Dummy01")->setPosition(0,0,0);
//	Common::GlobalObjects::sceneManager->getSceneNode("Dummy01")->setInitialState();
*/


}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	//mAnimTrack->addTime(Common::GlobalObjects::lastTimeFrame);

}
