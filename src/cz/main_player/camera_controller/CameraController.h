/*
 * CameraController.h
 *
 *  Created on: Oct 10, 2013
 *      Author: agustin
 */

#ifndef CAMERACONTROLLER_H_
#define CAMERACONTROLLER_H_

#include <OgreCamera.h>
#include <OgreBone.h>
#include <OgreSceneNode.h>

#include <debug/DebugUtil.h>
#include <global_data/GlobalData.h>


namespace cz {

class CameraController
{
public:
    CameraController();
    ~CameraController();

    // @brief Set the information needed to control the camera.
    // @param baseNode  The base node where the skeleton is attached to.
    // @param cam       The Camera
    // @param entity    The entity containing the bone boneName where we will
    //                  attach the camera
    // @param boneName  The name of the bone
    // @param offsetPos The offset position of the camera in the bone
    // @param offsetRot The offset rotation of the camera in the bone
    //
    inline void
    setData(Ogre::SceneNode* baseNode,
            Ogre::Camera* cam,
            Ogre::Entity* entity,
            const char* boneName,
            const Ogre::Vector3& offsetPos,
            const Ogre::Quaternion& offsetRot);

    // @brief Set the limits for the movement of the camera in the X and Z axis
    //        (this is the range where the camera (bone) will be able to rotate).
    //        in radians.
    // @param minX      Minimum rotation angle for the axis X (roll)
    // @param maxX      Maximum rotation angle for the axis X (roll)
    // @param minZ      Minimum rotation angle for the axis Z (yaw)
    // @param maxZ      Maximum rotation angle for the axis Z (yaw)
    //
    inline void
    setConstraints(Ogre::Radian minX, Ogre::Radian maxX,
                   Ogre::Radian minZ, Ogre::Radian maxZ);

    // @brief Set / get the velocity factor to rotate the camera.
    //
    inline void
    setVelFactor(float velFactor);
    inline float
    velFactor(void) const;

    // @brief Rotate the camera in the X and Z axis simultaneously
    // @param pitch Pitch radians to rotate (X axis)
    // @param roll  Roll radians to rotate (Z axis)
    //
    inline void
    rotatePitch(Ogre::Radian pitch);
    inline void
    rotateRoll(Ogre::Radian roll);
    inline void
    rotate(Ogre::Radian roll, Ogre::Radian pitch);

private:
    // avoid copying
    CameraController(const CameraController&);
    CameraController& operator=(const CameraController&);

private:
    Ogre::Radian mPitchLimits[2];
    Ogre::Radian mRollLimits[2];
    float mVelFactor;
    Ogre::Camera* mCamera;
    Ogre::SceneNode* mBaseNode;
};


////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline CameraController::CameraController() :
    mVelFactor(1.f)
,   mCamera(0)
,   mBaseNode(0)
{
}

inline CameraController::~CameraController()
{
}

inline void
CameraController::setData(Ogre::SceneNode* baseNode,
                          Ogre::Camera* cam,
                          Ogre::Entity* entity,
                          const char* boneName,
                          const Ogre::Vector3& offsetPos,
                          const Ogre::Quaternion& offsetRot)
{
    ASSERT(baseNode);
    ASSERT(cam);
    ASSERT(entity);
    ASSERT(boneName);
    ASSERT(!mBaseNode);

    mCamera = cam;
    mBaseNode = baseNode;

    // detach any parent of the cam
    mCamera->detachFromParent();

    // attach the camera to the bone
    entity->attachObjectToBone(boneName, mCamera, offsetRot, offsetPos);
}

inline void
CameraController::setConstraints(Ogre::Radian minX, Ogre::Radian maxX,
                                 Ogre::Radian minZ, Ogre::Radian maxZ)
{
    mPitchLimits[0] = minX; mPitchLimits[1] = maxX;
    mRollLimits[0] = minZ; mRollLimits[1] = maxZ;
}


inline void
CameraController::setVelFactor(float velFactor)
{
    ASSERT(velFactor > 0);
    mVelFactor = velFactor;
}
inline float
CameraController::velFactor(void) const
{
    return mVelFactor;
}

inline void
CameraController::rotatePitch(Ogre::Radian pitch)
{
    ASSERT(mBaseNode);
    Ogre::Radian futurePitch = mBaseNode->getOrientation().getPitch() +
        (pitch * mVelFactor);
    if (futurePitch < mPitchLimits[0] || futurePitch > mPitchLimits[1]) {
        // do not rotate
        return;
    }
    // rotate
    mBaseNode->pitch(pitch * mVelFactor);
}
inline void
CameraController::rotateRoll(Ogre::Radian roll)
{
    ASSERT(mBaseNode);
    Ogre::Radian futureRoll = mBaseNode->getOrientation().getRoll() +
        (roll * mVelFactor);
    if (futureRoll < mRollLimits[0] || futureRoll > mRollLimits[1]) {
        // do not rotate
        return;
    }
    // rotate
    mBaseNode->roll(roll * mVelFactor, Ogre::Node::TS_WORLD);
}
inline void
CameraController::rotate(Ogre::Radian roll, Ogre::Radian pitch)
{
    rotatePitch(pitch); rotateRoll(roll);
}

} /* namespace cz */
#endif /* CAMERACONTROLLER_H_ */
