/*
 * Test.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef ZombieBodyTest_H_
#define ZombieBodyTest_H_

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
#include <zombie_unit/ZombieBody.h>
#include <zombie_unit/RagDollQueue.h>
#include <zombie_unit/BodyPartQueue.h>



namespace tests {

class ZombieBodyTest : public core::AppTester
{
public:
    ZombieBodyTest();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~ZombieBodyTest();

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
    configureBody(void);

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

    cz::ZombieBody mBody;
    cz::RagDollQueue<> mRagdollQueue;
    cz::BodyPartQueue mBodyPartQueue;

};

}

#endif /* ZombieBodyTest_H_ */
