/*
 * BulletDebugDrawer.h
 *
 *  Created on: Jan 8, 2014
 *      Author: agustin
 */

#ifndef BULLETDEBUGDRAWER_H_
#define BULLETDEBUGDRAWER_H_

#include <OgreSceneManager.h>
#include <OgreFrameListener.h>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include "BulletUtils.h"

namespace physics {

class BulletDebugDrawer : public btIDebugDraw , public Ogre::FrameListener
{
public:
    BulletDebugDrawer(Ogre::SceneManager* scnMngr,
                      btDynamicsWorld* dw);
    virtual
    ~BulletDebugDrawer();

    // @brief Set enable / disable this debug drawer
    //
    void
    setEnable(bool enabled);

    virtual void
    drawLine(const btVector3 &from,
             const btVector3 &to,
             const btVector3 &color);
    virtual void
    drawTriangle(const btVector3 &v0,
                 const btVector3 &v1,
                 const btVector3 &v2,
                 const btVector3 &color,
                 btScalar);
    virtual void
    drawContactPoint(const btVector3 &PointOnB,
                     const btVector3 &normalOnB,
                     btScalar distance,
                     int lifeTime,
                     const btVector3 &color);
    virtual void
    reportErrorWarning(const char *warningString);
    virtual void
    draw3dText(const btVector3 &location, const char *textString);
    virtual void
    setDebugMode(int debugMode);
    virtual int
    getDebugMode() const;
protected:
    bool
    frameStarted(const Ogre::FrameEvent& evt);
    bool
    frameEnded(const Ogre::FrameEvent& evt);
private:
    struct ContactPoint
    {
        Ogre::Vector3 from;
        Ogre::Vector3 to;
        Ogre::ColourValue color;
        unsigned int dieTime;
    };
    DebugDrawModes mDebugModes;
    Ogre::ManualObject *mLines;
    Ogre::ManualObject *mTriangles;
    std::vector<ContactPoint> *mContactPoints;
    std::vector<ContactPoint> mContactPoints1;
    std::vector<ContactPoint> mContactPoints2;
    btDynamicsWorld* mDynamicWorld;
};

} /* namespace physics */
#endif /* BULLETDEBUGDRAWER_H_ */
