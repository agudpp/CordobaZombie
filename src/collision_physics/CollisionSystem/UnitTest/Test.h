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
#include <OgreVector3.h>
#include <OgreAnimationState.h>

#include "AppTester.h"
#include "LevelManager.h"
#include "CollisionManager.h"
#include "DebugUtil.h"
#include "GlobalObjects.h"
#include "CommonMath.h"
#include "MouseCursor.h"



class SphereColl {
public:
	SphereColl(const CollisionObject *c) :
		co(c)
	{
		ASSERT(c);
		// get the position of the co
		node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
		ent = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_CUBE);
		node->attachObject(ent);
		// set the real size
		float size = c->bb.getHeight()*0.5f;
		float realSize = ent->getBoundingRadius();
		float factor = size / realSize;
		node->setScale(factor, factor, factor);

	}
	~SphereColl(){};

	SphereColl(const SphereColl& other)
	{
		co = other.co;
		ent = other.ent;
		node = other.node;
	}

	void updatePos(math::Vector2 &p, CollisionResult &cr)
	{
		math::Vector2 aux, pos;
		p = transVec;


		pos = co->getPosition();
		node->setPosition(pos.x, 10, pos.y);

		for(int i = cr.size()-1; i >= 0; --i){
			aux = pos -co->getPosition();
//			float len = aux.inverseLen();
//			aux.normalize();
//			aux *= len;
			p += aux;
		}

		// check if we are outside
		float nextX = pos.x + p.x;
		float nextY = pos.y + p.y;
		if(nextX <= 0.0f || nextY <= 0.0f || nextX >= 22800.0f || nextY >= 22800.0f){
			p.x = 0; p.y = 0;
		}


	}

	math::Vector2 transVec;
private:
	const CollisionObject *co;
	Ogre::Entity *ent;
	Ogre::SceneNode *node;


};


class Test : public AppTester
{
public:
	Test();

	/* Load additional info */
	void loadAditionalData(void);

	/* function called every frame. Use GlobalObjects::lastTimeFrame */
	void update();


	~Test();

private:
	bool performMouseRay(Ogre::Vector3 &v);

	void loadLevel(void);
	void simpleTest(void);

	// handle input
	void handleInput(void);

private:
	MouseCursor				mMouseCursor;
	LevelManager			mLevelManager;

	std::vector<CollisionObject *>	mCollObjs;
	CollisionResult			mCollResult;
	std::vector<SphereColl *>			mSphereObjs;

};

#endif /* TEST_H_ */
