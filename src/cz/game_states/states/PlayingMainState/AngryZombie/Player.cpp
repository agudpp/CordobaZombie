/*
 * Player.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#include "Player.h"

namespace demo_app {

Player::Player() :
    mSceneMngr(0)
,   mCamera(0)
,   mCamXNode(0)
,   mCamYNode(0)
,   mZoomNode(0)
,   mRotVelFactor(1.f)
{
}

Player::~Player()
{
    if (mCamXNode) {
        Ogre::SceneNode* rootNode = mCamXNode->getParentSceneNode();
        rootNode->removeAndDestroyChild(mCamXNode->getName());
    }
}

////////////////////////////////////////////////////////////////////////////////
void
Player::configure(Ogre::SceneManager* sceneMngr, Ogre::Camera* camera)
{
    ASSERT(sceneMngr);
    ASSERT(camera);
    mSceneMngr = sceneMngr;
    mCamera = camera;

    camera->setOrientation(Ogre::Quaternion::IDENTITY);

    Ogre::SceneNode* rootNode = mSceneMngr->getRootSceneNode();
    mCamXNode = rootNode->createChildSceneNode();
    mCamYNode = mCamXNode->createChildSceneNode();
    mZoomNode = mCamYNode->createChildSceneNode();

    // rotate the root node to look -Y instead of -Z
    mCamXNode->roll(Ogre::Radian(M_PI_2), Ogre::Node::TS_WORLD);

    mCamYNode->setFixedYawAxis(true, Ogre::Vector3::UNIT_Z);
    mZoomNode->setFixedYawAxis(true, Ogre::Vector3::UNIT_Z);

    camera->detachFromParent();
    mZoomNode->attachObject(camera);
}

////////////////////////////////////////////////////////////////////////////////
void
Player::setPositions(const Ogre::Vector3& orbitCenter, const Ogre::Vector3& playerPos)
{
    ASSERT(mCamXNode);

    // we will set the camera at the same level than the orbit center for now
    mCamXNode->setPosition(orbitCenter);
    Ogre::Vector3 camPos = playerPos;
    camPos.z = orbitCenter.z;
    mCamYNode->setPosition(camPos);
    mCamYNode->lookAt(orbitCenter, Ogre::Node::TS_WORLD);
    mCamYNode->roll(Ogre::Radian(M_PI_2));
    mZoomNode->lookAt(orbitCenter, Ogre::Node::TS_WORLD);
}

////////////////////////////////////////////////////////////////////////////////
void
Player::rotateCamera(const Ogre::Radian yaw,
                     const Ogre::Radian pitch,
                     const Ogre::Radian roll)
{
    ASSERT(mCamYNode);

    // check if the current pitch exceeds the maximum
    mCamXNode->roll(yaw, Ogre::Node::TS_LOCAL);

    // check if this exceeds the maximum or minimum
    Ogre::Radian next = mCamYNode->getOrientation().getYaw() + pitch;
//    if (next < mRotLimits[0] || next > mRotLimits[1]) {
//        return; // do nothing
//    }
    // else apply the rotation
    mCamYNode->pitch(roll);
    mZoomNode->pitch(pitch);
}

} /* namespace demo_app */
