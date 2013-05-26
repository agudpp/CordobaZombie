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
#include "CivilFSMBuilder.h"


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
	mPlayerGroup = new PlayerGroup;


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

		math::Vector2 p;
		p.x = 507 + 5*i;
		p.y = 788 + 30*i;
		zu->setPosition(p);

		mPlayers.push_back(zu);
		mPlayerGroup->addUnit(zu);
	}

	CircularFormation *formation = new CircularFormation;
	mPlayerGroup->setCreator(mPlayers[0]);
	mPlayerGroup->setFormation(formation);
	mPlayerGroup->build();


	static ShootContainer sc;
	Shoot::setCollisionManager(mLevelManager.getCollisionManager());
	Shoot::setUpdObjsManager(&mUpdMngr);
	for(int i = 0; i < 10; ++i){
		Shoot *s = new Shoot;
		s->build(Ogre::Math::RangeRandom(8,16));
	}
	Gun9mm *w = new Gun9mm;
	w->setOwner(mPlayers.back());
	w->setPower(1);
	w->setSqrRange(90000.0);
	w->setEntity(GLOBAL_SCN_MNGR->createEntity("9mm.mesh"));
	Weapon::Ammunition ammo;
	ammo.ammo = 25;
	w->setAmmunition(ammo);
	mPlayers.back()->addNewWeapon(w);
}

void Test::createCivils(void)
{
	// set the collision system
	Ogre::Real	MAX_VEL = 50.0f;
	CivilSMTTable *tt = CivilFSMBuilder::build();
	CivilUnit::setSMTTable(tt);

	math::Vector2 p;
	for(int i = 0; i < 1; ++i){
		Ogre::Entity * ent = GLOBAL_SCN_MNGR->createEntity("civil01.mesh");
		Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();

		CivilUnit *cu = new CivilUnit;
		cu->setEntity(ent);
		cu->setSceneNode(node);
		cu->setHeight(5);
		cu->build();
		cu->setLife(190);
		node->showBoundingBox(true);

		cu->setVelocity(MAX_VEL);

		p.x = 507;
		p.y = 1030;
		cu->setPosition(p);
		mCivils.push_back(cu);
	}
}
void Test::createZombies(void)
{
	// set the collision system
	Ogre::Real	MAX_VEL = 50.0f;
	ZombieSMTTable *tt = ZombieFSMBuilder::build();
	ZombieUnit::setSMTransitionTable(tt);

	// create the batery for the zombies
	static billboard::BillboardBatery bbb;
	bbb.createSet(20, "Billboard/ZombieBlood", 10);
	ZombieUnit::setBillboardBBlood(&bbb);

	static ZombieQueue q;
	ZombieUnit::setQueue(&q);

	math::Vector2 p;
	for(int i = 0; i < 15; ++i){
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

void Test::createBombs(void)
{
	Bomb::setCollisionManager(mLevelManager.getCollisionManager());
	Bomb::setUpdObjsManager(&mUpdMngr);

	Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity("granada.mesh");
	mBomb = new ProximityBomb;
	mBomb->setEntity(ent);
	mBomb->hide();
	mBomb->setPower(99);
	mBomb->setRadius(50);
	static_cast<ProximityBomb *>(mBomb)->setActivationTime(5.0f);
	mPlayers.back()->addBomb(mBomb);

}

// create billboardset
void Test::createBillboardSet(void)
{
	mBillboardSet = GLOBAL_SCN_MNGR->createBillboardSet("testbb", 3);
	mBillboardSet->createBillboard(Ogre::Vector3::ZERO + Ogre::Vector3(0,0,0),Ogre::ColourValue::Green);
	mBillboardSet->createBillboard(Ogre::Vector3::ZERO + Ogre::Vector3(0,0,0),Ogre::ColourValue::Green);
	mBillboardSet->createBillboard(Ogre::Vector3::ZERO + Ogre::Vector3(0,0,0),Ogre::ColourValue::Green);
//	mBillboardSet->createBillboard(Ogre::Vector3::ZERO + Ogre::Vector3(0,0,0),Ogre::ColourValue::Green);
//	mBillboardSet->createBillboard(Ogre::Vector3::ZERO + Ogre::Vector3(0,0,0),Ogre::ColourValue::Green);
	mBillboardSet->setDefaultDimensions(50,50);
	mBillboardSet->setBillboardType(Ogre::BBT_PERPENDICULAR_COMMON);
	mBillboardSet->setCommonDirection(Ogre::Vector3::UNIT_Y);
	mBillboardSet->setCommonUpVector(Ogre::Vector3::UNIT_Z);
	mBillboardSet->setMaterialName("Billboard/GreenCircle");
	mBillboardSet->setVisible(true);
	mBillboardSet->setBounds(Ogre::AxisAlignedBox(
			Ogre::Vector3(0,0,0), Ogre::Vector3(25000,25000,25000)),25000*0.5f);

	debug("Number of billboards: %d\n", mBillboardSet->getNumBillboards());
//	mZombies[0]->getSceneNode()->attachObject(mBillboardSet);

	Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	node->attachObject(mBillboardSet);
//	mBillboardSet->setCullIndividually(true);

}

// Raul 5/6/2012:
void Test::createCollectable(void)
{
	Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity("9mm.mesh");
	////////////
	mMenuManager.build(GLOBAL_WINDOW->getWidth(),GLOBAL_WINDOW->getHeight(),
			5,5);
	IMenu::setMenuManager(&mMenuManager);

	//ShootContainer *sc = new ShootContainer;
	//LongWeapon::setShootContainer(sc);
	Gun9mm *weapon = new Gun9mm;
	weapon->setEntity(GLOBAL_SCN_MNGR->createEntity("9mm.mesh"));
	Weapon::Ammunition ammo;
	ammo.ammo = 123;
	weapon->setAmmunition(ammo);

	WeaponBackpackItem *item1 = new WeaponBackpackItem;
	item1->setWeapon(weapon);
	item1->setSection(BackpackDef::SECTION_WEAPON);
	item1->loadFromOverlay("BpItem/9mm");
	////////////
	mCollectable = new CollectableObject();
	mCollectable->setEntity(ent);
	mCollectable->setObject((void *)item1);
	mCollectable->setCollectableType(COT_BACKPACK);
	mCollectable->setPosition(Ogre::Vector3(1024,0,1024));
	mCollectable->getNode()->showBoundingBox(true);
}

Test::Test()
{
	mMouseCursor.setImage("cursor.png");
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(),
			GLOBAL_WINDOW->getHeight());
}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}

// handle input
void Test::handleInput(void)
{
	math::Vector2 tran;
	static const float VEL = 150.0f;
	tran.x = tran.y = 0.0f;
	static bool keyPres1 = false;
	static bool keyPres = false;

	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	static bool mousePressed = false;

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)){
		if(!keyPres1){
			keyPres1 = true;
			if(mCivils.back()->getFollowingUnit()){
				mCivils.back()->stopFollowUnit();
			} else {
				mCivils.back()->followUnit(mPlayers.back());
			}
		}
	} else {
		keyPres1 = false;
	}

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


			mLevelManager.getCollisionManager()->getCollisionObjects(
					math::Point(v.x, v.z), COL_FLAG_UNIT_PLAYER ,cr);
			if(!cr.empty()){
				// get the player
				 pu = static_cast<PlayerUnit *>(cr.front()->userDefined);

				pu->objectSelected();
			} else {
				if(pu){
//					pu->moveUnitTo(math::Vector2(v.x, v.z));

					pu->plantBomb(mBomb, math::Vector2(v.x,v.z));
				}
			}

//			Ogre::Billboard *b = mBillboardSet->getBillboard(0);
//			b->setPosition(v);

//			Ogre::SceneNode *n = mBillboardSet->getParentSceneNode();
//			v.y += 1.0f;
//			n->setPosition(v);
//			mBillboardSet->_updateBounds();



//			mPlayerGroup->moveGroupTo(math::Vector2(v.x, v.z));
//			mPlayers[0]->moveUnitTo(math::Vector2(v.x, v.z));
		}
	} else {
		mousePressed = false;
	}

}

/* Load additional info */
void Test::loadAditionalData(void)
{

	createLevelManager();
	createPlayer();
	createBombs();
	createCivils();
	createZombies();
	createBillboardSet();
	createCollectable();

	Ogre::DotSceneLoader dsl;
//	dsl.parseDotScene("metros.scene", "Popular", GLOBAL_SCN_MNGR);
	dsl.parseDotScene("generic.scene", "Popular", GLOBAL_SCN_MNGR);


}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	handleInput();

	static double t1,t2,t3,t4;
	static bool keyPres = false;
	math::Vector2 p;

	t1 = gettimestamp();
	t3 = t2 - t1;
	// update the game objects
	math::Vector2 trans;
	ZombieUnit *zu;
	for(int i = mZombies.size()-1; i>= 0; --i){
		mZombies[i]->update();
	}
	for(int i = mPlayers.size()-1; i>= 0; --i){
		mPlayers[i]->update();
	}
	for(int i = mCivils.size()-1; i>= 0; --i){
		mCivils[i]->update();
	}
	t2 = gettimestamp();

	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_P)) {
		if (!keyPres) {
			keyPres = true;

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
