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


#define myvideo2 "/home/raul/Dropbox/CordobaZombie/menu/Menu_animacion/menu_ar3:2.ogg"
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



// create overlay uv test
void Test::createOverlay(void)
{
	// load the fade
	Ogre::PanelOverlayElement	*mOverlayPanel = 0;
	Ogre::MaterialPtr			mFaderMaterial;
	Ogre::TextureUnitState 		*mTexture = 0;

	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

	mOverlayPanel = static_cast<Ogre::PanelOverlayElement*>(
		overlayManager.createOverlayElement("Panel", "Fader"));
	mOverlayPanel->setMetricsMode(Ogre::GMM_RELATIVE);
	mOverlayPanel->setPosition(0, 0);
	mOverlayPanel->setDimensions(0.1f, 0.1f);
	mOverlayPanel->setMaterialName("BackpackMaterial"); // Optional background material

	// Ensures that the material exists
	mOverlayPanel->setUV(0,0,0.5,1);

	// show the fade
	mOverlayPanel->show();

	// Create an overlay, and add the panel
	Ogre::Overlay			*mOverlay = 0;
	mOverlay = overlayManager.create("TestOverlay1");
	mOverlay->add2D(mOverlayPanel);
	mOverlay->show();
}




Test::Test():
		VPAPI(0)
{
	createOverlay();

	VPAPI = new VideoPlayerAPI();
	VPAPI->setRepeat(true);
	//VPAPI->setVisible(false);

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
					pu->plantBomb(mBomb, sm::Vector2(v.x,v.z));
				}
			}
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
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_1)){
		if (!keyPres) {
			keyPres = true;
			VPAPI->load(myvideo);
		}
	}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_2)){
		if (!keyPres) {
			keyPres = true;
			VPAPI->load(myvideo2);
		}
	}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_3)){
		if (!keyPres) {
			keyPres = true;
			VPAPI->load(myvideo,5.0f,10.0f);
		}
	}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_4)){
		if (!keyPres) {
			keyPres = true;
			ASSERT(VideoPlayerAPI::VIDEO_ERROR == VPAPI->load(myvideo2,15.0f,10.0f));
		}
	}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_G)){
		if (!keyPres) {
			keyPres = true;
			VPAPI->play();
		}
	}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_P)){
		if (!keyPres) {
			keyPres = true;
			VPAPI->pause();
		}
	}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_V)){
		if (!keyPres) {
			keyPres = true;
			VPAPI->setVisible(!VPAPI->isVisible());
		}
	}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_5)){
		if (!keyPres) {
			keyPres = true;
			VPAPI->load(0,4.4f,7.9f);
		}
	}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_6)){
		if (!keyPres) {
			keyPres = true;
			VPAPI->load();
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
