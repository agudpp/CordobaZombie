/*
 * SoundTest.h
 *
 *  Created on: September 24, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban
 */

#ifndef SOUND_TEST_H_
#define SOUND_TEST_H_

#include <OgreSceneNode.h>
#include <OgreAnimation.h>
#include <OgreAnimationTrack.h>
#include <OgreAnimationState.h>
#include <OgreColourValue.h>

#include <vector>
#include <string>

#include <app_tester/AppTester.h>
#include <debug/DebugUtil.h>
#include <debug/OgreText.h>
#include <debug/OgreTextTable.h>
#include <types/basics.h>
#include <utils/OrbitCamera.h>
#include <input/InputHelper.h>


// The xml file for this tool
// <AnimPlayer mesh="file.mesh" />
//
#define ANIM_PLAYER_XML_FILE    "AnimPlayer.xml"

namespace tool {

class SoundTest : public core::AppTester
{
    // Helper class used to handle the different animations of the entity
    //
    class EntityAnimations {

        // enable / disabled animation colors
        //
        static const Ogre::ColourValue ENABLED_ANIM_COL;
        static const Ogre::ColourValue DISABLED_ANIM_COL;
        static const Ogre::ColourValue SELECTED_ANIM_COL;
    public:
        // construct with the table of names we will use to show the current
        // active animations. The table will be positioned in (0,0)
        //
        EntityAnimations(core::OgreTextTable& table);
        ~EntityAnimations();

        // @brief Set the associated entity to handle
        //
        bool
        setEntity(Ogre::Entity* ent);

        // @brief Move to the next animation
        //
        void
        moveNextAnim(void);

        // @brief Move to the previous animation
        //
        void
        movePrevAnim(void);

        // @brief Start/stop animating the current (selected) animation
        //
        void
        startCurrentAnim(bool start);
        bool
        isStartedCurrentAnim(void) const;

        // @brief Set the current animation to run in loop mode
        //
        void
        loopCurrentAnim(bool loop);
        bool
        isLoopCurrentAnim(void) const;

        // @brief Update all the enabled animations
        //
        void
        updateAnims(float timeFrame);

    private:
        EntityAnimations(const EntityAnimations&);
        EntityAnimations& operator=(const EntityAnimations&);

        // @brief Set the correct text for a particular animation
        //
        void
        setText(unsigned int index);

    private:
        core::OgreTextTable& mTable;
        Ogre::Entity* mEntity;
        unsigned int mCurrentAnim;
        std::vector<Ogre::AnimationState *> mAnims;
    };
public:
    SoundTest();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
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

    // @brief Load the mesh file and load the EntityAnimations
    // @param meshName  The mesh name to load (entity).
    // @return true on success | false otherwise
    //
    bool
    loadEntity(const std::string& meshName);

    void
    handleCameraInput(void);

private:
    Ogre::SceneNode *mNode;
    Ogre::Entity *mEntity;
    float mTimeFrame;
    OrbitCamera mOrbitCamera;
    core::OgreTextTable mTextTable;
    EntityAnimations mEntityAnims;
    input::InputHelper mInputHelper;
};

}

#endif /* SOUND_TEST_H_ */
