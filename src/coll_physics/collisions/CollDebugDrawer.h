/*
 * CollDebugDrawer.h
 *
 *  Created on: Jan 9, 2014
 *      Author: agustin
 */

#ifndef COLLDEBUGDRAWER_H_
#define COLLDEBUGDRAWER_H_

#include <OgreSceneManager.h>
#include <OgreFrameListener.h>

#include <OgreManualObject.h>

#include "CollObject.h"

namespace coll {

class CollDebugDrawer : public Ogre::FrameListener
{
public:
    CollDebugDrawer(Ogre::SceneManager* scm);
    ~CollDebugDrawer();

    // @brief Set the color for the lines
    //
    inline void
    setLineColour(const Ogre::ColourValue& colour) {mColour = colour;}

    // @brief Enabled/Disable the drawer
    //
    void
    setEnable(bool enabled);
    inline bool
    isEnabled(void) const {return mEnabled;}

    // @brief Draw a new CollObject
    //
    void
    drawCollObject(const CollObject* co);

protected:

    // @brief Inherited methods from Framelistener
    //        Here is where we will draw (call the update) method to
    //        redraw everything
    //
    bool
    frameStarted(const Ogre::FrameEvent& evt);
    bool
    frameEnded(const Ogre::FrameEvent& evt);

private:
    // @brief Different methods to draw each one of the Box2D shapes
    //
    void
    drawCircle(const CollObject* co);
    void
    drawEdge(const CollObject* co);
    void
    drawPolygon(const CollObject* co);
    void
    drawChain(const CollObject* co);
    void
    drawBoundingBox(const CollObject* co);

private:
    bool mEnabled;
    Ogre::SceneNode* mRootNode;
    Ogre::ManualObject* mLines;
    Ogre::ColourValue mColour;
};

} /* namespace coll */
#endif /* COLLDEBUGDRAWER_H_ */
