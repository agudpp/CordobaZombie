/*
 * Test.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef MainPlayerTester_H_
#define MainPlayerTester_H_

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
#include <utils/SelectionHelper.h>
#include <input/InputHelper.h>
#include <global_data/GlobalData.h>
#include <cursor/MouseCursor.h>

#include <main_player/camera_controller/CameraController.h>


namespace tests {

class MainPlayerTester : public core::AppTester
{
public:
    MainPlayerTester();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~MainPlayerTester();

private:

    // @brief Load the player
    //
    void
    loadPlayer(void);

    void
    handleCameraInput(void);

private:
    Ogre::SceneNode *mNode;
    Ogre::Entity *mEntity;
    float mTimeFrame;
    ui::MouseCursor mMouseCursor;
    tool::OrbitCamera mOrbitCamera;
    tool::SelectionHelper mSelHelper;
    core::OgreTextTable mTextTable;
    input::InputHelper mInputHelper;
    Ogre::Bone* mBaseNode;
    Ogre::AnimationState* mAnim;

    cz::CameraController mCamController;
};

}

#endif /* MainPlayerTester_H_ */
