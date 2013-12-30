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
                float& globalTimeFrame,
                bool showText = true);
    ~OrbitCamera();

    /**
     * @brief Set the camera type to be used
     */
    void
    setCameraType(CameraType type);
    inline CameraType
    cameraType(void) const;

    inline void
    rotateCamera(const Ogre::Radian yaw, const Ogre::Radian pitch);
    inline void
    setYawAngle(const Ogre::Degree angle);
    inline void
    setPitchAngle(const Ogre::Degree angle);
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
    core::OgreText* mText;
};


// inline
//
inline OrbitCamera::CameraType
OrbitCamera::cameraType(void) const
{
    return mCamType;
}

inline void
OrbitCamera::rotateCamera(const Ogre::Radian yaw, const Ogre::Radian pitch)
{
    switch (mCamType) {
    case CameraType::FreeFly:
        mFreeNode->roll(yaw, Ogre::Node::TS_WORLD);
        mFreeNode->pitch(pitch);
        break;
    case CameraType::Orbit:
        mCamXNode->roll(yaw, Ogre::Node::TS_WORLD);
        mCamYNode->pitch(pitch);
        break;
    default:
        ASSERT(false);
    }
}

inline void
OrbitCamera::setYawAngle(const Ogre::Degree angle)
{
    switch (mCamType) {
    case CameraType::FreeFly:
        break;
    case CameraType::Orbit:
    {
        Ogre::Quaternion rot(angle, Ogre::Vector3::UNIT_X);
        mCamYNode->setOrientation(rot);
    }
        break;
    default:
        ASSERT(false);
    }
}
inline void
OrbitCamera::setPitchAngle(const Ogre::Degree angle)
{
    switch (mCamType) {
    case CameraType::FreeFly:
        break;
    case CameraType::Orbit:
    {
        Ogre::Quaternion rot(angle, Ogre::Vector3::UNIT_Y);
        mCamXNode->setOrientation(rot);
    }
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
