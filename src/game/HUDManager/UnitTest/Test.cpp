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

#include "GUIHelper.h"
#include "GlobalObjects.h"
#include "Test.h"
#include "PlayerFSMBuilder.h"

struct TestCallback : public ExtraAction::Callback {
	virtual void operator()(int buttonPressed)
	{
		debugColor(DEBUG_BROWN, "Apretado boton :%d\n", buttonPressed);
	}
};

static double gettimestamp(void)
{
	struct timeval now;

	if(gettimeofday(&now, 0) < 0)
		return 0.0;

	double nowDouble = static_cast<double>(now.tv_sec + (
						static_cast<double>(now.tv_usec)/1000000.0));

	return nowDouble;
}








Test::Test()
{
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight());

	// build menu manager
	mMenuManager.build(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight(),
				5,5);
	IMenu::setMenuManager(&mMenuManager);

	// Load passportPicture
	mPassportPic.load();
	mPassportPic.changePicture("cardenal.jpg");

//	Ogre::Overlay *elem = Ogre::OverlayManager::getSingleton().getByName("BackpackOverlay");
//	ASSERT(elem);
//	elem->show();
}

Test::~Test()
{
	// TODO Auto-generated destructor stub
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
		p.x = 10;
		p.y = 10;
		zu->setPosition(p);

		mPlayers.push_back(zu);
	}
}

void Test::configureMap(void)
{
	// create a plane
	Ogre::MeshManager::getSingleton().createPlane("Mapfloor", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			mGlobalPlane, 400, 400, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

	Ogre::Entity* floor = mSceneMgr->createEntity("Mapfloor", "Mapfloor");
	floor->setMaterialName("TEST_MAP_FLOOR");
	Ogre::SceneNode *aux = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	aux->attachObject(floor);
	aux->translate(200,10,200);

	// create the overlay to show the map
	Ogre::Overlay *overlay = Ogre::OverlayManager::getSingleton().create("mapOverlay");
	ASSERT(overlay);
	overlay->show();
	overlay->add2D(mMap.getContainer());
//	mMap.getContainer()->setMetricsMode(Ogre::GuiMetricsMode::GMM_RELATIVE);
	mMap.getContainer()->setLeft(0);
	mMap.getContainer()->setTop(0);
	mMap.getContainer()->setWidth(0.4);
	mMap.getContainer()->setHeight(0.4);
//	GUIHelper::reposContainer(mMap.getContainer());
	mMap.show();


	mMap.configure();
	CellphoneMapInfo mapInfo;
	mapInfo.materialName = "TEST_MAP";
	mapInfo.height = 400;
	mapInfo.width = 400;
	mMap.setMap(mapInfo);

	// add players
	for(int i = 0; i < mPlayers.size(); ++i){
		mMap.addPlayer(mPlayers[i]);
	}
}

// handle input
void Test::handleInput(void)
{
	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	Ogre::Vector3 m = Ogre::Vector3::ZERO;
	const float factor = 100 * GLOBAL_TIME_FRAME;
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_F)){
		m.z+=factor * 1.0f;
	}
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_H)){
		m.z-=factor * 1.0f;
	}
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_T)){
		m.x+=factor * 1.0f;
	}
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_G)){
		m.x-=factor * 1.0f;
	}

	mPlayers.back()->translate(m);
}


/* Load additional info */
void Test::loadAditionalData(void)
{
	mPanel.load();
	ExtraAction::Callback *cb = new TestCallback;
	ASSERT(mPanel.getNumLockers() == 2);
	ASSERT(mPanel.getNumActualActions() == 0);
	mPanel.addAction(cb, "HUD/ExtraAction/bombs");
	ASSERT(mPanel.getNumActualActions() == 1);
	mPanel.addAction(cb, "HUD/ExtraAction/bombs");
	ASSERT(mPanel.getNumActualActions() == 2);

	createLevelManager();
	createPlayer();
	configureMap();
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	handleInput();
//	mTestUnit.update();
	static bool keyPres = false, keyPres2 = false;

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)){
		if(!keyPres){
			keyPres = true;

			mPanel.removeAction(0);
		}
	} else {
		keyPres = false;
	}


	mMap.update();
	mMenuManager.update();
}
