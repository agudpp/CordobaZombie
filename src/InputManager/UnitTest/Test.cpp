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

#include "GlobalObjects.h"
#include "Test.h"
#include <DotSceneLoader.h>
#include <Util.h>

#include <GameUnits/Weapon/ShortWeapon/9mm/Gun9mm.h>
#include <GameUnits/PlayerUnit/StateMachine/PlayerFSMBuilder.h>
#include <GameUnits/PlayerUnit/PlayerUnitBuilder.h>
#include <GameUnits/PlayerUnit/PlayerDefs.h>
#include <InputManager/IInputState.h>
#include <SelectionSystem/SelectionManager.h>


// Add corners to a box
void Test::showCorners(const Ogre::AxisAlignedBox &b)
{
	const Ogre::Vector3 *corners = b.getAllCorners();

	for(int i = 0; i <= 7; ++i){
		Ogre::Entity *box = GLOBAL_SCN_MNGR->createEntity(Ogre::SceneManager::PT_CUBE);
		Ogre::SceneNode *node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
		node->attachObject(box);
		node->setPosition(corners[i]);
	}

}

void
Test::createPlayers(void)
{
    GameObject::collMng = mLevelManager.getCollisionManager();
    PlayerSMTTable *tt = PlayerFSMBuilder::build();
    PlayerUnit::setSMTransitionTable(tt);
    Shoot::setCollisionManager(mLevelManager.getCollisionManager());
    Shoot::setUpdObjsManager(&mUpdaterManager);
    for(int i = 0; i < 10; ++i){
        Shoot *s = new Shoot;
        s->build(Ogre::Math::RangeRandom(8,16));
    }

    // build the player:
    PlayerUnitBuilder &playerBuilder = PlayerUnitBuilder::instance();
    mPlayers.push_back(playerBuilder.buildPlayer(PlayerID::PLAYER_CORAL));


}

Test::Test() :
        AppTester(true)
{
	setUseDefaultInput(false);
	input::IInputState::setLevelManager(&mLevelManager);
    mInputManager = &input::InputManager::getInstance();
    mInputManager->setCameraController(&mCamController);
}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}


/* Load additional info */
void Test::loadAditionalData(void)
{
	GLOBAL_CAMERA->getParentSceneNode()->detachAllObjects();
	mCamController.setCamera(GLOBAL_CAMERA);

	Ogre::AxisAlignedBox mb;
	mb.setMaximum(13800,13800,13800);
	mb.setMinimum(-3800,0,-3800);
	mCamController.setCameraMoveZone(mb);
	mCamController.setCameraVelocity(10.0f);

	UpdatableObject::setUpdObjsManager(&mUpdaterManager);

	// build the level
    Ogre::Entity *ent = GLOBAL_SCN_MNGR->createEntity("pathfinder.mesh");
    mLevelManager.load(ent, 25000, 25000, 100, 100);
    mLevelManager.showTriangles();
    mInputManager->setLevelManager(&mLevelManager);

	// load the level
	Ogre::DotSceneLoader dsl;
	dsl.parseDotScene("cuidad01.scene", "Popular", GLOBAL_SCN_MNGR);
	createPlayers();
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
    static bool k1 = false;

    selection::SelectionManager &selManager = selection::SelectionManager::getInstance();

    if(mKeyboard->isKeyDown(OIS::KC_1)) {
        if (k1 == false) {
            k1 = true;
            // unselect all
            selManager.unselectAll();
        }
    } else {
        k1 = true;
    }

    if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
    }
    if(mKeyboard->isKeyDown(OIS::KC_SPACE)) {
        // we have to exit
        mCamController.setCamPos(Ogre::Vector3(0.f, 0.1f, 0.f));
    }
	mInputManager->update();

	mUpdaterManager.updateAllObjects();
//	mInputManager.update();

	mUpdaterManager.updateAllObjects();
	for (size_t i = 0, size = mPlayers.size(); i < size; ++i) {
	    mPlayers[i]->update();
	}

	// update selection changes
	selection::SelectionManager::getInstance().executeCallbacks();
}
