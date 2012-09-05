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

#include <iostream>
#include <cstdio>

#include "OgreHardwareBuffer.h"
#include "OgrePixelFormat.h"


//char *video_list[] = {"../Videos/scare.ogg", "../Videos/TalkTalk-SuchAShame.flv",
//"../Videos/TrailerdejuegoCordobaZombie.wmv","../Videos/MartinSolveig_Dragonette-Hello.mp4", 0};
//
//int video = 0;


#define myvideo "/home/raul/Dropbox/201Z-proyecto/Videos/caligaris.flv"


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

		sm::Vector2 p;
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
	w->setEntity(GLOBAL_SCN_MNGR->createEntity("9mm.mesh"));
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


void Test::createCollectable(void)
{
}

Test::Test():
		VPAPI(0)
{
	//mMouseCursor.setImage("cursor.png");
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(),
			GLOBAL_WINDOW->getHeight());

	// testing collision aabb with line
	sm::AABB bb;
	bb.tl.x = 0; bb.tl.y = 100;
	bb.br.x = 150; bb.br.y = 0;

	sm::Point p1,p2;

	p1.x = -1; p1.y = 0;
	p2.x = -10; p2.y = -10;
	ASSERT(!IntersectDetect::checkLineAABB(p1,p2,bb));

	p1.x = -10; p1.y = 10;
	p2.x = 1000; p2.y = 10;
	ASSERT(IntersectDetect::checkLineAABB(p1,p2,bb));

	p1.x = 1000; p1.y = 10;
	p2.x = -1000; p2.y = 10;
	ASSERT(IntersectDetect::checkLineAABB(p1,p2,bb));

	p1.x = 1000; p1.y = 100;
	p2.x = 1000; p2.y = 10;
	ASSERT(!IntersectDetect::checkLineAABB(p1,p2,bb));

	p1.x = 0; p1.y = 100;
	p2.x = 100; p2.y = 10;
	ASSERT(IntersectDetect::checkLineAABB(p1,p2,bb));

	p1.x = 50; p1.y = 1000;
	p2.x = 50; p2.y = 10;
	ASSERT(IntersectDetect::checkLineAABB(p1,p2,bb));

	p1.x = 50; p1.y = 1000;
	p2.x = 50; p2.y = 500;
	ASSERT(!IntersectDetect::checkLineAABB(p1,p2,bb));

	p1.x = 50; p1.y = 10;
	p2.x = 100; p2.y = 250;
	ASSERT(IntersectDetect::checkLineAABB(p1,p2,bb));

	p1.x = -50; p1.y = -10;
	p2.x = 100; p2.y = 250;
	ASSERT(IntersectDetect::checkLineAABB(p1,p2,bb));

	p1.x = 50; p1.y = -10;
	p2.x = 100; p2.y = 250;
	ASSERT(IntersectDetect::checkLineAABB(p1,p2,bb));

	p1.x = 149; p1.y = 103;
	p2.x = 102; p2.y = 98;
	ASSERT(IntersectDetect::checkLineAABB(p1,p2,bb));

	p1.x = -2; p1.y = 0;
	p2.x = 3; p2.y = -2;
	ASSERT(!IntersectDetect::checkLineAABB(p1,p2,bb));


}

Test::~Test()
{

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
					sm::Point(v.x, v.z), COL_FLAG_UNIT_PLAYER ,cr);
			if(!cr.empty()){
				// get the player
				 pu = static_cast<PlayerUnit *>(cr.front()->userDefined);

				pu->objectSelected();
			} else {
				if(pu){
//					pu->moveUnitTo(sm::Vector2(v.x, v.z));

					pu->plantBomb(mBomb, sm::Vector2(v.x,v.z));
				}
			}

//			Ogre::Billboard *b = mBillboardSet->getBillboard(0);
//			b->setPosition(v);

//			Ogre::SceneNode *n = mBillboardSet->getParentSceneNode();
//			v.y += 1.0f;
//			n->setPosition(v);
//			mBillboardSet->_updateBounds();



//			mPlayerGroup->moveGroupTo(sm::Vector2(v.x, v.z));
//			mPlayers[0]->moveUnitTo(sm::Vector2(v.x, v.z));
		}
	} else {
		mousePressed = false;
	}

}

void Test::testCollectObject(void){

	for(int i = mPlayers.size()-1; i>= 0; --i){
		mPlayers[i]->collectObject(mCollectable);
	}
}


void Test::testCollisionRaycast(void)
{
	CollisionObject co;
	float tlx, tly, brx, bry;

	// set the size
	tlx = 10; tly = 100;
	brx = 150; bry = 10;
	co.bb.setSize(brx - tlx, tly-bry);
	co.bb.setPosition(sm::Vector2((tlx+brx)/2.0f,
			(tly+bry)/2.0f));
	co.maskFlag = COL_FLAG_UNIT_PLAYER;
	co.userDefined = 0;
	mLevelManager.getCollisionManager()->addStaticObject(&co);

	// create the lines here and test
	CollisionResult result;
	std::vector<sm::Point> points;
	sm::Point p1,p2;

	p1.x = 9; p1.y = 19;
	p2.x = 20; p2.y = 0;
	mLevelManager.getCollisionManager()->getCollisionObjects(p1,p2,~0,result);
	ASSERT(result.size() == 1);
	result[0]->getIntPoints(p1,p2,points);
	ASSERT(points.size() == 2);


	p1.x = 140; p1.y = 7;
	p2.x = 220; p2.y = 190;
	mLevelManager.getCollisionManager()->getCollisionObjects(p1,p2,~0,result);
	ASSERT(result.size() == 1);
	result[0]->getIntPoints(p1,p2,points);
	ASSERT(points.size() == 2);

	p1.x = 9; p1.y = 70;
	p2.x = 220; p2.y = 70;
	mLevelManager.getCollisionManager()->getCollisionObjects(p1,p2,~0,result);
	ASSERT(result.size() == 1);
	result[0]->getIntPoints(p1,p2,points);
	ASSERT(points.size() == 2);

	p1.x = 100; p1.y = 170;
	p2.x = 130; p2.y = 7;
	mLevelManager.getCollisionManager()->getCollisionObjects(p1,p2,~0,result);
	ASSERT(result.size() == 1);
	result[0]->getIntPoints(p1,p2,points);
	ASSERT(points.size() == 2);

	p1.x = 5; p1.y = 70;
	p2.x = 9; p2.y = 70;
	mLevelManager.getCollisionManager()->getCollisionObjects(p1,p2,~0,result);
	ASSERT(result.size() == 0);


}


/* Load additional info */
void Test::loadAditionalData(void)
{

	createLevelManager();
	//testCollisionRaycast();
	//createPlayer();
	//createBombs();
	//createZombies();
	//createBillboardSet();
	//createCollectable();


	//VIDEOPLAYER
	//mVideoPlayer = new VideoPlayer();
	//mUpdMngr.addObject((UpdatableObject*)mVideoPlayer);
	//ENDVIDEOPLAYER

	Ogre::DotSceneLoader dsl;
//	dsl.parseDotScene("metros.scene", "Popular", GLOBAL_SCN_MNGR);
	dsl.parseDotScene("generic.scene", "Popular", GLOBAL_SCN_MNGR);

//	551.041	409.493
//	542.984	877.037
//	1024.84	925.684
//	1026.31	408.371

//	1017.45	433.93
//	1014.17	893.606
//	566.47	896.569
//	539.702	408.827

	// create a collision object (primera manzana)
	static CollisionObject co;
	co.bb.setSize(1014 - 548, 896-433);
	co.bb.setPosition(sm::Vector2((1014+566)/2.0f,
			(893+433)/2.0f));
	co.maskFlag = COL_FLAG_UNIT_PLAYER;
	co.userDefined = 0;
	DRAWER.createBox(co.bb);
//	mLevelManager.getCollisionManager()->addStaticObject(&co);


	// create the bomb



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
//	Ogre::Billboard *b = mBillboardSet->getBillboard(0);
//	const sm::Vector2 &zp = mZombies[0]->getPosition();
//	b->setPosition(Ogre::Vector3(zp.x, 1, zp.y));

	t2 = gettimestamp();
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_N)){
		if (!keyPres) {
			keyPres = true;
			VPAPI->next();
//			double len = 0;
//			mVideoPlayer->get_video_length(len);
//			len /= 3.0f;
//			mVideoPlayer->seek_time_stamp(len);
		}
	}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_S)){
		if (!keyPres) {
			keyPres = true;
			if(VPAPI){
				VPAPI->set_visible(!VPAPI->is_visible());
			}
		}
	}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_J)){
		if (!keyPres) {
			keyPres = true;
			if(VPAPI){
				VPAPI->play(1);
			}
		}
	}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_P)){
		if (!keyPres) {
			keyPres = true;
			if(VPAPI){
				if(VPAPI->is_playing()){
					VPAPI->pause();
				}else{
					VPAPI->play();
				}
			}
		}
	}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_V)){
		if (!keyPres) {
			keyPres = true;
			if(!VPAPI){
				//VPAPI = new VideoPlayerAPI(&Ogre::Vector4(-0.5f,0.5f,0.5f,-0.5f));
				VPAPI = new VideoPlayerAPI();
				//VPAPI->queue_video("../Videos/TrailerdejuegoCordobaZombie.wmv");
				//VPAPI->queue_video("../Videos/MartinSolveig_Dragonette-Hello.mp4", 20.0f, 25.0f);
				VPAPI->queue_video(myvideo);
				VPAPI->queue_video(myvideo, 5.0f, 10.0f);
				VPAPI->queue_video(myvideo);
				VPAPI->queue_video(myvideo, 5.0f, 10.0f);
				VPAPI->set_repeat(true);
				VPAPI->play();
			}
		}
	}else {
		keyPres = false;
	}

	mUpdMngr.updateAllObjects();
	if(VPAPI){
		VPAPI->update(GLOBAL_TIME_FRAME);
	}

	///////////////////// END VIDEO

}



//OgreApp: /home/raul/Programacion/ZOMBIE/trunk/src/VideoPlayer/FfmpegVideoPlayer/FfmpegVideoPlayer.cpp:1010: int VideoPlayer::audio_decode_frame(uint8_t**, int): Assertion `decoded_data_size == 0' failed.
