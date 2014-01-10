/*
 * Test.h
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#ifndef WorldObjectRepTest_H_
#define WorldObjectRepTest_H_

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
#include <physics/Shape.h>
#include <physics/RigidBody.h>
#include <physics/MotionState.h>
#include <physics/BulletDebugDrawer.h>
#include <collisions/CollisionHandler.h>
#include <collisions/CollDebugDrawer.h>

#include <cursor/MouseCursor.h>

#include <WorldObject.h>
#include <types/DataHolder.h>


namespace tests {

class WorldObjectRepTest : public core::AppTester
{
public:
    WorldObjectRepTest();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();

    ~WorldObjectRepTest();

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

    // @brief Create a rigid body (box). We will NOT add the box to the world
    //        but we will add it to the mRigidBodies vector.
    //
    physics::RigidBody*
    createRigidBodyBox(const Ogre::AxisAlignedBox& box);

    // @brief Build a world object from an entity
    //
    void
    buildWorldObject(w_o::WorldObject& wo, Ogre::Entity* ent);

private:
    float mTimeFrame;
    tool::OrbitCamera mOrbitCamera;
    input::InputHelper mInputHelper;
    ui::MouseCursor mMouseCursor;
    coll::CollisionHandler mCollHandler;
    coll::CollDebugDrawer mCollDbgDrawer;

    physics::DynamicWorld mDynamicWorld;
    physics::CollisionObject mFloorCollObj;
    std::vector<physics::RigidBody*> mRigidBodies;
    physics::BulletDebugDrawer mBulletDbgDrawer;
    w_o::WorldObject mWorldObject;
    core::DataHolder<w_o::GraphicRepresentation*> mGraphicRepHolder;
    core::DataHolder<w_o::Coll2DRepresentation*> mColl2DRepHolder;
    core::DataHolder<w_o::PhysicsRepresentation*> mPhysicsRepHolder;
    core::DataHolder<btCollisionShape*> mShapeHolder;
    core::DataHolder<btCollisionObject*> mCollisionObjectsHolder;
    core::DataHolder<physics::MotionState*> mMotionStatesHolder;
    std::vector<physics::CollisionObject*> mCollisionObjects;

};

}

#endif /* WorldObjectRepTest_H_ */
