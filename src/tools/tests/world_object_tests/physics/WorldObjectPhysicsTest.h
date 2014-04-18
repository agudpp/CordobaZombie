/*
 * Test.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef WorldObjectPhysicsTest_H_
#define WorldObjectPhysicsTest_H_

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

#include <physics/DynamicWorld.h>
#include <physics/CollisionObject.h>
#include <physics/PhysicsBuilder.h>
#include <physics/BulletDebugDrawer.h>
#include <cursor/MouseCursor.h>



namespace tests {

class WorldObjectPhysicsTest : public core::AppTester
{
public:
    WorldObjectPhysicsTest();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~WorldObjectPhysicsTest();

private:

    // @brief Load the floor
    //
    void
    loadFloor(void);

    // @brief Configure bullet dynamic world and bullet scene
    //
    void
    configureBullet(void);

    void
    handleCameraInput(void);

private:
    float mTimeFrame;
    tool::OrbitCamera mOrbitCamera;
    input::InputHelper mInputHelper;
    ui::MouseCursor mMouseCursor;
    physics::DynamicWorld mDynamicWorld;
    physics::CollisionObject mFloorCollObj;
    std::vector<physics::RigidBody*> mRigidBodies;
    physics::BulletDebugDrawer mBulletDbgDrawer;
};

}

#endif /* WorldObjectPhysicsTest_H_ */
