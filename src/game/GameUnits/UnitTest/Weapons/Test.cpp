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
#include "SMTransitionTable.h"
#include "Test.h"
#include "GameUnitDefines.h"
#include "PlayerFSMBuilder.h"
#include "ZombieFSMBuilder.h"
#include "ShortWeapon.h"
#include "Gun9mm.h"
#include "BillboardBatery.h"
#include "ProximityBomb.h"
#include "CollectableObject.h"
#include "CollectableObjTypes.h"

#include "DotSceneLoader.h"


#include "CircularFormation.h"


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
#include "BackpackItem.h"
#include "WeaponBackpackItem.h"
#include "Weapon.h"
#include "Gun9mm.h"

#include "BackpackDefines.h"


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

// create game objects
void Test::createPlayer(void)
{
	GameObject::collMng = mLevelManager.getCollisionManager();
	Ogre::Real	MAX_VEL = 180.0f;
	PlayerSMTTable *tt = PlayerFSMBuilder::build();
	PlayerUnit::setSMTransitionTable(tt);

	// Create the player group
	PlayerGroup::setLevelManager(&mLevelManager);


	for(int i = 0; i < 1; i++){
		Ogre::Entity * ent = GLOBAL_SCN_MNGR->createEntity("coral.mesh");
		Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();

		PlayerUnit *zu = new PlayerUnit;
		zu->setEntity(ent);
		zu->setSceneNode(node);
		zu->setHeight(5);
		zu->build();
		zu->setLife(9);
		node->showBoundingBox(true);
		zu->setMaxVelocity(MAX_VEL*2.0f);
		zu->setVelocity(MAX_VEL);

		sm::Vector2 p;
		p.x = 507 + 5*i;
		p.y = 788 + 30*i;
		zu->setPosition(p);

		mPlayers.push_back(zu);
	}


	// Build the weapon;
	mWeaponBuilder.configureShoots(100, mLevelManager.getCollisionManager(),&mUpdMngr);
	mWeaponBuilder.setFilename("Weapons.xml");
	Weapon *w = mWeaponBuilder.createWeapon("shotgun");
	w->setSqrRange(290*290);
	ASSERT(w);
	w->setOwner(mPlayers.back());

	mPlayers.back()->addNewWeapon(w);
}

void Test::createZombies(void)
{
	// set the collision system
	Ogre::Real	MAX_VEL = 50.0f;
	ZombieSMTTable *tt = ZombieFSMBuilder::build();
	ZombieUnit::setSMTransitionTable(tt);

	// create the batery for the zombies
	staticbillboard:: BillboardBatery bbb;
	bbb.createSet(20, "Billboard/ZombieBlood", 10);
	ZombieUnit::setBillboardBBlood(&bbb);

	static ZombieQueue q;
	ZombieUnit::setQueue(&q);

	sm::Vector2 p;
	for(int i = 0; i < 1; ++i){
		Ogre::Entity * ent = GLOBAL_SCN_MNGR->createEntity("zombie01.mesh");
		Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();

		ZombieUnit *zu = new ZombieUnit;
		zu->setEntity(ent);
		zu->setSceneNode(node);
		zu->setHeight(5);
		zu->setAttackPower(1);
		zu->build();
		zu->setLife(1900);
		node->showBoundingBox(true);

		zu->setWalkVelocity(MAX_VEL);
		zu->setAttackVelocity(MAX_VEL*2.0f);
		zu->setVelocity(MAX_VEL);

		p.x = 1107;
		p.y = 1030;
		zu->setPosition(p);
		mZombies.push_back(zu);
	}
}

void Test::createLine(void)
{
	// create the manual object and the scene node
	mLineNode = GLOBAL_SCN_MNGR->createSceneNode();
	mLine = GLOBAL_SCN_MNGR->createManualObject();

	// TODO: tenemos que usar un material aca predeterminado
	mLine->begin("", Ogre::RenderOperation::OT_LINE_STRIP);

	const float size = 10.0f;
	Ogre::Vector3 p1(Ogre::Vector3::ZERO),p2(Ogre::Vector3::ZERO);
	p1.z = size * 0.5f;
	p2.z = -size * 0.5f;

	mLine->position(p1);  // start position
	mLine->position(p2);  // end position

	mLine->end();

	mLineNode->attachObject(mLine);

	GLOBAL_SCN_MNGR->getRootSceneNode()->addChild(mLineNode);

}

void Test::createShoots(void)
{
	Shoot::setCollisionManager(mLevelManager.getCollisionManager());
	Shoot::setUpdObjsManager(&mUpdMngr);

	for(int i = 0; i < 100; ++i){
		Shoot *s = new Shoot;
		// we build it of a random size
		Ogre::Real rnd = Ogre::Math::RangeRandom(5,10);
		s->build(rnd);
	}
}

Test::Test()
{
	mMouseCursor.setImage("cursor.png");
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(),
			GLOBAL_WINDOW->getHeight());

	mBPos = Ogre::Vector3::ZERO;
	mEPos = Ogre::Vector3::ZERO;

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

	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	static bool mousePressed = false;
	// Mouse input check
	if(GLOBAL_MOUSE->getMouseState().buttonDown(OIS::MB_Left)){
		if (!mousePressed)
		{
			mousePressed = true;
			Ogre::Vector3 pos;
			if(!performMouseRay(pos)){
				return;
			}
			pos.y += 4;
			// else we perform raycast
			if(mBPos == Ogre::Vector3::ZERO){
				mBPos = pos;
				DRAWER.destroyAllVPoints();
				DRAWER.createNewVPoint(pos);
				return;
			} else if (mEPos == Ogre::Vector3::ZERO){
				mEPos = pos;
				DRAWER.createNewVPoint(pos);
			}

		}
	} else {
		mousePressed = false;
	}

	// clear with right mouse
	if(GLOBAL_MOUSE->getMouseState().buttonDown(OIS::MB_Right)){
		DRAWER.destroyAllVPoints();
		mBPos = mEPos = Ogre::Vector3::ZERO;
	}

}


/* Load additional info */
void Test::loadAditionalData(void)
{

	createLevelManager();
	createPlayer();
	createZombies();
	createLine();
	createShoots();

	Ogre::DotSceneLoader dsl;
//	dsl.parseDotScene("metros.scene", "Popular", GLOBAL_SCN_MNGR);
	dsl.parseDotScene("generic.scene", "Popular", GLOBAL_SCN_MNGR);

	// create a collision object (primera manzana)
	static CollisionObject co;
	co.bb.setSize(1014 - 548, 896-433);
	co.bb.setPosition(sm::Vector2((1014+566)/2.0f,
			(893+433)/2.0f));
	co.maskFlag = COL_FLAG_UNIT_PLAYER;
	co.userDefined = 0;
	DRAWER.createBox(co.bb);
//	mLevelManager.getCollisionManager()->addStaticObject(&co);

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	handleInput();

	static float counter = 0;
	static double t1,t2,t3,t4;
	static bool keyPres = false;
	static bool keyPres1 = false;
	static bool keyPres2 = false;
	sm::Vector2 p;

	t1 = gettimestamp();
	t3 = t2 - t1;
	// update the game objects
	sm::Vector2 trans;
	ZombieUnit *zu;
	for(int i = mZombies.size()-1; i>= 0; --i){
		mZombies[i]->update();
	}
	for(int i = mPlayers.size()-1; i>= 0; --i){
		mPlayers[i]->update();
	}

	// shoot every middle second
	counter += GLOBAL_TIME_FRAME;
	if(counter > 0.5f) {
		counter = 0;
		if(mEPos != Ogre::Vector3::ZERO && mBPos != Ogre::Vector3::ZERO){
			Shoot *s = mShootContainer.getAvailableObject();
			if(s){
//				debugRED("NEW SHOOT: SizeContainer: %d\n", mShootContainer.getQueue().size());
				s->setStartPos(mBPos);
				Ogre::Vector3 trans = mEPos - mBPos;
				trans.normalise();
				trans *= 2;
				s->setTranslateVec(trans);
				s->startUpdate();
			}
		}
	}

//	const sm::Vector2 &zp = mZombies[0]->getPosition();
//	b->setPosition(Ogre::Vector3(zp.x, 1, zp.y));

	t2 = gettimestamp();

	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_P)) {
		if (!keyPres) {
			keyPres = true;
			mPlayers.back()->engageUnit(mZombies.back());
//			return;

			std::cout << "Printing Times:\n" << "LastFrameTime: "
					<< GLOBAL_TIME_FRAME << "\n" << "Time of sim: " << t2 - t1
					<< "\n" << "Time of rendering and more: " << t3
					<< std::endl;

			std::cout << mBodySceneNode->getPosition().x << "\t" <<
					mBodySceneNode->getPosition().z << std::endl;
		}
	} else {
		keyPres = false;
	}
	static Ogre::Vector3 dir = Ogre::Vector3(0.003,0,0.0010);
	mBackPos = mLineNode->getPosition();
	mLineNode->setPosition(0,0,0);
	mLineNode->getParentSceneNode()->removeChild(mLineNode);
	mLineNode->setPosition(mBackPos);
	GLOBAL_SCN_MNGR->getRootSceneNode()->addChild(mLineNode);

	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_G)) {
		if (!keyPres1) {
			keyPres1 = true;
			dir.x -= 0.1 * GLOBAL_TIME_FRAME;
			mLineNode->lookAt(mLineNode->getPosition() + dir * 50.0f,
								Ogre::Node::TS_WORLD);
//			mLineNode->setDirection(dir);
		}
	} else {
		keyPres1 = false;
	}
	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_H)) {
		if (!keyPres2) {
			keyPres2 = true;
			dir.z -= 0.1 * GLOBAL_TIME_FRAME;
			mLineNode->lookAt(mLineNode->getPosition() + dir * 50.0f,
					Ogre::Node::TS_WORLD);
//			mLineNode->setDirection(dir);
		}
	} else {
		keyPres2 = false;
	}
	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_Y)) {
			Ogre::Vector3 t = dir;
			const float l = t.length();
			if(l){
				t /= t.length();
				mLineNode->translate(t * 200.0f * GLOBAL_TIME_FRAME);
			}

	}
	mUpdMngr.updateAllObjects();

}
