/*
 * MeshPlayer.cpp
 *
 *  Created on: Mar 23, 2013
 *      Author: agustin
 */

#include "MeshPlayer.h"

#include <fstream>
#include <sstream>

#include <OgreTimer.h>
#include <OgrePlane.h>
#include <OgreMeshManager.h>
#include <OgreWindowEventUtilities.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>

#include <Common/GlobalObjects/GlobalObjects.h>
#include <Utils/MouseCursor/MouseCursor.h>

namespace extras {


// handle input
void
MeshPlayer::handleInput(void)
{


    ///////////////////////////////////////////////////////////////////////////
    // CAMERA
//  float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
    Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;
    Ogre::Real zoom = mCamController.zoom();

    // HERE WE DEFINE THE KEYS USED TO MOVE THE CAMERA, WE WILL USE THE
    // ARROWS TO MOVE THE CAMERA
    // NOTE: we are getting the cameraPosition and modifying the position
    // without using translation, this is because we want to move always
    // in the same axis whatever be the direction of the camera.


    // MOUSE
    const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
    GLOBAL_CURSOR->updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

    if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_LEFT) ||
        GLOBAL_KEYBOARD->isKeyDown(OIS::KC_A) ||
            lMouseState.X.abs <= 0)
    {
        mTranslationVec.x -= 1.0f;
    }
    if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_RIGHT) ||
        GLOBAL_KEYBOARD->isKeyDown(OIS::KC_D) ||
            lMouseState.X.abs >= lMouseState.width)
    {
        mTranslationVec.x += 1.0f;
    }
    if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_Q))
    {
        zoom += 0.05f;
    }
    if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_E))
    {
        zoom -= 0.05f;
    }
    if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_UP) ||
        GLOBAL_KEYBOARD->isKeyDown(OIS::KC_W) ||
            lMouseState.Y.abs <= 0)
    {
        mTranslationVec.z -= 1.0f;
    }
    if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_DOWN) ||
        GLOBAL_KEYBOARD->isKeyDown(OIS::KC_S) ||
            lMouseState.Y.abs >= lMouseState.height)
    {
        mTranslationVec.z += 1.0f;
    }

    if(mTranslationVec != Ogre::Vector3::ZERO)
    {
        mCamController.moveCamera(mTranslationVec);
    }
    if(zoom != mCamController.zoom()){
        mCamController.zoomCamera(zoom);
    }


    const float lMouseZ = float(lMouseState.Z.rel);
    float scrollZoom = mCamController.zoom();
    if (lMouseZ > 0.0f) {
        scrollZoom += 1.f;
    } else if (lMouseZ < 0.0f) {
        scrollZoom -= 1.f;
    }
    if(scrollZoom != mCamController.zoom()){
        mCamController.zoomCamera(scrollZoom);
    }

    // check tracking camera
    static int lastX = 0, lastY = 0;
    const float lMouseX = float(lMouseState.X.rel);
    const float lMouseY = float(lMouseState.Y.rel);
    if(lMouseState.buttonDown(OIS::MB_Right)){
        const float factor = -0.01 * mCamController.getRotationVelocity();
        mCamController.rotateCamera(Ogre::Radian(lMouseX * factor),
                                    Ogre::Radian(lMouseY * factor));
    }


    // Extras keys
    //
    if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)) {
        mCamController.setCamPos(Ogre::Vector3(0.f, 0.1f, 0.f));
    }
    if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_PGUP)) {
        mCamController.setCameraVelocity(mCamController.getCameraVelocity() + 0.01f);
    }
    if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_DOWN)) {
        mCamController.setCameraVelocity(mCamController.getCameraVelocity() - 0.01f);
    }

}


/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
MeshPlayer::update()
{
    static bool kPressed = false;

    handleInput();

//    mInputManager.update();

    // update the animations
    for (size_t i = 0, size = mMeshes.size(); i < size; ++i) {
        MeshInfo &mi = mMeshes[i];
        mi.anims[mi.currentAnim]->addTime(GLOBAL_TIME_FRAME);
        if (mi.anims[mi.currentAnim]->hasEnded()) {
            mi.configureAnimation(mi.currentAnim,
                                  (mi.currentAnim + 1) % mi.anims.size());
        }
    }

}

void
MeshPlayer::loadAnimations(MeshInfo &mi)
{
    Ogre::AnimationStateSet *allAnim = mi.entity->getAllAnimationStates();
    Ogre::AnimationStateIterator it = allAnim->getAnimationStateIterator();

    while(it.hasMoreElements()){
        Ogre::AnimationState *anim = it.getNext();
        if (!anim) {
            break;
        }
        mi.anims.push_back(anim);
    }
    mi.currentAnim = 0;
    mi.anims[0]->setEnabled(true);
    mi.mText->setCaption(mi.anims[0]->getAnimationName());
}


bool
MeshPlayer::loadMeshes(const std::string &fileName, std::vector<MeshInfo> &result)
{
    // we will parse all the lines of the file and load all these meshes and put
    // them in different places
    std::ifstream file(fileName);

    if (file.fail()) {
        debugERROR("Error opening file: %s\n", fileName.c_str());
        return false;
    }

    std::string meshName;
    size_t i = 0;
    while (file >> meshName) {
        MeshInfo mi;
        try {
            mi.entity = GLOBAL_SCN_MNGR->createEntity(meshName);
            mi.node = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();
            mi.node->attachObject(mi.entity);
            mi.currentAnim = 0;
        } catch (...) {
            debugERROR("Error loading mesh %s\n", meshName.c_str());
        }

        std::stringstream ss;
        ss << i;
        mi.mText.reset(new Ogre::MovableText(ss.str(), "None"));


        // configure animations
        loadAnimations(mi);

        result.push_back(mi);
        ++i;
    }
    return true;
}

void
MeshPlayer::positionMeshes(std::vector<MeshInfo> &result)
{
    Ogre::Real x = 0.f, z = 0.f;
    for (size_t i = 0, size = result.size(); i < size; ++i) {
        MeshInfo &mi = result[i];
        Ogre::Vector3 bbsize = mi.entity->getBoundingBox().getSize();
        x += bbsize.x;
        z += bbsize.z;

        mi.node->setPosition(x * 2.f, bbsize.y / 2.f, z * 2.f);

        // set the position of the animation text
        Ogre::SceneNode *animNamePos = mi.node->createChildSceneNode();
        animNamePos->attachObject(mi.mText.get());
        Ogre::Vector3 textSize = mi.mText->GetAABB().getSize();
        animNamePos->translate(0.f/*-textSize.x / 2.f*/,
                               bbsize.y,
                               0.f/*-textSize.z / 2.f*/);



        debugGREEN("Positioning mesh %s in (%f, %f, %f)\n",
            mi.entity->getMesh()->getName().c_str(),
            mi.node->getPosition().x,
            mi.node->getPosition().y,
            mi.node->getPosition().z);
    }
}

MeshPlayer::MeshPlayer()
{
    if (GLOBAL_CAMERA->getParentSceneNode()) {
        GLOBAL_CAMERA->getParentSceneNode()->detachAllObjects();
    }
    mCamController.setCamera(GLOBAL_CAMERA);

    Ogre::AxisAlignedBox mb;
    mb.setMaximum(800,800,800);
    mb.setMinimum(-800,0,-800);
    mCamController.setCameraMoveZone(mb);

    Ogre::Plane plane;
    plane.redefine(Ogre::Vector3::UNIT_Y, Ogre::Vector3(0,-1,0));
    // create the green plane
    Ogre::MeshManager::getSingleton().createPlane("floor", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 52500, 52500, 25, 25, true, 1, 10000, 10000, Ogre::Vector3::UNIT_Z);

    Ogre::Entity* floor = GLOBAL_SCN_MNGR->createEntity("Floor", "floor");
    floor->setMaterialName("Ogre/Skin");//("grass");
    floor->setCastShadows(false);
    Ogre::SceneNode *aux = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode("floorNode");
    aux->attachObject(floor);
    aux->translate(0,0,0);


    // Load the entities
    loadMeshes("meshes.txt", mMeshes);
    positionMeshes(mMeshes);
}

MeshPlayer::~MeshPlayer()
{
}

void
MeshPlayer::run()
{
    Ogre::Timer timer;
    float timeStamp = 0;

    // here is the main loop
    // TODO: fix the FrameElapsedTime and check how to get the ogres one.

    while(true) {
        timeStamp = timer.getMilliseconds();


        // handle local input       capture input
        GLOBAL_KEYBOARD->capture();
        GLOBAL_MOUSE->capture();

        if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_ESCAPE)) {
            // we have to exit
            break;
        }

        handleInput();


        // render the frame
        if(!GLOBAL_ROOT->renderOneFrame())
            break;

        update();

        // This must be called when we use the renderOneFrame approach
        Ogre::WindowEventUtilities::messagePump();

        Common::GlobalObjects::lastTimeFrame = (timer.getMilliseconds() - timeStamp) * 0.001;
    }

    return;
}



} /* namespace extras */
