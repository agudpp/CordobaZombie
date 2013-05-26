/*
 * MeshPlayer.h
 *
 *  Created on: Mar 23, 2013
 *      Author: agustin
 */

#ifndef MESHPLAYER_H_
#define MESHPLAYER_H_

#include <vector>

#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreAnimationState.h>

#include <CameraController/CameraController.h>
#include <Extras/Helpers/MovableText.h>


namespace extras {

class MeshPlayer
{

    struct MeshInfo {
        Ogre::Entity *entity;
        Ogre::SceneNode *node;
        std::vector<Ogre::AnimationState *> anims;
        size_t currentAnim;
        Ogre::MovableTextPtr mText;

        void configureAnimation(size_t old, size_t newAnim)
        {
            anims[old]->setEnabled(false);
            anims[newAnim]->setTimePosition(0);
            anims[newAnim]->setEnabled(true);
            currentAnim = newAnim;

            // set the name
            if (mText.get()) {
                mText->setCaption(anims[newAnim]->getAnimationName());
            }
        }
    };

public:
    MeshPlayer();
    ~MeshPlayer();

    /**
     * @brief Run this app
     */
    void run(void);

private:

    void
    handleInput(void);

    void
    update();

    void
    loadAnimations(MeshInfo &mi);

    bool
    loadMeshes(const std::string &file, std::vector<MeshInfo> &result);

    void
    positionMeshes(std::vector<MeshInfo> &result);

private:
    CameraController mCamController;
    std::vector<MeshInfo> mMeshes;
};

} /* namespace extras */
#endif /* MESHPLAYER_H_ */
