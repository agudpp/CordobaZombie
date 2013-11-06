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

#include <bullet/btBulletDynamicsCommon.h>

#include <app_tester/AppTester.h>
#include <collisions/CollDefines.h>
#include <collisions/CollisionHandler.h>
#include <collisions/CollObject.h>
#include <collisions/CollPreciseInfo.h>
#include <collisions/CollPreciseInfoBuilder.h>
#include <cursor/MouseCursor.h>
#include <debug/DebugUtil.h>
#include <debug/OgreText.h>
#include <debug/OgreTextTable.h>
#include <debug/PrimitiveDrawer.h>
#include <global_data/GlobalData.h>
#include <input/InputHelper.h>
#include <physics/BulletObject.h>
#include <physics/DynamicWorld.h>
#include <physics/RagDoll.h>
#include <static_data_loader/StaticDataLoader.h>
#include <types/basics.h>
#include <types/StackVector.h>
#include <utils/OrbitCamera.h>
#include <zombie_unit/BodyPartQueue.h>
#include <zombie_unit/RagDollQueue.h>
#include <zombie_unit/ZombieBody.h>
#include <zombie_unit/ZombieUnit.h>
#include <zombie_unit/ZombieUnitBuilder.h>

#include <firing_system/BulletQueue.h>
#include <firing_system/Bullet.h>
#include <firing_system/FiringSystemHandler.h>

#include <effect_handler/EffectHandler.h>
#include <fx/effects/blood/BloodParticles.h>
#include <fx/effects/EffectQueueDefs.h>
#include <fx/effects/billboards/BillboardStack.h>
#include <fx/effects/billboards/BillboardGroupHandler.h>




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

    // @brief Test ZombieUnitBuilder
    //
    void
    testBuilder(void);

    // @brief Create bullets and configure the firing system
    //
    void
    createBulletSystem(void);

    void
    handleCameraInput(void);

private:
    cz::StaticDataLoader mData;

    float& mTimeFrame;
    tool::OrbitCamera mOrbitCamera;
    input::InputHelper mInputHelper;
    ui::MouseCursor mMouseCursor;
    physics::DynamicWorld mDynamicWorld;
    core::StackVector<physics::BulletObject*, 450> mObjects;

    coll::CollisionHandler& mCollHandler;

    cz::ZombieBody mBody;
    cz::ZombieUnit mZombieUnit;
    cz::RagDollQueue<> mRagdollQueue;
    cz::BodyPartQueue mBodyPartQueue;
    cz::ZombieUnitBuilder mBuilder;

    effect::EffectHandler mEffectHandler;
    cz::BloodParticlesQueue mBloodQueue;

    cz::FiringSystemHandler mFiringSystem;
    cz::BulletQueue mBulletsQueue;
};

}

#endif /* ZombieBodyTest_H_ */
