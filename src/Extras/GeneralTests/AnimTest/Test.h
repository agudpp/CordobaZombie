/*
 * Test.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef TEST_H_
#define TEST_H_

#include <OgreSceneNode.h>
#include <OgreAnimation.h>
#include <OgreAnimationTrack.h>
#include <OgreAnimationState.h>

#include <vector>

#include "AppTester.h"
#include "DebugUtil.h"



class Test : public AppTester
{
	const static Ogre::Real	RANDOM_POSITION		= 2000.0f;
public:
	Test();

	/* Load additional info */
	void loadAditionalData(void);

	/* function called every frame. Use GlobalObjects::lastTimeFrame */
	void update();


	~Test();

private:
	void loadAnimations(void);
	void changeAnim(int i)
	{
		ASSERT(i < mAnims.size());

		mActualAnim->setEnabled(false);
		mActualAnim = mAnims[i];
		mActualAnim->setTimePosition(0);
		mActualAnim->setEnabled(true);
	}

	void createLotOfZombies(int number);

private:
	Ogre::SceneNode *node;
	Ogre::Entity *ent;
	std::vector<Ogre::AnimationState *> mAnims;
	Ogre::AnimationState *mActualAnim;
	std::vector<Ogre::AnimationState *> mCamina;


};

#endif /* TEST_H_ */
