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
#include <sound/SoundHandler.h>


namespace tests {

class SoundTest : public core::AppTester
{
public:
    SoundTest();

    // Load additional info
    void
    loadAditionalData(void);

    // Function called every frame. Use GlobalObjects::lastTimeFrame
    void
    update();

    ~SoundTest();

private:

    // @brief Read the xml file to check which is the .mesh we want to load.
    // @param xmlFName  The xml filename
    // @param meshName  The mesh filename we want to load (entity)
    // @return true on success | false otherwise
    //
    bool
    parseXML(const std::string& xmlFName, std::string& meshName) const;

//    // @brief Load the mesh file and load the EntityAnimations
//    // @param meshName  The mesh name to load (entity).
//    // @return true on success | false otherwise
//    //
//    bool
//    loadEntity(const std::string& meshName);

    void
    handleCameraInput(void);

    // @brief Locate and print out available audio devices
    void
    printDevices(void);

private:
    // Generic members
    Ogre::SceneNode *mNode;
    Ogre::Entity *mEntity;
    float mTimeFrame;
    tool::OrbitCamera mOrbitCamera;
    core::OgreTextTable mTextTable;
    input::InputHelper mInputHelper;
    // SoundSystem specific members
    mm::SoundHandler&  mSH;
};

}

#endif /* SOUND_TEST_H_ */
