/*
 * StaticWorldObjectTest.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include "StaticWorldObjectTest.h"

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

#include <CZMasksDefines.h>

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


inline Ogre::Quaternion
globalOgreRotation(const Ogre::Bone *bone, const Ogre::SceneNode* parentNode)
{
    return parentNode->_getDerivedOrientation() * bone->_getDerivedOrientation();
}
inline Ogre::Vector3
globalOgrePosition(const Ogre::Bone *bone, const Ogre::SceneNode* parentNode)
{
    return parentNode->getPosition() + parentNode->getOrientation() *
        bone->_getDerivedPosition();
}


}


namespace tests {


////////////////////////////////////////////////////////////////////////////////
void
StaticWorldObjectTest::loadFloor(void)
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
StaticWorldObjectTest::configureBullet(void)
{
    Ogre::AxisAlignedBox floor;
    floor.setExtents(Ogre::Vector3(-1520,-1550,-10),Ogre::Vector3(1520,1550,0));
    // leak here
    physics::BulletObject* floorObject = physics::BulletImporter::createBox(floor, 0);
    floorObject->motionState.node()->setVisible(false);
    mDynamicWorld.addObject(*floorObject);
}



////////////////////////////////////////////////////////////////////////////////
physics::BulletObject*
StaticWorldObjectTest::performRaycast(void)
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
StaticWorldObjectTest::handleCameraInput()
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
StaticWorldObjectTest::loadStaticWorldObject(void)
{
    // configure the staticBuilder
    mStaticBuilder.setShapeHolder(&mShapeHolder);
    mStaticBuilder.setWorldStaticObjectHolder(&mWorldObjectsHolder);

    cz::SceneAssetLoader assetLoader;
    assetLoader.addBuilder(&mStaticBuilder);

    // configure the assetLoader
    std::string scenePath = "test_19_10.scene";
    std::string rootRscPath;
    if (!cz::GlobalData::getRootResourcesPath(rootRscPath)) {
        debugERROR("Error!!\n");
        return;
    }
    scenePath = rootRscPath + "test/static_world_objects/" + scenePath;

    // load the scene
    if (!assetLoader.loadScene(scenePath, rootRscPath, mSceneMgr)) {
        debugERROR("Error loading scene %s\n", scenePath.c_str());
        return;
    }

    // add all of them into the scene
    core::DataHolder<cz::StaticWorldObject*>::ElementVec& swos =
        mWorldObjectsHolder.elements();
    for (cz::StaticWorldObject* swo : swos) {
        mSceneMgr->getRootSceneNode()->addChild(swo->sceneNode());
    }



//    const std::string collTestMesh = "test_coll.mesh";
//    const std::string collTestMesh2D = "test_coll_2D.mesh";
//    const std::string collTestMesh3D = "test_coll.mesh";
//    const std::string coll2D = "test_coll.2DColl";
//    const std::string coll3D = "test_coll.3DColl";
//
//    Ogre::Entity* ent = mSceneMgr->createEntity(collTestMesh);
//    Ogre::Entity* ent2D = mSceneMgr->createEntity(collTestMesh2D);
//    Ogre::Entity* ent3D = mSceneMgr->createEntity(collTestMesh3D);
//    ASSERT(ent && ent2D && ent3D && "We couldn't find mesh! resource problem?");
//
//    // save 2D and 3D collision mesh representation
//    if (!coll::CollObjectExporter::transform(ent2D->getMesh().get(), coll2D)) {
//        debugERROR("Error exporting the mesh %s to %s\n",
//            collTestMesh.c_str(), coll2D.c_str());
//        return;
//    }
//    if (!physics::BulletExporter::transform(ent3D->getMesh().get(), coll3D)) {
//        debugERROR("Error exporting mesh %s to %s\n",
//            collTestMesh.c_str(), coll3D.c_str());
//        return;
//    }
//
//    // import that data now
//    coll::CollPreciseInfo* rep2D = 0;
//    core::AABB aabb;
//    if (!coll::CollObjectExporter::buildFromFile(coll2D, rep2D, aabb)) {
//        debugERROR("Error importing 2D\n");
//        return;
//    }
//
//    physics::BulletShape* rep3D = physics::BulletExporter::buildShapeFromFile(coll3D);
//    if (!rep3D) {
//        debugERROR("Error importing 3D\n");
//        return;
//    }
//
//    // now configure the static world object
//    mStaticWorldObject.setCollisionInformation(rep2D, aabb);
//    mStaticWorldObject.setShape(rep3D);
//
//    Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
//    node->attachObject(ent);
//    node->translate(50,100,150);
////    node->yaw(Ogre::Radian(Ogre::Math::PI/2));
////    node->pitch(Ogre::Radian(Ogre::Math::PI/2));
////    node->roll(Ogre::Radian(Ogre::Math::PI/2));
//    mStaticWorldObject.configure(node, ent);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
StaticWorldObjectTest::StaticWorldObjectTest() :
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
    cz::PhysicGameObject::setDynamicWorld(&mDynamicWorld);

    // configure the static world
    core::AABB worldBB(3000,-3000,-3000,3000);
    mCollHandler.setWorldBoundingBox(worldBB);

}

////////////////////////////////////////////////////////////////////////////////
StaticWorldObjectTest::~StaticWorldObjectTest()
{
    // TODO Auto-generated destructor stub
}

////////////////////////////////////////////////////////////////////////////////
void
StaticWorldObjectTest::loadAditionalData(void)
{
    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }

    // load the resources for the game configuring the resource handler
    std::string rootRscPath;
    if (!cz::GlobalData::getRootResourcesPath(rootRscPath)) {
        debugERROR("Error!!\n");
        return;
    }
    mResourceHandler.setResourceRootPath(rootRscPath);
    rrh::ResourceGroup rg;
    rg.setOgreResourceFile(rootRscPath + "test/static_world_objects/resources.cfg");
    mResourceHandler.loadResourceGroup(rg);


    // try to load the xml file
    loadFloor();

    configureBullet();

    // create the static world object
    loadStaticWorldObject();
}

////////////////////////////////////////////////////////////////////////////////
void
StaticWorldObjectTest::update()
{
    // update the input system
    mInputHelper.update();

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
        return;
    }

    if (mInputHelper.isMouseReleased(input::MouseButtonID::MB_Middle)) {
        Ogre::Ray ray = mCamera->getCameraToViewportRay(mMouseCursor.getXRelativePos(),
                                                        mMouseCursor.getYRelativePos());
        const Ogre::Vector3 halfSize(2.5,2.5,2.5);
        const Ogre::Vector3 camPos = mCamera->getRealPosition();
        Ogre::AxisAlignedBox bb(camPos - halfSize, camPos + halfSize);
        physics::BulletObject* bo = physics::BulletImporter::createBox(bb, 10);
        mDynamicWorld.addObject(*bo);

        Ogre::Vector3 force(mCamera->getDerivedDirection());
        force.normalise();
        force *= 1700;
        btVector3 btForce(force.x, force.y, force.z);
        bo->rigidBody->applyCentralImpulse(btForce);
    }

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_LEFT)) {
        Ogre::Quaternion current = mStaticWorldObject.rotation();
        current = current * Ogre::Quaternion(Ogre::Degree(0.1), Ogre::Vector3::UNIT_Z);
        mStaticWorldObject.configureRotation(current);
    }

    // update the camera
    handleCameraInput();

    // update dynamic world
    mCollHandler.update();
    mDynamicWorld.simulate(mTimeFrame);

}

}
