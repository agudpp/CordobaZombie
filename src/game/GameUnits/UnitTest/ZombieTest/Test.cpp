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
#include "ZombieIdleState.h"
#include "SMTransitionTable.h"
#include "Test.h"
#include "GameUnitDefines.h"
#include "ZombieFSMBuilder.h"
#include "DotSceneLoader.h"




static double gettimestamp(void)
{
	struct timeval now;

	if(gettimeofday(&now, 0) < 0)
		return 0.0;

	double nowDouble = static_cast<double>(now.tv_sec + (
						static_cast<double>(now.tv_usec)/1000000.0));

	return nowDouble;
}


void Test::createLevelManager(void)
{
	Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity("pathfinder.mesh");
	mLevelManager.load(ent, 25000, 25000, 100, 100);
	mLevelManager.showTriangles();
}

void Test::loadLevelMeshes(void)
{
	Ogre::DotSceneLoader dsl;
	dsl.parseDotScene("demo.scene", "General",
			GLOBAL_SCN_MNGR, GLOBAL_SCN_MNGR->getRootSceneNode());
//	Ogre::DotSceneLoader dsl2;
//	dsl2.parseDotScene("Escena Oger.scene", "General",
//			GLOBAL_SCN_MNGR, GLOBAL_SCN_MNGR->getRootSceneNode());
}

void Test::createRealZombies(void)
{
	GameObject::collMng = mLevelManager.getCollisionManager();
	Ogre::Real	MAX_VEL = 18.0f;
	ZombieSMTTable *tt = ZombieFSMBuilder::build();
	ZombieUnit::setSMTransitionTable(tt);

	// create the batery for the zombies
	static effects::BillboardBatery bbb;
	bbb.createSet(20, "Billboard/ZombieBlood", 10);
	ZombieUnit::setBillboardBBlood(&bbb);

	static ZombieQueue q;
	ZombieUnit::setQueue(&q);

	core::Vector2 p;
	int counter = 1;
	for(int i = 0; i < 400; ++i){
		Ogre::Entity * ent = GLOBAL_SCN_MNGR->createEntity("zombie01.mesh");
		Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
		std::stringstream ss;
		ss << "zombie";
		counter = counter +1;
		counter %= 10;
		counter++;
		if(counter == 10){
			ss << "10";
		} else {
			ss << "0" << counter;
		}
		ent->setMaterialName(ss.str());

		ZombieUnit *zu = new ZombieUnit;
		zu->setEntity(ent);
		zu->setSceneNode(node);
		zu->setHeight(5);
		zu->setAttackPower(1);
		zu->build();
		zu->setLife(9);
//		node->showBoundingBox(true);


		zu->setWalkVelocity(MAX_VEL);
		zu->setAttackVelocity(MAX_VEL*2.0f);
		zu->setVelocity(MAX_VEL);

//		p.x = 940;
//		p.y = 1269;
		p.x = 507 + Ogre::Math::RangeRandom(-30,30);
		p.y = 788 + Ogre::Math::RangeRandom(-30,30);
		zu->setPosition(p);
		mZombies.push_back(zu);
	}
	for(int i = 0; i < 400; ++i){
		Ogre::Entity * ent = GLOBAL_SCN_MNGR->createEntity("zombie01.mesh");
		Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
		std::stringstream ss;
		ss << "zombie";
		counter = counter +1;
		counter %= 10;
		counter++;
		if(counter == 10){
			ss << "10";
		} else {
			ss << "0" << counter;
		}
		ent->setMaterialName(ss.str());

		ZombieUnit *zu = new ZombieUnit;
		zu->setEntity(ent);
		zu->setSceneNode(node);
		zu->setHeight(5);
		zu->setAttackPower(1);
		zu->build();
		zu->setLife(9);
//		node->showBoundingBox(true);

		zu->setWalkVelocity(MAX_VEL);
		zu->setAttackVelocity(MAX_VEL*2.0f);
		zu->setVelocity(MAX_VEL);

//		p.x = 377;
//		p.y = 945;
		p.x = 1107 + Ogre::Math::RangeRandom(-30,30);
		p.y = 1030 + Ogre::Math::RangeRandom(-30,30);
		zu->setPosition(p);
		mZombies.push_back(zu);
	}


	Ogre::Entity *coral = GLOBAL_SCN_MNGR->createEntity("coral.mesh");
	Ogre::SceneNode *coralNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	coralNode->attachObject(coral);
	coralNode->setPosition(1117, 5, 1040);


}

void Test::createZombieTarget(void)
{
	Ogre::Entity * ent = GLOBAL_SCN_MNGR->createEntity("zombie01.mesh");
	Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
	node->showBoundingBox(true);

	ZombieUnit *go = new ZombieUnit;
	go->setEntity(ent);
	go->setSceneNode(node);
	go->setHeight(5);
	go->build();
	go->setLife(9999999);
//	go->configCollObj(w,h,c_p::COL_FLAG_UNIT_PLAYER, c_p::COL_FLAG_UNIT_PLAYER);
//	go->setHeight(5);
	go->getCollisionObject().maskFlag = c_p::COL_FLAG_UNIT_PLAYER;
	go->setPosition(core::Vector2(517,3347));

	go->setWalkVelocity(70.0f);
	go->setAttackVelocity(70.0f*2.0f);
	go->setVelocity(70.0f);

	mPlayer = go;
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
	core::Vector2 tran;
	static const float VEL = 150.0f;
	tran.x = tran.y = 0.0f;
	static bool keyPres1 = false;
	static bool keyPres2 = false;
	static bool keyPres = false;

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_J)){
		tran.x += 1;
	}
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_L)){
		tran.x -= 1;
	}
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_I)){
		tran.y += 1;
	}
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_K)){
		tran.y -= 1;
	}
	tran.normalize();
	tran *= VEL * GLOBAL_TIME_FRAME;
	mPlayer->translate(tran);

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_1)){
		if(!keyPres){
			keyPres = true;
			Hit_t h;
			h.shooter = mPlayer;
			h.power = 1.0f;
			h.hitDir = core::Vector2(.0f, 1.0);

			mZombies[0]->beenHit(h);
		}
	} else
		keyPres = false;

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_2)){
		if(!keyPres1){
			keyPres1 = true;
			Hit_t h;
			h.shooter = mPlayer;
			h.power = 1.0f;
			h.hitDir = core::Vector2(1.0f, 0.0);

			mZombies[0]->beenHit(h);
		}
	} else
		keyPres1 = false;

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)){
		if(!keyPres2){
			keyPres2 = true;
			debug("Adding effect\n");
		}
	} else
		keyPres2 = false;

}


/* Load additional info */
void Test::loadAditionalData(void)
{


	createLevelManager();
	loadLevelMeshes();
//	createGameObjects();
//	createGameObjects2();
	createRealZombies();
	createZombieTarget();

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	handleInput();

	static double t1,t2,t3,t4;
	static bool keyPres = false;
	core::Vector2 p;

	t1 = gettimestamp();
	t3 = t2 - t1;
	// update the game objects
	core::Vector2 trans;
	ZombieUnit *zu;
	int size = mZombies.size()-1;
	for(int i = 0; i <= size; ++i){
		mZombies[i]->update();
		mZombies[i]->haveLineOfSightTo(mZombies[i%size], ZOMBIE_MAX_VISIBILITY_SQRDIST);
	}
	mPlayer->update();

	t2 = gettimestamp();

	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_P)) {
		if (!keyPres) {
			keyPres = true;
			std::cout << "Printing Times:\n" << "LastFrameTime: "
					<< GLOBAL_TIME_FRAME << "\n" << "Time of sim: " << t2 - t1
					<< "\n" << "Time of rendering and more: " << t3
					<< std::endl;

			static PathfindingManager::Path p;
			static DrawablePath dp;
			core::Point point(507,788);
			mLevelManager.getPathfinderManager()->getRandomPath(point,p,0,10);
			dp.clearPath();
			dp.drawPath(p);


			std::cout << "Camera Pos: " << mBodySceneNode->getPosition().x << " "
					<< mBodySceneNode->getPosition().z << std::endl;

		}
	} else {
		keyPres = false;
	}


}
