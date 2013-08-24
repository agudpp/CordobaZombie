/*
 * OrbitCamera.cpp
 *
 *  Created on: Jun 9, 2013
 *      Author: agustin
 */

#include "OrbitCamera.h"


namespace tool {

////////////////////////////////////////////////////////////////////////////////
void
OrbitCamera::setCameraType(CameraType type)
{
    if (mCamType == type) {
        return;
    }


    ASSERT(mCamera);
    mCamType = type;

    Ogre::SceneNode* parent = mCamera->getParentSceneNode();
    if (parent) {
        parent->detachObject(mCamera);
    }
    switch (type) {
    case CameraType::FreeFly:
        mFreeNode->attachObject(mCamera);
        mText.setText("CameraType: FreeFly", 0.02f);
        break;

    case CameraType::Orbit:
        mZoomNode->attachObject(mCamera);
        mText.setText("CameraType: Orbit", 0.02f);
        break;
    default:
        debugERROR("Type %d not supported\n", type);
        ASSERT(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
OrbitCamera::OrbitCamera(Ogre::Camera* camera,
                         Ogre::SceneManager* manager,
                         float& globalTimeFrame) :
    mCamera(camera)
,   mCamXNode(0)
,   mCamYNode(0)
,   mZoomNode(0)
,   mFreeNode(0)
,   mCamType(CameraType::FreeFly)
,   mZoom(50.f)
,   mCamVelocityFactor(1.f)
,   mGlobalTimeFrame(globalTimeFrame)
{
    ASSERT(camera);
    ASSERT(manager);

    Ogre::SceneNode* rootNode = manager->getRootSceneNode();
    mCamXNode = rootNode->createChildSceneNode();
    mCamYNode = mCamXNode->createChildSceneNode();
    mZoomNode = mCamYNode->createChildSceneNode();

    mZoomNode->setPosition(0.f, 0.f, -100.f);
    mZoomNode->lookAt(mCamXNode->getPosition(), Ogre::Node::TS_WORLD);

    mFreeNode = rootNode->createChildSceneNode();
    mFreeNode->setPosition(0.f, 0.f, -100.f);
    mFreeNode->lookAt(mCamXNode->getPosition(), Ogre::Node::TS_WORLD);

    const Ogre::Vector3 distVec = mCamXNode->getPosition() - mZoomNode->getPosition();
    mStartDistance = distVec.length();

    // configure the text
    mText.setPos(0.75f, 0.97f);
    setCameraType(CameraType::Orbit);
}

OrbitCamera::~OrbitCamera()
{
    Ogre::SceneNode* rootNode = mCamXNode->getParentSceneNode();
    rootNode->removeAndDestroyChild(mCamXNode->getName());
    rootNode->removeAndDestroyChild(mFreeNode->getName());
}

////////////////////////////////////////////////////////////////////////////////
void
OrbitCamera::moveCamera(const Ogre::Vector3 &dir)
{
    if (mCamType != CameraType::FreeFly) {
        // nothing to do
        return;
    }
    Ogre::Vector3 mNextPosition = mFreeNode->getPosition() +
        mFreeNode->getOrientation() * (dir *(mCamVelocityFactor *
                    mGlobalTimeFrame * 200.f));

    mFreeNode->setPosition(mNextPosition);
}

} /* namespace tool */
