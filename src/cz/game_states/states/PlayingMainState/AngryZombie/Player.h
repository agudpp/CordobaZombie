/*
 * Player.h
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreSceneNode.h>

namespace demo_app {

class Player
{
public:
    Player();
    ~Player();

    // @brief Configure the player from the camera and scene manager
    //
    void
    configure(Ogre::SceneManager* sceneMngr, Ogre::Camera* camera);

    // @brief Set the center of the orbit position and the position of the player
    //        (camera)
    // @param orbitCenter       Center of orbit position
    // @param playerPos         The player pos
    //
    void
    setPositions(const Ogre::Vector3& orbitCenter, const Ogre::Vector3& playerPos);

    // @brief Set the orbit rotation velocity factor
    //
    inline void
    setRotationVelocityFactor(float rotVelFactor);

    // @brief Rotate the camera over the 2 angles supported
    //
    inline void
    rotateCamera(const Ogre::Radian yaw, const Ogre::Radian pitch);

private:
    Ogre::SceneManager* mSceneMngr;
    Ogre::Camera* mCamera;
    Ogre::SceneNode* mCamXNode;
    Ogre::SceneNode* mCamYNode;
    Ogre::SceneNode* mZoomNode;
    float mRotVelFactor;
};

} /* namespace demo_app */
#endif /* PLAYER_H_ */
