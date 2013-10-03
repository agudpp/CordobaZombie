/*
 * Test.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef EffectsPlayer_H_
#define EffectsPlayer_H_

#include <OgreSceneNode.h>
#include <OgreColourValue.h>
#include <OgreSceneNode.h>
#include <OgreParticleSystem.h>

#include <vector>
#include <string>

#include <app_tester/AppTester.h>
#include <debug/DebugUtil.h>
#include <debug/OgreText.h>
#include <debug/OgreTextTable.h>
#include <types/basics.h>
#include <types/StackVector.h>
#include <utils/OrbitCamera.h>
#include <input/InputHelper.h>
#include <global_data/GlobalData.h>


// Define the Name of the XML FILE
//
#define EFfECTS_PLAYER_XML "EffectsPlayer.xml"


namespace tool {

class EffectsPlayer : public core::AppTester
{
public:
    EffectsPlayer();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~EffectsPlayer();

private:

    // Helper class
    //
    struct ParticleInfo {
        std::string name;
        Ogre::Vector3 position;
    };

    struct ParticleSystemData {
        Ogre::ParticleSystem* ps;
        Ogre::SceneNode* node;
    };

    // @brief Parse the XML with all the ParticleSystems we want to reproduce
    //
    bool
    parseXML(const std::string& xml, std::vector<ParticleInfo>& particleNames) const;

    void
    handleCameraInput(void);

private:
    Ogre::SceneNode *mNode;
    Ogre::Entity *mEntity;
    float mTimeFrame;
    tool::OrbitCamera mOrbitCamera;
    core::OgreTextTable mTextTable;
    input::InputHelper mInputHelper;
    core::StackVector<ParticleSystemData, 10> mParticles;
};

}

#endif /* EffectsPlayer_H_ */
