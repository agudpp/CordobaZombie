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


#include "CircularFormation.h"




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
//
//	// Create the player group
//	PlayerGroup::setLevelManager(&mLevelManager);
//	mPlayerGroup = new PlayerGroup;


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
//		mPlayerGroup->addUnit(zu);
	}

//	CircularFormation *formation = new CircularFormation;
//	mPlayerGroup->setCreator(mPlayers[0]);
//	mPlayerGroup->setFormation(formation);
//	mPlayerGroup->build();


	static ShootContainer sc;
	Shoot::setCollisionManager(mLevelManager.getCollisionManager());
	Shoot::setContainer(&sc);
	Shoot::setUpdObjsManager(&mUpdMngr);
	for(int i = 0; i < 10; ++i){
		Shoot *s = new Shoot;
		s->build(Ogre::Math::RangeRandom(8,16));
	}
	Weapon::setShootContainer(&sc);
	Gun9mm *w = new Gun9mm;
	w->setOwner(mPlayers.back());
	w->setPower(1);
	w->setSqrRange(90000.0);
	w->setEntity(GLOBAL_SCN_MNGR->createEntity("arma.mesh"));
	Weapon::Ammunition ammo;
	ammo.ammo = 25;
	w->setAmmunition(ammo);
	mPlayers.back()->addNewWeapon(w);
}

void Test::createZombies(void)
{
	// set the collision system
	Ogre::Real	MAX_VEL = 50.0f;
	ZombieSMTTable *tt = ZombieFSMBuilder::build();
	ZombieUnit::setSMTransitionTable(tt);

	// create the batery for the zombies
	static BillboardBatery bbb;
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
		zu->setLife(19);
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

/**
 * create the bombs
 */
void Test::createBombs(void)
{
	Bomb::setCollisionManager(mLevelManager.getCollisionManager());
	Bomb::setUpdObjsManager(&mUpdMngr);

	Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_SPHERE);
	Ogre::Entity *ent2 = ent->clone("bomb2");
	Ogre::Entity *ent3 = ent->clone("bomb3");


	mProximityBomb = new ProximityBomb;
	mProximityBomb->setActivationTime(5);
	mProximityBomb->setRadius(20.0f);
	mProximityBomb->setPower(400);
	mProximityBomb->setEntity(ent);

	mTimeBomb = new TimeBomb;
	mTimeBomb->setPower(500);
	mTimeBomb->setBlowTime(10);
	mTimeBomb->setRadius(20);
	mTimeBomb->setEntity(ent2);

	mRemoteBomb = new RemoteBomb;
	mRemoteBomb->setPower(500);
	mRemoteBomb->setRadius(20);
	mRemoteBomb->setEntity(ent3);
	mTrigger = new RemoteBombTrigger(mRemoteBomb);
	ASSERT(!mTrigger->bombWasTriggered());
}



Test::Test()
{
	mMouseCursor.setImage("cursor.png");
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
	static bool mousePressed2 = false;
	// Mouse input check
	if(GLOBAL_MOUSE->getMouseState().buttonDown(OIS::MB_Left)){
		if (!mousePressed)
		{
			mousePressed = true;

			// check if we are getting a player
			static CollisionResult cr;
			static PlayerUnit *pu = 0;

			// first check if we have a player selected
			if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_LSHIFT)){
				if(pu){
					pu->objectUnselected();
					pu = 0;
				}

				return;
			}

			// else...

			Ogre::Vector3 v;
			mLevelManager.getRaycastManger()->getPoint(mMouseCursor.getXRelativePos(),
					mMouseCursor.getYRelativePos(), v);

//			mProximityBomb->setPosition(v);
//			mProximityBomb->startUpdate();
//			mTimeBomb->setPosition(v);
//			mTimeBomb->startUpdate();
			mRemoteBomb->setPosition(v);

		}
	} else {
		mousePressed = false;
	}
	if(GLOBAL_MOUSE->getMouseState().buttonDown(OIS::MB_Right)){
		if (!mousePressed2)
		{
			mousePressed2 = true;
			if(!mTrigger->bombWasTriggered())	mTrigger->triggerBomb();
		}
	} else {
		mousePressed2 = false;
	}

}


/* Load additional info */
void Test::loadAditionalData(void)
{
	createLevelManager();
	createPlayer();
	createZombies();
	createBombs();

	Ogre::MaterialPtr ptr = Ogre::MaterialManager::getSingleton().load("pruebaPijuela", "Popular");
//	ASSERT(ptr.isNull());
	Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_SPHERE);
	ent->setMaterial(ptr);
	Ogre::SceneNode *n = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	n->attachObject(ent);
	n->setPosition(100,100,100);
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
	// update the game objects
	sm::Vector2 trans;
	ZombieUnit *zu;
	for(int i = mZombies.size()-1; i>= 0; --i){
		mZombies[i]->update();
	}
	for(int i = mPlayers.size()-1; i>= 0; --i){
		mPlayers[i]->update();
	}
	t2 = gettimestamp();

	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_P)) {
		if (!keyPres) {
			keyPres = true;
			mPlayers.back()->setActualTartet(mZombies.back());
			mPlayers.back()->newExternalEvent(PlayerUnit::E_ENGAGE_UNIT);
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

	mUpdMngr.updateAllObjects();

}
