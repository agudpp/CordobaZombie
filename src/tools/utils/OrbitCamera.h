/*
 * OrbitCamera.h
 *
 *  Created on: Jun 9, 2013
 *      Author: agustin
 */

#ifndef ORBITCAMERA_H_
#define ORBITCAMERA_H_

#include <OgreSceneNode.h>
#include <OgreCamera.h>
#include <OgreSceneManager.h>

#include <debug/DebugUtil.h>
#include <debug/OgreText.h>

namespace tool {

class OrbitCamera
{
public:

    // define the type of camera we want to use
    //
    enum CameraType {
        FreeFly = 0,
        Orbit,
    };

public:
    OrbitCamera(Ogre::Camera* camera,
                Ogre::SceneManager* manager,
                float& globalTimeFrame);
    ~OrbitCamera();

    /**
     * @brief Set the camera type to be used
     */
    void
    setCameraType(CameraType type);

    inline void
    rotateCamera(const Ogre::Radian yaw, const Ogre::Radian pitch);
    inline void
    setZoom(const Ogre::Real zoom);
    inline Ogre::Real
    zoom(void) const {return mZoom;}

    // Move the camera in axis x,z (only for freeFly camera)
    void
    moveCamera(const Ogre::Vector3 &dir);

    // get the position of the camera
    inline const Ogre::Vector3&
    getCameraPosition(void) const;

private:
    Ogre::Camera* mCamera;
    Ogre::SceneNode* mCamXNode;
    Ogre::SceneNode* mCamYNode;
    Ogre::SceneNode* mZoomNode;
    Ogre::SceneNode* mFreeNode;
    CameraType mCamType;
    Ogre::Real mZoom;
    Ogre::Real mStartDistance;
    Ogre::Real mCamVelocityFactor;
    float& mGlobalTimeFrame;
    core::OgreText mText;
};


// inline
//
inline void
OrbitCamera::rotateCamera(const Ogre::Radian yaw, const Ogre::Radian pitch)
{
    switch (mCamType) {
    case CameraType::FreeFly:
        mFreeNode->yaw(yaw, Ogre::Node::TS_WORLD);
        mFreeNode->pitch(pitch);
        break;
    case CameraType::Orbit:
        mCamXNode->yaw(yaw, Ogre::Node::TS_WORLD);
        mCamYNode->pitch(pitch);
        break;
    default:
        ASSERT(false);
    }
}

inline void
OrbitCamera::setZoom(const Ogre::Real zoom)
{
    if (mCamType != CameraType::Orbit) {
        // not supported
        return;
    }
    Ogre::Vector3 posVec = mZoomNode->getPosition();
    posVec.normalise();
    mZoom = (zoom <= 0) ? 0.01f : zoom;
    const Ogre::Real len = mZoom * mStartDistance * 0.01f;
    mZoomNode->setPosition(posVec * len);
}

inline const Ogre::Vector3&
OrbitCamera::getCameraPosition(void) const
{
    return mCamera->getRealPosition();
}

} /* namespace tool */
#endif /* ORBITCAMERA_H_ */
