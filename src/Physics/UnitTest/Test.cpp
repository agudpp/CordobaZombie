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
#include "PhysicsHelper.h"




static double gettimestamp(void)
{
	struct timeval now;

	if(gettimeofday(&now, 0) < 0)
		return 0.0;

	double nowDouble = static_cast<double>(now.tv_sec + (
						static_cast<double>(now.tv_usec)/1000000.0));

	return nowDouble;
}

bool Test::performMouseRay(Ogre::Vector3 &v)
{
	// set up the ray
	Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mMouseCursor.getXRelativePos(),
			mMouseCursor.getYRelativePos());

	// check if the ray intersects our plane
	// intersects() will return whether it intersects or not (the bool value) and
	// what distance (the Real value) along the ray the intersection is
	std::pair<bool, Ogre::Real> result = mouseRay.intersects(mGlobalPlane);

	if (result.first)
	{
		// if the ray intersect the plane, we have a distance value
		// telling us how far from the ray origin the intersection occurred.
		// the last thing we need is the point of the intersection.
		// Ray provides us getPoint() function which returns a point
		// along the ray, supplying it with a distance value.

		// get the point where the intersection is
		v = mouseRay.getPoint(result.second);
		return true;
	}

	return false;
}

void Test::createLevelManager(void)
{
	Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity("pathfinder.mesh");
	mLevelManager.load(ent, 25000, 25000, 100, 100);
	mLevelManager.showTriangles();
}



Test::Test()
{
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
	sm::Vector2 tran;
	static constexpr float VEL = 150.0f;
	tran.x = tran.y = 0.0f;
	static bool keyPres1 = false;
	static bool keyPres = false;

	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	static bool mousePressed = false;

	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_1)) {
		if (!keyPres) {
			keyPres = true;
			mPhysicObj.force += Ogre::Vector3(1,10,0);
		}
	} else {
		keyPres = false;
	}

}

void Test::createPhysics(void)
{
	Ogre::Entity *bomb = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_CUBE);
	Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();


	node->attachObject(bomb);
	static CollisionObject collObj;
	const Ogre::AxisAlignedBox &ab = bomb->getBoundingBox();
	Ogre::Real r = ab.getHalfSize().length();
	collObj.bb.setSize(r, r);
	node->setPosition(0,3*r,0);
	// configure physic object
	mPhysicObj.collisionObject = &collObj;
	mPhysicObj.frictionCoeff = 0.5f;
	mPhysicObj.node = node;
	mPhysicObj.weight = 1.0f;

}


/* Load additional info */
void Test::loadAditionalData(void)
{
	createLevelManager();
	createPhysics();

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	handleInput();

	PhysicsHelper::updateAllObject(mPhysicObj);

	static double t1,t2,t3,t4;
	static bool keyPres = false;
	static bool keyPres1 = false;
	static bool keyPres2 = false;
	static bool keyPres3 = false;
	sm::Vector2 p;

	t1 = gettimestamp();
	t3 = t2 - t1;

	t2 = gettimestamp();

	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_1)) {
		if (!keyPres) {
			keyPres = true;
			mMouseCursor.setCursor(MouseCursor::NORMAL_CURSOR);
			debugBLUE("MouseCursor::NORMAL_CURSOR\n");
		}
	} else {
		keyPres = false;
	}
	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_2)) {
		if (!keyPres1) {
			keyPres1 = true;
			mMouseCursor.setCursor(MouseCursor::ATTACK_CURSOR);
			debugBLUE("MouseCursor::ATTACK_CURSOR\n");
		}
	} else {
		keyPres1 = false;
	}
	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_3)) {
		if (!keyPres2) {
			keyPres2 = true;
			mMouseCursor.setCursor(MouseCursor::MOVE_CRUSOR);
			debugBLUE("MouseCursor::MOVE_CRUSOR\n");
		}
	} else {
		keyPres2 = false;
	}
	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_4)) {
		if (!keyPres3) {
			keyPres3 = true;
			mMouseCursor.setCursor(MouseCursor::PICK_OBJECT_CURSOR);
			debugBLUE("MouseCursor::PICK_OBJECT_CURSOR\n");
		}
	} else {
		keyPres3 = false;
	}

}
