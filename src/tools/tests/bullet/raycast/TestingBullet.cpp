/*
 * TestingBullet.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include "TestingBullet.h"

#include <OgreMath.h>
#include <OgreAnimationState.h>
#include <OgreResourceManager.h>
#include <OgreFontManager.h>

#include <memory>
#include <cstring>

#include <tinyxml/tinyxml.h>

#include <xml/XMLHelper.h>
#include <physics/BulletUtils.h>
#include <physics/BulletImporter.h>
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

}


namespace tests {


////////////////////////////////////////////////////////////////////////////////
void
TestingBullet::loadFloor(void)
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
TestingBullet::configureBullet(void)
{
    Ogre::AxisAlignedBox floor;
    floor.setExtents(Ogre::Vector3(-1520,-1550,-10),Ogre::Vector3(1520,1550,0));
    // leak here
    physics::BulletObject* floorObject = physics::BulletImporter::createBox(floor, 0);
    floorObject->motionState.node()->setVisible(false);
    mDynamicWorld.addObject(*floorObject);
}


////////////////////////////////////////////////////////////////////////////////
void
TestingBullet::createSampleScene(void)
{
    core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
    Ogre::Vector3 size(20,20,20);
    float currentZ = 10;
    for (unsigned int i = 0; i < 20; ++i) {
        for (unsigned int j = 0; j < 20; ++j) {
            Ogre::Vector3 min(j*size.x+2,0,currentZ);
            Ogre::Vector3 max((j+1)*size.x,10,currentZ + size.z);

            Ogre::AxisAlignedBox bb(min, max);
            mObjects.push_back(physics::BulletImporter::createBox(bb, 10));
            mDynamicWorld.addObject(*mObjects.back());

        }
        currentZ += size.z + 2;
    }
}

////////////////////////////////////////////////////////////////////////////////
physics::BulletObject*
TestingBullet::performRaycast(void)
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
TestingBullet::handleCameraInput()
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

TestingBullet::TestingBullet() :
    core::AppTester(mTimeFrame)
,   mOrbitCamera(mCamera, mSceneMgr, mTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
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

}

TestingBullet::~TestingBullet()
{
    // TODO Auto-generated destructor stub
}

/* Load additional info */
void
TestingBullet::loadAditionalData(void)
{
    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

    // try to load the xml file
    loadFloor();

    configureBullet();

    createSampleScene();

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
TestingBullet::update()
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
        Ogre::Ray ray = mCamera->getCameraToViewportRay(mMouseCursor.getXRelativePos(),
                                                        mMouseCursor.getYRelativePos());
        physics::RaycastResult result;
        if (mDynamicWorld.performClosestRay(physics::RaycastInfo(ray), result)) {
            result.object->motionState.node()->flipVisibility();
            core::PrimitiveDrawer& pd = core::PrimitiveDrawer::instance();
            Ogre::Vector3 pos = physics::BulletUtils::bulletToOgre(result.worldPosition);
            Ogre::Vector3 normal = physics::BulletUtils::bulletToOgre(result.worldNormal);
            pd.createLine(pos, pos + normal * 15.f, pd.getFreshColour());
        }

    }
    // update the camera
    handleCameraInput();

    // update dynamic world
    mDynamicWorld.simulate(mTimeFrame);
}

}
