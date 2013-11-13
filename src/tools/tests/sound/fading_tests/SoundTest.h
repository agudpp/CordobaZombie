/*
 * SoundTest.h
 *
 *  Created on: September 24, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban
 */

#ifndef SOUND_TEST_H_
#define SOUND_TEST_H_

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
#include <sound/SoundHandler.h>


namespace tests {

class SoundTest : public core::AppTester
{
public:
    SoundTest();
    ~SoundTest();

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
     ** Initalize various sounds playback.
     **
     ** @return
     ** true on success
     ** false otherwise
     **/
    bool
    initSoundsPlayback(void);

    /**
     ** @brief
     ** Interpret keyboard & mouse input to handle camera position & orientation
     **/
    void
    handleCameraInput(void);

    /**
     ** @brief
     ** Interpret keyboard input to handle the sound system.
     **/
    void
    handleSoundInput(void);

    /**
     ** @brief
     ** Locate and print out available audio devices
     **/
    void
    printDevices(void);

    /**
     ** @brief
     ** Load a plane 2D floor into Ogre world.
     **
     ** @return
     ** true on success
     ** false otherwise
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

    // SoundSystem specific members
    mm::SoundHandler&  mSH;
};

}

#endif /* SOUND_TEST_H_ */
