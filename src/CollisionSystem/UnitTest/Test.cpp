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
#include <OgreMath.h>
#include "PrimitiveDrawer.h"
#include "DotSceneLoader.h"
#include "CollObjBuilder.h"

#include "Test.h"

#include <sys/time.h>



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


void Test::loadLevel(void)
{
	Ogre::DotSceneLoader dsl;
	dsl.parseDotScene("nivelprueba.scene","General", GLOBAL_SCN_MNGR, GLOBAL_SCN_MNGR->getRootSceneNode());
	Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity("Manzana03.mesh");
	Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	node->attachObject(ent);
	ent = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_CUBE);
	node->attachObject(ent);
	node->setPosition(0,10,0);
	node->showBoundingBox(true);
	node = GLOBAL_SCN_MNGR->getSceneNode("floorNode");
	node->setVisible(false);

//	CollObjBuilder::createFromMesh(ent, "aabb");
}

void Test::simpleTest(void)
{
//	mCollManager.removeAllObjects();
//	mCollManager.build(22800.0f, 22800.0f, 100,100);

//	sm::Vector2 p;
//	int numUnits = 1600;
//	float velFactor = 1;
//
//	for(int i = 0; i < numUnits; ++i){
//		p.x = Ogre::Math::RangeRandom(1500.0f, 12000.0f);
//		p.y = Ogre::Math::RangeRandom(1500.0f, 12000.0f);
//		CollisionObject *co = new CollisionObject;
//		co->bb.setSize(50.0f, 50.0f);
//		co->bb.setPosition(p);
//		mCollObjs.push_back(co);
//		mCollManager.addMovableObject(co);
//		p = co->getPosition();
//		SphereColl *s = new SphereColl(co);
//		s->transVec.x = Ogre::Math::RangeRandom(0.0f, velFactor);
//		s->transVec.y = Ogre::Math::RangeRandom(0.0f, velFactor);
//		mSphereObjs.push_back(s);
//
//	}
//
//	for(int i = 0; i < numUnits; ++i){
//			p.x = Ogre::Math::RangeRandom(15000.0f, 22000.0f);
//			p.y = Ogre::Math::RangeRandom(15000.0f, 22000.0f);
//			CollisionObject *co = new CollisionObject;
//			co->bb.setSize(50.0f, 50.0f);
//			co->bb.setPosition(p);
//			mCollObjs.push_back(co);
//			mCollManager.addMovableObject(co);
//			p = co->getPosition();
//			SphereColl *s = new SphereColl(co);
//			s->transVec.x = Ogre::Math::RangeRandom(-velFactor, 0);
//			s->transVec.y = Ogre::Math::RangeRandom(-velFactor, 0);
//			mSphereObjs.push_back(s);
//
//		}


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
	sm::Vector2 tran;
	static const float VEL = 150.0f;
	tran.x = tran.y = 0.0f;
	static bool keyPres1 = false;
	static bool keyPres = false;
	static std::vector<sm::Point>	selPoints;

	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	static bool mousePressed = false;
	// Mouse input check
	if(GLOBAL_MOUSE->getMouseState().buttonDown(OIS::MB_Left)){
		if (!mousePressed)
		{
			mousePressed = true;

			// check if we are getting a player
			static CollisionResult cr;

			Ogre::Vector3 v;
			mLevelManager.getRaycastManger()->getPoint(mMouseCursor.getXRelativePos(),
					mMouseCursor.getYRelativePos(), v);

			selPoints.push_back(sm::Point(v.x,v.z));

			if(selPoints.size() >= 2){
				DRAWER.destroyAllVPoints();
				DRAWER.createNewVPoint(Ogre::Vector3(
						selPoints[0].x, 5, selPoints[0].y));
				DRAWER.createNewVPoint(Ogre::Vector3(
						selPoints[1].x, 5, selPoints[1].y));
				// ray cast
				mLevelManager.getCollisionManager()->getCollisionObjects(
						selPoints[0], selPoints[1], ~0, cr);
				if(!cr.empty()){
					// get the points for every point that collide
					std::vector<sm::Vector2> intpoints;

					for(int i = cr.size() - 1; i >= 0; --i){
						cr[i]->getIntPoints(selPoints[0], selPoints[1], intpoints);
						if(intpoints.empty()){
							debug("No intersection!!!\n");
							break;
						}
						// else...
						for(int j = intpoints.size()-1; j >= 0; --j){
							DRAWER.createNewVPoint(Ogre::Vector3(
									intpoints[j].x, 5, intpoints[j].y));
						}
					}
				} else {
					debug("No object detected..\n");
				}

				selPoints.clear();
			}


		}
	} else {
		mousePressed = false;
	}

}


/* Load additional info */
void Test::loadAditionalData(void)
{
	loadLevel();

	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight());

//	simpleTest();
	Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity("pathfinder.mesh");
	mLevelManager.load(ent, 25000, 25000, 100, 100);
	mLevelManager.showTriangles();

//	mCamera->setPolygonMode(Ogre::PM_WIREFRAME);


	// create a collision object (primera manzana)
//	static CollisionObject co;
//	co.bb.setSize(1014 - 548, 896-433);
//	co.bb.setPosition(sm::Vector2((1014+566)/2.0f,
//			(893+433)/2.0f));
//	co.maskFlag = ~0;
//	co.userDefined = 0;
//	DRAWER.createBox(co.bb);
//	mLevelManager.getCollisionManager()->addStaticObject(&co);

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	handleInput();

	static double t1,t2,t3,t4;
	static bool keyPres = false;
	sm::Vector2 p;

	t1 = gettimestamp();
	t3 = t2 - t1;
	for (int i = mCollObjs.size() - 1; i >= 0; --i) {
		mLevelManager.getCollisionManager()->getCollision(mCollObjs[i], mCollResult);
//		if(!mCollResult.empty()){
//			debug("Collision found [%d]: size: %d\n", i, mCollResult.size());
//		}
		mSphereObjs[i]->updatePos(p, mCollResult);
		mLevelManager.getCollisionManager()->translateObject(mCollObjs[i], p);

	}

	t2 = gettimestamp();

	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_P)) {
		if (!keyPres) {
			keyPres = true;
			std::cout << "Printing Times:\n" << "LastFrameTime: "
					<< GLOBAL_TIME_FRAME << "\n" << "Time of sim: " << t2 - t1
					<< "\n" << "Time of rendering and more: " << t3
					<< std::endl;
		}
	} else {
		keyPres = false;
	}

}
