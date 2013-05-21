/*
 * Test.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreMath.h>
#include <OgreAnimationState.h>

#include "GlobalObjects.h"
#include "Test.h"


const Ogre::Real Test::RANDOM_POSITION     = 2000.0f;



void Test::createLotOfZombies(int number)
{
	Ogre::Real r = 0/2.0f;
	Ogre::Entity *e = GLOBAL_SCN_MNGR->createEntity("Sphere.001.mesh");
    Ogre::SceneNode *n = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
    n->attachObject(e);
    n->setPosition(Ogre::Math::RangeRandom(-r, r), 10.0f,
				Ogre::Math::RangeRandom(-r,r));

    Ogre::AnimationStateSet *allAnim = e->getAllAnimationStates();
    Ogre::AnimationStateIterator it = allAnim->getAnimationStateIterator();

    while(it.hasMoreElements()){
        Ogre::AnimationState *anim = it.getNext();
        if (!anim) {
            break;
        }
        mAnims.push_back(anim);
    }
    mCurrentIndex = 0;
    // set enable the current anim
    mAnims.back()->setEnabled(true);

    // get the animations
//		Ogre::AnimationState *anim = e->getAnimationState("camina");
//		anim->setLoop(true);
//		anim->setEnabled(true);
//		mCamina.push_back(anim);

}


Test::Test() :
    AppTester(true)
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
	mActualAnim = mAnims[mCurrentIndex];
	if (mActualAnim == 0) {
	    return;
	}

	mActualAnim->addTime(GLOBAL_TIME_FRAME);

	if (mActualAnim->hasEnded()) {
	    mCurrentIndex = (mCurrentIndex+1) % mAnims.size();
	    mActualAnim->setEnabled(false);
        mActualAnim = mAnims[mCurrentIndex];;
        mActualAnim->setTimePosition(0);
        mActualAnim->setEnabled(true);
	}

}
