/*
 * Bullet.h
 *
 *  Created on: Sep 10, 2013
 *      Author: agustin
 */

#ifndef BULLET_H_
#define BULLET_H_

#include <OgreVector3.h>
#include <OgreSceneNode.h>
#include <OgreManualObject.h>
#include <OgreSceneManager.h>

#include <debug/DebugUtil.h>
#include <global_data/GlobalData.h>


namespace cz {

// Forward
//
class BulletQueue;

class Bullet
{
    // We will define the range of the shoot entity.
    //
    static const float MIN_BULLET_SIZE;
    static const float MAX_BULLET_SIZE;

public:
    inline Bullet();
    inline ~Bullet();

    // @brief Set bullet properties.
    // @param vel       The velocity of the bullet
    // @param power     The power of the bullet (how much damage it will create).
    //
    inline void
    setProperties(float vel, float power);
    inline float
    velocity(void) const;
    inline float
    power(void) const;

    // @brief Set / get the parent BulletQueue to be used
    // @param queue     The queue to be used
    //
    inline void
    setQueue(BulletQueue* queue);
    inline BulletQueue*
    queue(void);
    inline const BulletQueue*
    queue(void) const;

    // @brief Create the bullet with the information we need. The bullet will be
    //        invisible
    //
    void
    create(void);

    // @brief Configure the initial position and direction of the bullet
    //        This method should be called before using this instance. This will
    //        show the bullet in the scene in the right position / direction.
    // @param pos       The initial position
    // @param dir       The initial direction (normalized)
    //
    void
    configure(const Ogre::Vector3& pos, const Ogre::Vector3& dir);

    // @brief Return the current position of the bullet
    // @returns the current position of the bullet
    //
    inline const Ogre::Vector3&
    currentPosition(void) const;

    // @brief Get the current direction direction.
    // @returns the direction reference of the bullet
    //
    inline const Ogre::Vector3&
    direction(void) const;

    // @brief Update the current position of the bullet after moving it over a
    //        certain time. The new position of the bullet will be obtained
    //        from currentPosition();
    // @param time      The time we want to move the bullet (time frame).
    //
    void
    update(float time);

    // @brief This method will be called when we don't use this instance anymore
    //        and we want to put it again in the queue (this method will put the
    //        bullet in the associated queue again).
    //
    void
    disable(void);

protected:
    Ogre::Vector3 mPosition;
    Ogre::Vector3 mNormalizedDir;
    Ogre::ManualObject* mEntity;
    Ogre::SceneNode* mNode;
    BulletQueue* mQueue;
    float mPower;
    float mVelocity;
};



////////////////////////////////////////////////////////////////////////////////
// inline stuff
//

inline Bullet::Bullet() :
    mEntity(0)
,   mNode(0)
,   mQueue(0)
,   mPower(1.f)
,   mVelocity(50.f)
{
}

inline Bullet::~Bullet()
{
    Ogre::SceneManager* scnMngr = GlobalData::sceneMngr;
    if(mNode){
        mNode->detachAllObjects();
        if(mNode->getParentSceneNode()){
            mNode->getParentSceneNode()->removeChild(mNode);
        }
        ASSERT(mEntity);
        scnMngr->destroySceneNode(mNode);
        scnMngr->destroyManualObject(mEntity);
    }
}

inline void
Bullet::setProperties(float vel, float power)
{
    ASSERT(vel > 0.f);
    ASSERT(power > 0.f);

    mVelocity = vel;
    mPower = power;
}

inline float
Bullet::velocity(void) const
{
    return mVelocity;
}
inline float
Bullet::power(void) const
{
    return mPower;
}

inline void
Bullet::setQueue(BulletQueue* queue)
{
    mQueue = queue;
}
inline BulletQueue*
Bullet::queue(void)
{
    return mQueue;
}
inline const BulletQueue*
Bullet::queue(void) const
{
    return mQueue;
}

inline const Ogre::Vector3&
Bullet::currentPosition(void) const
{
    return mPosition;
}

inline const Ogre::Vector3&
Bullet::direction(void) const
{
    return mNormalizedDir;
}

} /* namespace cz */
#endif /* BULLET_H_ */
