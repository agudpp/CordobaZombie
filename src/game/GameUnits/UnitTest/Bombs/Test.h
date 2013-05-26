/*
 * Test.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef TEST_H_
#define TEST_H_

#include <OgreSceneNode.h>
#include <OgreAnimation.h>
#include <OgreAnimationTrack.h>
#include <OgreAnimationState.h>
#include <OgreBillboardSet.h>
#include <OgreBillboard.h>

#include <vector>

#include "AppTester.h"
#include "DebugUtil.h"
#include "PrimitiveDrawer.h"
#include "MouseCursor.h"

#include "LevelManager.h"

#include "ZombieUnit.h"
#include "PlayerUnit.h"
#include "PlayerUnit/PlayerGroup/PlayerGroup.h"
#include "UpdObjsManager.h"
#include "Shoot.h"

// bombs
#include "Bomb.h"
#include "TimeBomb.h"
#include "ProximityBomb.h"
#include "RemoteBomb.h"
#include "RemoteBombTrigger.h"

//#include "TestUnit.h"

class Test : public AppTester
{
public:
    Test();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~Test();

private:
    /// performe a ray from the position of the mouse
    bool
    performMouseRay(Ogre::Vector3 &v);

    void
    createLevelManager(void);

    // create game objects
    void
    createPlayer(void);
    void
    createZombies(void);

    /**
     * create the bombs
     */
    void
    createBombs(void);

    // handle input
    void
    handleInput(void);

private:
    LevelManager mLevelManager;
    bool mKeyPressed;
    int mPlacesCount;
    f_e::MouseCursor mMouseCursor;
//	CameraController		mCamController;
//	MouseCursor				mMouseCursor;
    std::vector<ZombieUnit*> mZombies;
    std::vector<PlayerUnit*> mPlayers;

    UpdObjsManager mUpdMngr;

    TimeBomb *mTimeBomb;
    ProximityBomb *mProximityBomb;
    RemoteBomb *mRemoteBomb;
    RemoteBombTrigger *mTrigger;

//	TestUnit				mTestUnit;

};

#endif /* TEST_H_ */
