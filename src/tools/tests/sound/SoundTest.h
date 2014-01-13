/*
 * SoundTest.h
 *
 *  Created on: September 24, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban
 */

#ifndef SOUND_TEST_H_
#define SOUND_TEST_H_


#include <string>

#include <debug/OgreTextTable.h>
#include <app_tester/AppTester.h>
#include <utils/OrbitCamera.h>
#include <input/InputHelper.h>
#include <cursor/MouseCursor.h>
#include <sound/SoundHandler.h>
#include <openal_handler/OpenALHandler.h>


// Forward declarations
namespace Ogre {
    class SceneNode;
    class Entity;
}
namespace rrh {
    class ResourceHandler;
}

namespace tests {

class SoundTest : public core::AppTester
{
    static const unsigned int SOUNDTEST_NUM_RC_FILES = 2;
    static const std::string SOUNDTEST_RC_FILES[SOUNDTEST_NUM_RC_FILES];

public:
    SoundTest();
    ~SoundTest();

    /**
     * @brief Load additional stuff before launching update cycle.
     */
    void
    loadAditionalData(void);

    /**
     * @brief Function called every frame. Use GlobalObjects::lastTimeFrame
     */
    void
    update();

private:
    /**
     * @brief Initalize resources system.
     * @return true on success | false otherwise
     * @remarks Requires file "EngineConfig.xml" properly set in CWD
     */
    bool
    findResources(void);

    /**
     * @brief Loads the needed sounds into the sound system.
     * @remarks findResources() should have been called beforehand.
     */
    bool
    loadSoundFiles(void);

    /**
     * @brief Initalize various sounds playback.
     * @return true on success | false otherwise
     */
    bool
    initSoundsPlayback(void);

    /**
     * @brief Interpret keyboard & mouse input to handle camera
     */
    void
    handleCameraInput(void);

    /**
     * @brief Interpret keyboard input to handle the sound system.
     */
    void
    handleSoundInput(void);

    /**
     * @brief Locate and print out available audio devices
     */
    void
    printDevices(void);

    /**
     * @brief Load a 2D floor plane into the rendered Ogre world.
     * @return true on success | false otherwise
     */
    bool
    loadFloor(void);

private:
    // General purpose test members
    float mTimeFrame;
    Ogre::SceneNode*        mNode;
    Ogre::Entity*           mEntity;
    tool::OrbitCamera       mOrbitCamera;
    core::OgreTextTable     mTextTable;
    input::InputHelper      mInputHelper;
    ui::MouseCursor         mMouseCursor;
    rrh::ResourceHandler*   mRH;

    // SoundSystem specific members
    mm::OpenALHandler       mOpenALHandler;
    mm::SoundHandler&       mSH;
};

}

#endif /* SOUND_TEST_H_ */
