/* @brief This class will represent the Satellite camera mode.
 *        This camera will always be looking at the floor and will be not able
 *        to rotate it
 * SatelliteCamera.h
 *
 *  Created on: Jul 10, 2013
 *      Author: agustin
 */

#ifndef SATELLITECAMERA_H_
#define SATELLITECAMERA_H_

#include <OgreSceneNode.h>
#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreAxisAlignedBox.h>

#include <debug/DebugUtil.h>
#include <debug/OgreText.h>

namespace tool {

class SatelliteCamera {
public:

    // Constructor / destructor
    SatelliteCamera(Ogre::Camera* camera,
                    Ogre::SceneManager* manager,
                    float& globalTimeFrame);
    virtual ~SatelliteCamera();

    // @brief Configure the camera satellite
    // @param zone  The movement zone for where the camera will be able to move
    // @param vel   [optional] The velocity factor
    //
    void
    configure(const Ogre::AxisAlignedBox& zone, Ogre::Real vel = 1.f);

    // @brief Translate the camera (only if it the next position is still inside
    //        the movement zone).
    // @param t     The translate vector
    //
    inline void
    translate(const Ogre::Vector3& t);

    // @brief Set the position of the camera if and only if the new position
    //        is inside the movement zone
    // @param pos   The new position where the camera will be moved.
    //
    void
    setCamPos(const Ogre::Vector3& pos);

    // @brief Get the camera position in the world
    //
    inline const Ogre::Vector3&
    getWorldCamPos(void) const;


private:
    core::OgreText mText;
    Ogre::Camera* mCamera;
    float& mGlobalTimeFrame;
    Ogre::SceneNode* mCamNode;
    Ogre::AxisAlignedBox mMoveZone;
    Ogre::Real mVelFactor;
};



////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
SatelliteCamera::translate(const Ogre::Vector3& t)
{
    setCamPos(mCamNode->getPosition() - (t * mVelFactor));
}
inline const Ogre::Vector3&
SatelliteCamera::getWorldCamPos(void) const
{
    ASSERT(mCamera);
    return mCamera->getRealPosition();
}
}

#endif /* SATELLITECAMERA_H_ */
