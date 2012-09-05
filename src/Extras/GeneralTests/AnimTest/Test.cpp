/*
 * Test.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreMath.h>
#include "GlobalObjects.h"
#include "Test.h"


void Test::loadAnimations(void)
{
	Ogre::AnimationState *anim;
	try{
		anim = ent->getAnimationState("anim1");
		anim->setEnabled(false);
		anim->setLoop(false);
		mAnims.push_back(anim);

		anim = ent->getAnimationState("anim2");
		anim->setEnabled(false);
		anim->setLoop(false);
		mAnims.push_back(anim);

		anim = ent->getAnimationState("anim3");
		anim->setEnabled(false);
		anim->setLoop(false);
		mAnims.push_back(anim);

		anim = ent->getAnimationState("anim4");
		anim->setEnabled(false);
		anim->setLoop(false);
		mAnims.push_back(anim);

	} catch(...){
		debug("The animation dosn't exists %s\n", "....");
		ASSERT(false);
	}

}

void Test::createLotOfZombies(int number)
{
	Ogre::Real r = RANDOM_POSITION/2.0f;
	for(int i = 0; i < number; ++i){
		Ogre::Entity *e = GLOBAL_SCN_MNGR->createEntity("zombie01.mesh");
		Ogre::SceneNode *n = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
		n->attachObject(e);
		n->setPosition(Ogre::Math::RangeRandom(-r, r), 10.0f,
				Ogre::Math::RangeRandom(-r,r));

		Ogre::AnimationState *anim = e->getAnimationState("camina");
		anim->setLoop(true);
		anim->setEnabled(true);
		mCamina.push_back(anim);
	}
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
//	ent = GLOBAL_SCN_MNGR->createEntity("prueba.mesh");
//	node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
//	node->attachObject(ent);
//
//	loadAnimations();
//	ASSERT(mAnims.size() == 4);
//	mActualAnim = mAnims[0];

	createLotOfZombies(1850);

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	static bool keyPressed = false;
//	mActualAnim->addTime(GLOBAL_TIME_FRAME);

	for(int i = mCamina.size()-1; i >= 0; --i){
		mCamina[i]->addTime(GLOBAL_TIME_FRAME);
	}

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_1)){
		if(!keyPressed){
			keyPressed = true;
			changeAnim(0);
		}
	} else if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_2)){
		if(!keyPressed){
			keyPressed = true;
			changeAnim(1);
		}
	}else if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_3)){
		if(!keyPressed){
			keyPressed = true;
			changeAnim(2);
		}
	}else if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_4)){
		if(!keyPressed){
			keyPressed = true;
			changeAnim(3);
		}
	} else {
		keyPressed = false;
	}
}
