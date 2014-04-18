/*
 * WorldObjectRepTest.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include "WorldObjectRepTest.h"

#include <OgreMath.h>
#include <OgreAnimationState.h>
#include <OgreResourceManager.h>
#include <OgreFontManager.h>

#include <memory>
#include <cstring>

#include <tinyxml/tinyxml.h>

#include <xml/XMLHelper.h>
#include <ogre_utils/DotSceneLoader.h>
#include <physics/BulletUtils.h>
#include <physics/RaycastInfo.h>
#include <physics/MotionState.h>
#include <physics/PhysicsBuilder.h>

#include <collisions/CollPreciseInfoBuilder.h>



// helper stuff
//
namespace {

// Construct the mouse input keys we will use
//
static std::vector<input::MouseButtonID>
getMouseButtons(void)
{
    std::vector<input::MouseButtonID> buttons;
    buttons.reserve(10); // just in case :p

    buttons.push_back(input::MouseButtonID::MB_Left);
    buttons.push_back(input::MouseButtonID::MB_Right);
    buttons.push_back(input::MouseButtonID::MB_Middle);

    return buttons;
}


// Construct the keyboard keys we will use
//
static std::vector<input::KeyCode>
getKeyboardKeys(void)
{
    std::vector<input::KeyCode> buttons;
    buttons.reserve(18); // just in case :p

    buttons.push_back(input::KeyCode::KC_ESCAPE);
    buttons.push_back(input::KeyCode::KC_A);
    buttons.push_back(input::KeyCode::KC_S);
    buttons.push_back(input::KeyCode::KC_D);
    buttons.push_back(input::KeyCode::KC_W);
    buttons.push_back(input::KeyCode::KC_LEFT);
    buttons.push_back(input::KeyCode::KC_DOWN);
    buttons.push_back(input::KeyCode::KC_RIGHT);
    buttons.push_back(input::KeyCode::KC_UP);
    buttons.push_back(input::KeyCode::KC_LSHIFT);
    buttons.push_back(input::KeyCode::KC_RSHIFT);
    buttons.push_back(input::KeyCode::KC_0);
    buttons.push_back(input::KeyCode::KC_1);
    buttons.push_back(input::KeyCode::KC_2);
    buttons.push_back(input::KeyCode::KC_E);
    buttons.push_back(input::KeyCode::KC_L);
    buttons.push_back(input::KeyCode::KC_ADD);
    buttons.push_back(input::KeyCode::KC_MINUS);
    buttons.push_back(input::KeyCode::KC_SPACE);

    return buttons;
}


// We will define a own motion state here to test
//

class MotionStateTest : public physics::MotionState
{
public:
    MotionStateTest() {transform.setIdentity();};
    virtual
    ~MotionStateTest() {};

     virtual void
     getWorldTransform(btTransform &worldTrans) const
     {
         worldTrans = transform;
     }

     virtual void
     setWorldTransform(const btTransform &worldTrans)
     {
         transform = worldTrans;

         if (!primitive) {
             return;
         }
         const btQuaternion& rot = worldTrans.getRotation();
         primitive->node->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
         const btVector3& pos = worldTrans.getOrigin();
         primitive->node->setPosition(pos.x(), pos.y(), pos.z());
     }

     btTransform transform;
     core::Primitive* primitive;
};


// @brief Create a box
//


}


namespace tests {


////////////////////////////////////////////////////////////////////////////////
void
WorldObjectRepTest::loadFloor(void)
{
    Ogre::Plane p;
    p.normal = Ogre::Vector3(0, 0, 1);
    p.d = 0;
    Ogre::MeshManager::getSingleton().createPlane("FloorPlane",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, p, 200000,
        200000, 20, 20, true, 1, 9000, 9000, Ogre::Vector3::UNIT_Y);
    // Create an entity (the floor)
    Ogre::Entity *ent = mSceneMgr->createEntity("floor", "FloorPlane");
    ent->setMaterialName("GrassSample");
    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
}

////////////////////////////////////////////////////////////////////////////////
void
WorldObjectRepTest::configureBullet(void)
{
    physics::PhysicsBuilder::createPlaneShape(mFloorCollObj.shape(),
                                              Ogre::Vector3::UNIT_Z,
                                              0);
    physics::PhysicsBuilder::createCollObjectFromShape(mFloorCollObj,
                                                       mFloorCollObj.shape());
    mDynamicWorld.addCollisionObject(mFloorCollObj);

    // enable the debug information
    mBulletDbgDrawer.setEnable(true);

    // create a dummy box and test if bullet works
    Ogre::AxisAlignedBox bb(-5,-5,-5,5,5,5);
    MotionStateTest* ms = new MotionStateTest;
    ms->primitive = core::PrimitiveDrawer::instance().createBox(bb.getCenter(),
                                                                bb.getSize());

    physics::RigidBody* rb = new physics::RigidBody;
    physics::PhysicsBuilder::createBoxShape(rb->shape(), bb);
    physics::PhysicsBuilder::createRigidBodyFromShape(*rb, rb->shape(), 10, ms);

    // move the rigid body
    rb->setPosition(Ogre::Vector3(0,0,400));
    mDynamicWorld.addRigidBody(*rb);
    mRigidBodies.push_back(rb);

}



////////////////////////////////////////////////////////////////////////////////
void
WorldObjectRepTest::handleCameraInput()
{
    ///////////////////////////////////////////////////////////////////////////
    // CAMERA
    //  float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
    Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;
    Ogre::Real zoom = mOrbitCamera.zoom();

    // HERE WE DEFINE THE KEYS USED TO MOVE THE CAMERA, WE WILL USE THE
    // ARROWS TO MOVE THE CAMERA
    // NOTE: we are getting the cameraPosition and modifying the position
    // without using translation, this is because we want to move always
    // in the same axis whatever be the direction of the camera.


    // MOUSE
    const OIS::MouseState& lMouseState = mMouse->getMouseState();

    if(mInputHelper.isKeyPressed(input::KeyCode::KC_A)) {
        mTranslationVec.x -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_D)) {
        mTranslationVec.x += 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_W)) {
        mTranslationVec.z -= 1.0f;
    }
    if(mInputHelper.isKeyPressed(input::KeyCode::KC_S)) {
        mTranslationVec.z += 1.0f;
    }

    if(mTranslationVec != Ogre::Vector3::ZERO) {
        mOrbitCamera.moveCamera(mTranslationVec);
    }
    if(zoom != mOrbitCamera.zoom()) {
        mOrbitCamera.setZoom(zoom);
    }

    const float lMouseZ = input::Mouse::relZ();
    float scrollZoom = mOrbitCamera.zoom();
    if (lMouseZ > 0.0f) {
        scrollZoom += 5.f;
    } else if (lMouseZ < 0.0f) {
        scrollZoom -= 5.f;
    }
    if(scrollZoom != mOrbitCamera.zoom()) {
        mOrbitCamera.setZoom(scrollZoom);
    }

    // check tracking camera
    static int lastX = 0, lastY = 0;
    const float lMouseX = float(input::Mouse::relX());
    const float lMouseY = float(input::Mouse::relY());
    if (mInputHelper.isMousePressed(input::MouseButtonID::MB_Right)) {
        const float factor = -0.005 * 1.5f;
        mOrbitCamera.rotateCamera(Ogre::Radian(lMouseX * factor),
                                    Ogre::Radian(lMouseY * factor));
    }

    // check for the type of camera we want to use
    if (mInputHelper.isKeyPressed(input::KeyCode::KC_1)) {
        mOrbitCamera.setCameraType(tool::OrbitCamera::CameraType::FreeFly);
    } else if (mInputHelper.isKeyPressed(input::KeyCode::KC_2)) {
        mOrbitCamera.setCameraType(tool::OrbitCamera::CameraType::Orbit);
    }

}

////////////////////////////////////////////////////////////////////////////////
physics::RigidBody*
WorldObjectRepTest::createRigidBodyBox(const Ogre::AxisAlignedBox& bb)
{
    MotionStateTest* ms = new MotionStateTest;
    ms->primitive = core::PrimitiveDrawer::instance().createBox(bb.getCenter(),
                                                                bb.getSize());

    physics::RigidBody* rb = new physics::RigidBody;
    physics::PhysicsBuilder::createBoxShape(rb->shape(), bb);
    physics::PhysicsBuilder::createRigidBodyFromShape(*rb, rb->shape(), 10, ms);

    // move the rigid body
    rb->setPosition(Ogre::Vector3(0,0,200));
    mDynamicWorld.addRigidBody(*rb);
    mRigidBodies.push_back(rb);
}

////////////////////////////////////////////////////////////////////////////////
void
WorldObjectRepTest::buildWorldObject(w_o::WorldObject& wo,
                                     Ogre::Entity* ent)
{
    ASSERT(ent);

    // we will create the representations for that entity

    // Graphic rep
    w_o::GraphicRepresentation* grep = new w_o::GraphicRepresentation;
    mGraphicRepHolder.add(grep);
    grep->setEntity(ent);
    Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(ent);
    grep->setSceneNode(node);
    wo.setGraphicRepresentation(grep);

    // Coll2D rep
    w_o::Coll2DRepresentation* crep = new w_o::Coll2DRepresentation;
    mColl2DRepHolder.add(crep);
    const float radius = ent->getBoundingRadius() / 4.f;
    core::AABB bb(radius, -radius, -radius, radius);
    crep->setCollisionInformation(0, bb);
    crep->setCollision2DType(w_o::Coll2DRepresentation::Coll2DType::Coll2D_Dynamic);
    wo.setColl2DRepresentation(crep);

    // Physics rep
    w_o::PhysicsRepresentation* prep = new w_o::PhysicsRepresentation;
    mPhysicsRepHolder.add(prep);
    physics::CollisionObject* rb = new physics::CollisionObject; // LEAK HERE!
    mCollisionObjects.push_back(rb);
    physics::PhysicsBuilder::createShapeFromMesh(rb->shape(), ent->getMesh().get());
    mShapeHolder.add(rb->shape().bulletShape());
    physics::PhysicsBuilder::createCollObjectFromShape(*rb, rb->shape());
    mCollisionObjectsHolder.add(rb->bulletCollObj());
    prep->setCollisionObjectElement(rb);
    wo.setPhysicsRepresentation(prep);


}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
WorldObjectRepTest::WorldObjectRepTest() :
    core::AppTester(mTimeFrame)
,   mOrbitCamera(mCamera, mSceneMgr, mTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
,   mBulletDbgDrawer(mSceneMgr, &(mDynamicWorld.bulletDynamicWorld()))
,   mCollDbgDrawer(mSceneMgr)
{
    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);
    setUseDefaultInput(false);

    // set the collision handler to the coll2D rep and the physics world to the
    // physics rep
    w_o::Coll2DRepresentation::setCollisionHandler(&mCollHandler);
    w_o::PhysicsRepresentation::setDynamicWorld(&mDynamicWorld);

    // configure collision handler
    static const float worldDelta = 9999.f;
    core::AABB worldBB(0,0,0,0);
    worldBB.tl.x -= worldDelta;
    worldBB.tl.y += worldDelta;
    worldBB.br.y -= worldDelta;
    worldBB.br.x += worldDelta;
    mCollHandler.setWorldBoundingBox(worldBB);
    mCollHandler.setDebugDrawer(&mCollDbgDrawer);
    mCollDbgDrawer.setEnable(true);

    // configure the mouse cursor
    mMouseCursor.setCursor(ui::MouseCursor::Cursor::NORMAL_CURSOR);
    mMouseCursor.setVisible(true);
    mMouseCursor.setWindowDimensions(mWindow->getWidth(), mWindow->getHeight());
    mMouseCursor.updatePosition(mWindow->getWidth() / 2,
                                mWindow->getHeight() / 2);

}

WorldObjectRepTest::~WorldObjectRepTest()
{
    // TODO Auto-generated destructor stub
}

/* Load additional info */
void
WorldObjectRepTest::loadAditionalData(void)
{
    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

    // try to load the xml file
    loadFloor();

    configureBullet();

    // create a zombie in the screen
    Ogre::Entity* zombie = mSceneMgr->createEntity("zombie.mesh");
    buildWorldObject(mWorldObject, zombie);
    mWorldObject.setEnabled(true);
    mWorldObject.setPosition(Ogre::Vector3(0,0,25));
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
WorldObjectRepTest::update()
{
    // update the input system
    mInputHelper.update();

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
        return;
    }

    if (mInputHelper.isMouseReleased(input::MouseButtonID::MB_Left)) {
        Ogre::Vector3 force = Ogre::Vector3(100,0,0);//mCamera->getDirection();
        force.normalise();
        force *= 400.f;
        for (physics::RigidBody* rb : mRigidBodies) {
            rb->activate(true);
            rb->applyCentralImpulse(force);
        }
    }

    if (mInputHelper.isKeyReleased(input::KeyCode::KC_UP)) {
        mWorldObject.setPosition(mOrbitCamera.getCameraPosition());
    }
    if (mInputHelper.isKeyReleased(input::KeyCode::KC_DOWN)) {
        mWorldObject.setOrientation(mCamera->getDerivedOrientation());
    }

    // update the camera
    handleCameraInput();

    // update dynamic world
    mDynamicWorld.simulate(mTimeFrame);

    // update coll2D world
    mCollHandler.update();
}

}
