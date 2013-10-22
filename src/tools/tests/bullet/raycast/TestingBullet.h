/*
 * Test.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef TestingBullet_H_
#define TestingBullet_H_

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
#include <types/StackVector.h>
#include <debug/PrimitiveDrawer.h>

#include <bullet/btBulletDynamicsCommon.h>
#include <physics/RagDoll.h>
#include <physics/DynamicWorld.h>
#include <physics/BulletObject.h>
#include <cursor/MouseCursor.h>


// The xml file for this tool
// <TestingBullet mesh="file.mesh" />
//
#define ANIM_PLAYER_XML_FILE    "TestingBullet.xml"

namespace tests {

class TestingBullet : public core::AppTester
{
public:
    TestingBullet();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~TestingBullet();

private:

    // @brief Load the floor
    //
    void
    loadFloor(void);

    // @brief Configure bullet dynamic world and bullet scene
    //
    void
    configureBullet(void);

    // @brief create sample fisics
    //
    void
    createSampleScene(void);

    // @brief Perform raycast
    //
    physics::BulletObject*
    performRaycast(void);

    void
    handleCameraInput(void);

private:
    float mTimeFrame;
    tool::OrbitCamera mOrbitCamera;
    input::InputHelper mInputHelper;
    ui::MouseCursor mMouseCursor;
    physics::DynamicWorld mDynamicWorld;
    core::StackVector<physics::BulletObject*, 450> mObjects;

};

}

#endif /* TestingBullet_H_ */
