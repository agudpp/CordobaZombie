/*
 * Bomb.h
 *
 *  Created on: 30/04/2012
 *      Author: agustin
 */

#ifndef BOMB_H_
#define BOMB_H_

#include <OgreEntity.h>
#include <OgreSceneNode.h>

#include "UpdatableObject.h"
#include "CollisionManager.h"
#include "GlobalObjects.h"
#include "CommonMath.h"
#include "CollisionTypedefs.h"
#include "SoundAPI.h"
#include "SoundFamilyTable.h"

class Bomb : public UpdatableObject
{
public:
    enum
    {
        BOMB_NONE = 0, BOMB_TIME, BOMB_REMOTE, BOMB_PROXIMITY
    };

    // Bombs sound codes
    enum
    {
        SS_BIP_A_CODE, // Slow bip for time bombs, detonation bip for the rest
        SS_BIP_B_CODE, // Fast bip for time bombs, silence        for the rest
        SS_EXPLODE_CODE // Guess
    };

public:
    Bomb(int bombType = BOMB_NONE);
    virtual
    ~Bomb();

    /**
     * Get bomb type
     */
    inline int
    getType(void) const;

    /**
     * Set the entity
     */
    void
    setEntity(Ogre::Entity *ent);
    inline Ogre::Entity *
    getEntity(void);
    inline Ogre::SceneNode *
    getSceneNode(void);

    /**
     * Show/Hide
     */
    inline void
    show(void);
    inline void
    hide(void);

    /**
     * Set the power of the bomb
     */
    inline void
    setPower(float p);

    /**
     * Set the radius that this bomb affect
     */
    inline void
    setRadius(float r);

    /**
     * Set the position of the bomb
     */
    inline void
    setPosition(const Ogre::Vector3 &p);

    /**
     * Set the Collision Manager
     */
    static void
    setCollisionManager(c_p::CollisionManager *cm);

    /**
     * Function called before the object start to bee updated
     */
    virtual void
    beforeUpdate(void) = 0;

    /**
     * Function to be implemented.
     * This function is called every time the Manager is updated (every frame...)
     */
    virtual void
    update(void) = 0;

    /**
     * Function called when the object stop been updated
     */
    virtual void
    updateStopped(void) = 0;

protected:
    /**
     * Perform explosion
     * @param	sqrRadius	The squared radius that affect the bomb
     * @param	power		The power of the bomb (this will decrees when we are
     * 						far away from the "explosion point")
     */
    void
    performExplosion(float sqrRadius, float power);

private:
    /**
     * Check if there are some object in the middle between us and the target
     * @param p1	The position where the bomb is
     * @param t		The target
     * @return		True if there are an object between, false otherwise
     */
    bool
    checkObjectBetween(const core::Vector2 &p1, const CollisionObject *t);

protected:
    Ogre::Entity *mEntity;
    Ogre::SceneNode *mNode;
    float mPower;
    float mRadius;
    int mType;
    mm::SoundFamilyTable mSounds;
    mm::SoundAPI mSAPI;

    static c_p::CollisionManager *mCollMngr;
    static c_p::CollisionResult mCollResult;
};

inline int
Bomb::getType(void) const
{
    return mType;
}

inline Ogre::Entity *
Bomb::getEntity(void)
{
    return mEntity;
}
inline Ogre::SceneNode *
Bomb::getSceneNode(void)
{
    return mNode;
}

inline void
Bomb::show(void)
{
    ASSERT(mNode);
    if (mNode->getParentSceneNode())
        return;
    GLOBAL_SCN_MNGR->getRootSceneNode()->addChild(mNode);

}
inline void
Bomb::hide(void)
{
    if (!mNode->getParentSceneNode())
        return;
    mNode->getParentSceneNode()->removeChild(mNode);
}

/**
 * Set the power of the bomb
 */
inline void
Bomb::setPower(float p)
{
    mPower = p;
}

/**
 * Set the radius that this bomb affect
 */
inline void
Bomb::setRadius(float r)
{
    mRadius = r * r;
}

inline void
Bomb::setPosition(const Ogre::Vector3 &p)
{
    ASSERT(mNode);
    mNode->setPosition(p);
}

#endif /* BOMB_H_ */
