/*
 * ZombieBodyTest.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include "ZombieBodyTest.h"

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
#include <physics/BulletImporter.h>
#include <physics/BulletLoader.h>
#include <physics/RaycastInfo.h>



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


// @brief print the bone names of an entity
//
inline void
printBoneNames(Ogre::Entity* ent)
{
    if (ent == 0) {
        debugBLUE("Null entity\n");
        return;
    }
    Ogre::SkeletonInstance* skeleton = ent->getSkeleton();
    if (!skeleton) {
        debugBLUE("ent %s has no skeleton\n", ent->getName().c_str());
        return;
    }

    Ogre::Skeleton::BoneIterator boneIt = skeleton->getBoneIterator();
    unsigned int i = 0;
    while(boneIt.hasMoreElements()) {
        Ogre::Bone* bone = boneIt.getNext();
        debugBLUE("Bone[%d]: %s\n", i, bone->getName().c_str());
        ++i;
    }
}

// @brief print the subentities names of an entity
//
inline void
printSubentitiesNames(Ogre::Entity* ent)
{
    if (ent == 0) {
        debugBLUE("Null entity\n");
        return;
    }
    unsigned int count = ent->getNumSubEntities();
    debugBLUE("The entity %s has %d subentities\n",
        ent->getName().c_str(), ent->getNumSubEntities());
}

}


namespace tests {


////////////////////////////////////////////////////////////////////////////////
void
ZombieBodyTest::loadFloor(void)
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
ZombieBodyTest::configureBullet(void)
{
    mDynamicWorld.setGravity(Ogre::Vector3(0,0,-100));
    Ogre::AxisAlignedBox floor;
    floor.setExtents(Ogre::Vector3(-1520,-1550,-10),Ogre::Vector3(1520,1550,0));
    // leak here
    physics::BulletObject* floorObject = physics::BulletImporter::createBox(floor, 0);
    floorObject->motionState.node()->setVisible(false);
    mDynamicWorld.addObject(*floorObject);
}


////////////////////////////////////////////////////////////////////////////////
void
ZombieBodyTest::configureBody(void)
{
    Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    Ogre::Entity* ent = mSceneMgr->createEntity("zombie.mesh");
    node->attachObject(ent);

    printBoneNames(ent);
    printSubentitiesNames(ent);

    node->roll(Ogre::Radian(3.14/2));

    mRagdollQueue.configure(&mDynamicWorld.bulletDynamicWorld(),
                            ent->getSkeleton(),
                            node);
    mBody.setBodyPartQueue(&mBodyPartQueue);
    mBody.setRagDollQueue(&mRagdollQueue);
    mBody.setSceneNode(node);
    mBody.setSkeleton(ent->getSkeleton());
    mBody.setEntity(ent);
    mBody.build();
}

////////////////////////////////////////////////////////////////////////////////
physics::BulletObject*
ZombieBodyTest::performRaycast(void)
{
    // 1) Get the ray
    Ogre::Ray ray = mCamera->getCameraToViewportRay(mMouseCursor.getXRelativePos(),
                                                    mMouseCursor.getYRelativePos());

    // 2) Perform the bullet raycast
    const btVector3 from = physics::BulletUtils::ogreToBullet(ray.getOrigin());
    const btVector3 to = physics::BulletUtils::ogreToBullet(ray.getOrigin() +
        ray.getDirection() * 5000.f);
    btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
    mDynamicWorld.bulletDynamicWorld().rayTest(from, to, rayCallback);
    if (!rayCallback.hasHit()) {
        return 0;
    }
    void* ptr = rayCallback.m_collisionObject->getUserPointer();
    if (ptr == 0) {
        return 0;
    }

    // this is dangerous!
    physics::BulletObject* bo = static_cast<physics::BulletObject*>(ptr);
    return bo;
}


////////////////////////////////////////////////////////////////////////////////
void
ZombieBodyTest::handleCameraInput()
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
void
ZombieBodyTest::testBuilder(void)
{
    // We need to set the global data first
    //

    // build the blood particle queue
    //
    const unsigned int size = mBloodQueue.numAllocatedEffects();
    cz::BloodParticles* particles = mBloodQueue.getEffects();
    for (unsigned int i = 0; i < size; ++i) {
        particles[i].construct();
    }

    // configure the ZombieUnit and the queue
    cz::ZombieUnit::setEffectHandler(&mEffectHandler);
    cz::ZombieUnit::setBloodParticlesQueue(&mBloodQueue);

    // set the builder data
    mBuilder.setDynamicWorld(&mDynamicWorld);
    mBuilder.setQueues(&mRagdollQueue, &mBodyPartQueue);

    if (!mBuilder.parseXml("ZombieConfig.xml")) {
        debugERROR("Error parsing xml ZombieConfig file\n");
        return;
    }

    // build the queues
    mBuilder.fillBodyPartQueue(mBodyPartQueue);

    // build a zombie
    if (!mBuilder.loadZombie("normal", mZombieUnit)) {
        debugERROR("Error loading normal zombie\n");
        return;
    }
    mZombieUnit.born();

    // put it in scene
    mSceneMgr->getRootSceneNode()->addChild(mZombieUnit.sceneNode());

    // move it a little up
    mZombieUnit.set3DPosition(Ogre::Vector3(0,0,900));
    const core::Vector2 nextPos(0,10);
    mZombieUnit.moveAndLook(nextPos);

    // we will use that zombie as the model for the ragdolls
    mRagdollQueue.configure(&mDynamicWorld.bulletDynamicWorld(),
                            mZombieUnit.entity()->getSkeleton(),
                            mZombieUnit.sceneNode());
}

////////////////////////////////////////////////////////////////////////////////
void
ZombieBodyTest::createBulletSystem(void)
{
    Ogre::AxisAlignedBox worldLimits(-1000,-1000,-100,1000,1000,1000);
    mFiringSystem.setInfo(&mDynamicWorld, worldLimits);

    // create the bullets
    mBulletsQueue.build();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ZombieBodyTest::ZombieBodyTest() :
    core::AppTester(cz::GlobalData::lastTimeFrame)
,   mTimeFrame(cz::GlobalData::lastTimeFrame)
,   mOrbitCamera(mCamera, mSceneMgr, mTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
,   mCollHandler(mData.collHandler)
{
    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);
    setUseDefaultInput(false);

    // configure the mouse cursor
    mMouseCursor.setCursor(ui::MouseCursor::Cursor::NORMAL_CURSOR);
    mMouseCursor.setVisible(true);
    mMouseCursor.setWindowDimensions(mWindow->getWidth(), mWindow->getHeight());
    mMouseCursor.updatePosition(mWindow->getWidth() / 2,
                                mWindow->getHeight() / 2);

    // configure the global data
    cz::GlobalData::camera = mCamera;
    cz::GlobalData::collHandler = &mCollHandler;
    cz::GlobalData::sceneMngr = mSceneMgr;

    // Zombie Unit static values
    cz::ZombieUnit::setDynamicWorld(&mDynamicWorld);

    // Effects
    cz::BodyPartEffect::setDynamicWorld(&mDynamicWorld);

    // configure the static world
    core::AABB worldBB(3000,-3000,-3000,3000);
    mCollHandler.setWorldBoundingBox(worldBB);

}

////////////////////////////////////////////////////////////////////////////////
ZombieBodyTest::~ZombieBodyTest()
{
    // TODO Auto-generated destructor stub
}

////////////////////////////////////////////////////////////////////////////////
void
ZombieBodyTest::loadAditionalData(void)
{
    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

    // try to load the xml file
    loadFloor();

    configureBullet();
//
//    configureBody();

    // Test the zombie builder
    testBuilder();
    mZombieUnit.sceneNode()->showBoundingBox(true);

    // configure the bullet and firing system
    createBulletSystem();

}

////////////////////////////////////////////////////////////////////////////////
void
ZombieBodyTest::update()
{
    // update the input system
    mInputHelper.update();

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
        return;
    }

    // if click shoot a box
    if (mInputHelper.isMouseReleased(input::MouseButtonID::MB_Left)) {
        // fire a bullet
        cz::Bullet* bullet = mBulletsQueue.getAvailable();
        if (bullet) {
            const Ogre::Vector3& camDir = mCamera->getRealDirection();
            const Ogre::Vector3& camPos = mCamera->getRealPosition();
            bullet->configure(camPos - Ogre::Vector3(0,0,1.2f), camDir);
            bullet->setProperties(1.f, 444.f);
            mFiringSystem.add(bullet);
        }
    }

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_UP)) {
        core::Vector2 t(10,0);
        mZombieUnit.moveAndLook(t);
    }


    // update the camera
    handleCameraInput();

    // update dynamic world
    mCollHandler.update();
    mDynamicWorld.simulate(mTimeFrame);
    mEffectHandler.update(cz::GlobalData::lastTimeFrame);
    mZombieUnit.update();

    // firing system
    mFiringSystem.update();

}

}
