/*
 * HUDTest.h
 *
 *  Created on: Nov 9, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban
 */

#ifndef HUD_TEST_H_
#define HUD_TEST_H_

#include <vector>
#include <string>

#include <OgreSceneNode.h>
#include <OgreAnimation.h>
#include <OgreAnimationTrack.h>
#include <OgreAnimationState.h>
#include <OgreColourValue.h>

#include <app_tester/AppTester.h>
#include <debug/DebugUtil.h>
#include <debug/OgreText.h>
#include <debug/OgreTextTable.h>
#include <types/basics.h>
#include <utils/OrbitCamera.h>
#include <input/InputHelper.h>
#include <cursor/MouseCursor.h>
#include <hud/HUD.h>


namespace tests {

class HUDTest : public core::AppTester
{
public:
    HUDTest();
    ~HUDTest();

    /**
     ** @brief
     ** Load additional stuff before launching update cycle.
     **/
    void
    loadAditionalData(void);

    /**
     ** @brief
     ** Function called every frame. Use GlobalObjects::lastTimeFrame
     **/
    void
    update();

private:
    /**
     ** @brief
     ** Read the xml file to check which is the .mesh we want to load.
     **
     ** @param
     ** xmlFName: xml filename
     ** meshName: mesh filename we want to load (entity)
     **
     ** @return
     ** true on success
     ** false otherwise
     **/
    bool
    parseXML(const std::string& xmlFName, std::string& meshName) const;

    void
    handleCameraInput(void);

    /**
     ** @brief
     ** Interpret keyboard input to handle the HUD.
     **/
    void
    handlePlayerInput(void);

    /**
     ** @brief
     ** Load a plane 2D floor into Ogre world.
     **
     ** @return
     **/
    bool
    loadFloor(void);

private:
    // Generic members
    Ogre::SceneNode *mNode;
    Ogre::Entity *mEntity;
    float mTimeFrame;
    tool::OrbitCamera mOrbitCamera;
    core::OgreTextTable mTextTable;
    input::InputHelper mInputHelper;
    ui::MouseCursor mMouseCursor;

    // HUD specific members
    cz::HUD  mHUD;
};

}

#endif /* SOUND_TEST_H_ */
