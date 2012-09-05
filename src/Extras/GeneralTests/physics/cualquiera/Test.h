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


#include <btBulletDynamicsCommon.h>
#include <btGhostObject.h>


class CollObj {
public:
	CollObj(const Ogre::String &n) : name(n){};
	~CollObj(){};

	// box
	void createObj(const Ogre::Vector3 &pos);
	void translateObject(const Ogre::Vector3 &dir);
	void setPosition(const Ogre::Vector3 &p);
	void collision(const CollObj *other);
	void restoreTransform(void);



	btCollisionObject *btObj;
	Ogre::Entity	*ent;
	Ogre::SceneNode	*node;
	Ogre::String 	name;
	Ogre::Vector3 lastPos;
	btTransform saveTransform;
};


class Test : public AppTester
{
	const static Ogre::Real	RANDOM_POSITION		= 200.0f;
public:
	Test();

	/* Load additional info */
	void loadAditionalData(void);

	/* function called every frame. Use GlobalObjects::lastTimeFrame */
	void update();


	~Test();

private:
	void createCollisionWorld(void);
	void createBulletBox(const Ogre::Vector3 &pos, const Ogre::Vector3 &size = Ogre::Vector3::UNIT_SCALE);
	void oldTest(void);
private:
	Ogre::SceneNode *node;
	Ogre::Entity *ent;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btCollisionWorld	*mCollWorld;
	CollObj *obj1;
	CollObj *obj2;

	Ogre::SceneNode *ballNode;
	btGhostObject* fallRigidBody;


};

#endif /* TEST_H_ */
