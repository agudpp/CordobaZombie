/*
 * SatelliteCamera.cpp
 *
 *  Created on: Jul 10, 2013
 *      Author: agustin
 */

#include "SatelliteCamera.h"

namespace tool {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
SatelliteCamera::SatelliteCamera(Ogre::Camera* camera,
                                 Ogre::SceneManager* manager,
                                 float& globalTimeFrame) :
    mCamera(camera)
,   mGlobalTimeFrame(globalTimeFrame)
,   mCamNode(0)
,   mVelFactor(1.f)
{
    ASSERT(camera); ASSERT(manager);

    Ogre::SceneNode* parent = mCamera->getParentSceneNode();
    if (parent) {
        parent->detachObject(mCamera);
    }

    Ogre::SceneNode* rootNode = manager->getRootSceneNode();
    mCamNode = rootNode->createChildSceneNode();
    mCamNode->attachObject(mCamera);
    mCamNode->lookAt(Ogre::Vector3(0, 0, -10), Ogre::Node::TransformSpace::TS_WORLD);

    // configure the text
    mText.setPos(0.75f, 0.97f);
    mText.setText("CameraMode: Satellite",  0.02f);
}

////////////////////////////////////////////////////////////////////////////////
SatelliteCamera::~SatelliteCamera()
{
    if (mCamNode && mCamNode->getParentSceneNode()) {
        mCamNode->getParentSceneNode()->removeAndDestroyChild(mCamNode->getName());
    }
}

////////////////////////////////////////////////////////////////////////////////
void
SatelliteCamera::configure(const Ogre::AxisAlignedBox& zone, Ogre::Real vel)
{
    mMoveZone = zone;
    mVelFactor = vel;
    setCamPos(mMoveZone.getCenter());
}

////////////////////////////////////////////////////////////////////////////////
void
SatelliteCamera::setCamPos(const Ogre::Vector3& pos)
{
    if (!mMoveZone.contains(pos)) {
        // the new position is not inside the movement zone
        return;
    }
    ASSERT(mCamNode);
    mCamNode->setPosition(pos);
}


}
