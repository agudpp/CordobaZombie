/*
 * BulletObject.h
 *
 *  Created on: Sep 19, 2013
 *      Author: agustin
 */

#ifndef BULLETOBJECT_H_
#define BULLETOBJECT_H_

#include <OgreSceneNode.h>
#include <OgreEntity.h>

#include <bullet/LinearMath/btMotionState.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <debug/DebugUtil.h>

#include "BulletUtils.h"

namespace physics {

////////////////////////////////////////////////////////////////////////////////
// @brief BulletMotionState will represent the motion state used to sync scene nodes
//        with the world transformation of the bullet physics world
//

class BulletMotionState : public btMotionState
{
public:
    // constructor with the scene node associated
    BulletMotionState(Ogre::SceneNode* node = 0) : mNode(node), mDirty(false) {};
    virtual
    ~BulletMotionState(){};

    // @brief Set the associated scene node to be used.
    // @param node  The node
    //
    inline void
    setNode(Ogre::SceneNode* node) { mNode = node; }
    inline Ogre::SceneNode*
    node(void) { return mNode; }

    // @brief Used by bullet
    //
    virtual void
    getWorldTransform(btTransform &worldTrans) const
    {
        worldTrans = mTransform;
    }

    // @brief set the world transform
    //
    virtual void
    setWorldTransform(const btTransform &worldTrans)
    {
        mDirty = true;
        mTransform = worldTrans;
        if (mNode) {
            const btQuaternion& rot = worldTrans.getRotation();
            mNode->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
            const btVector3& pos = worldTrans.getOrigin();
            mNode->setPosition(pos.x(), pos.y(), pos.z());
        }
    }

    // @brief Return the current transform
    //
    inline btTransform&
    transform(void) { return mTransform; }

    // @brief check / set if this object is dirty or not
    //
    inline bool
    isDirty(void) const { return mDirty; }
    inline void
    setDirty(bool dirty) { mDirty = dirty; }


private:
    btTransform mTransform;
    Ogre::SceneNode* mNode;
    bool mDirty;
};

////////////////////////////////////////////////////////////////////////////////

// @brief We will define the Bullet object as if it were a plain structure with
//        some information.
//        Note that we will not allocate nor remove any memory here, just
//        contain the information.

struct BulletObject {
    btRigidBody* rigidBody;
    btCollisionShape* shape;
    BulletMotionState motionState;
    Ogre::Entity* entity;

    BulletObject(btRigidBody* rb, btCollisionShape* s, Ogre::Entity* ent) :
        rigidBody(rb)
    ,   shape(s)
    ,   entity(ent)
    {}

    BulletObject() :
        rigidBody(0)
    ,   shape(0)
    ,   entity(0)
    {}

    // @brief Comparator operator
    // @param other     The other BulletObject to compare against
    //
    inline bool
    operator==(const BulletObject& other) const;

    // @brief Helper method used to apply a force to the rigid body
    // @param force     The force we want to apply
    //
    inline void
    applyCentralForce(const Ogre::Vector3& force);
};











////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline bool
BulletObject::operator==(const BulletObject& other) const
{
    return rigidBody == other.rigidBody && shape == other.shape &&
        entity == other.entity;
}

inline void
BulletObject::applyCentralForce(const Ogre::Vector3& force)
{
    ASSERT(rigidBody);
    rigidBody->applyCentralForce(BulletUtils::ogreToBullet(force));
}

} /* namespace physics */
#endif /* BULLETOBJECT_H_ */
