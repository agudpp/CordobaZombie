/*
 * BulletObject.h
 *
 *  Created on: Sep 12, 2013
 *      Author: agustin
 */

#ifndef BULLETOBJECT_H_
#define BULLETOBJECT_H_

#include <OgreSceneNode.h>

#include <bullet/LinearMath/btMotionState.h>
#include <bullet/btBulletDynamicsCommon.h>

namespace tests {

class BulletObject : public btMotionState
{
public:
    BulletObject(): mVisibleobj(0){};
    virtual
    ~BulletObject(){};

    void setNode(Ogre::SceneNode *node) {
            mVisibleobj = node;
        }

    virtual void getWorldTransform(btTransform &worldTrans) const {
        worldTrans = mPos1;
    }

    virtual void setWorldTransform(const btTransform &worldTrans) {
        if(NULL == mVisibleobj)
            return; // silently return before we set a node
        const btQuaternion& rot = worldTrans.getRotation();
        Ogre::Quaternion q(rot.w(), rot.x(), rot.y(), rot.z());
        if (q.isNaN()) {
            std::cout << ".";
            return;
        }
        mVisibleobj->setOrientation(q);
        const btVector3& pos = worldTrans.getOrigin();
        mVisibleobj->setPosition(pos.x(), pos.y(), pos.z());
        mPos1 = worldTrans;
    }

public:
    Ogre::SceneNode *mVisibleobj;
    btTransform mPos1;
    btRigidBody* mRigidBody;
};

} /* namespace tests */
#endif /* BULLETOBJECT_H_ */
