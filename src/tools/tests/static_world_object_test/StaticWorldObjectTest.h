/*
 * Test.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef StaticWorldObjectTest_H_
#define StaticWorldObjectTest_H_

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

#include <ResourceHandler.h>
#include <static_world_objects/StaticWorldObject.h>
#include <physics/helpers/BulletExporter.h>
#include <collisions/helpers/CollObjectExporter.h>
#include <physic_game_object/PhysicGameObject.h>
#include <scene_asset_loader/SceneAssetLoader.h>
#include <scene_asset_loader/StaticAssetBuilder.h>
#include <types/DataHolder.h>

#include <firing_system/BulletQueue.h>
#include <firing_system/Bullet.h>
#include <firing_system/FiringSystemHandler.h>

#include <effect_handler/EffectHandler.h>
#include <fx/effects/blood/BloodParticles.h>
#include <fx/effects/EffectQueueDefs.h>
#include <fx/effects/bullet_impact/BulletImpactQueueHandler.h>


namespace tests {

class StaticWorldObjectTest : public core::AppTester
{
public:
    StaticWorldObjectTest();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~StaticWorldObjectTest();

private:

    // @brief Load the floor
    //
    void
    loadFloor(void);

    // @brief Configure bullet dynamic world and bullet scene
    //
    void
    configureBullet(void);

    // @brief Perform raycast
    //
    physics::BulletObject*
    performRaycast(void);

    void
    handleCameraInput(void);

    void
    createBulletSystem(void);

    // @brief Load the static world object
    //
    void
    loadStaticWorldObject(void);



private:
    cz::StaticDataLoader mData;

    float& mTimeFrame;
    tool::OrbitCamera mOrbitCamera;
    input::InputHelper mInputHelper;
    ui::MouseCursor mMouseCursor;
    physics::DynamicWorld mDynamicWorld;
    coll::CollisionHandler& mCollHandler;

    rrh::ResourceHandler mResourceHandler;

    cz::StaticWorldObject mStaticWorldObject;
    cz::StaticAssetBuilder mStaticBuilder;
    core::DataHolder<physics::BulletShape*> mShapeHolder;
    core::DataHolder<cz::StaticWorldObject*> mWorldObjectsHolder;

    effect::EffectHandler mEffectHandler;
    cz::FiringSystemHandler mFiringSystem;
    cz::BulletQueue mBulletsQueue;
    cz::BulletImpactQueueHandler mBulletImpactQueueHandler;
};

}

#endif /* StaticWorldObjectTest_H_ */
