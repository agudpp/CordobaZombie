/*
 * SelectionHelper.cpp
 *
 *  Created on: Jun 30, 2013
 *      Author: agustin
 */

#include "SelectionHelper.h"

#include <OgreUserObjectBindings.h>

#include <types/basics.h>
#include <input/InputMouse.h>

namespace tool {

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

SelectionHelper::SelectionHelper(Ogre::SceneManager& sm,
                                 Ogre::Camera& camera,
                                 const ui::MouseCursor& mouseCursor) :
    mSceneMngr(sm)
,   mCamera(camera)
,   mMouseCursor(mouseCursor)
,   mRaySceneQuery(0)
{
    mRaySceneQuery = mSceneMngr.createRayQuery(mMouseRay,0);
}

SelectionHelper::~SelectionHelper()
{
    mSceneMngr.destroyQuery(mRaySceneQuery);
}

////////////////////////////////////////////////////////////////////////////////
void
SelectionHelper::setBaseIntersectionPlane(const Ogre::Plane& plane)
{
    mPlane = plane;
}

////////////////////////////////////////////////////////////////////////////////
bool
SelectionHelper::getPlaneIntersection(Ogre::Vector3& intPos)
{
    // set up the ray
    mMouseRay = mCamera.getCameraToViewportRay(mMouseCursor.getXRelativePos(),
                                               mMouseCursor.getYRelativePos());

    // check if the ray intersects our plane
    // intersects() will return whether it intersects or not (the bool value) and
    // what distance (the Real value) along the ray the intersection is
    std::pair<bool, Ogre::Real> result = mMouseRay.intersects(mPlane);

    if (result.first)
    {
        // if the ray intersect the plane, we have a distance value
        // telling us how far from the ray origin the intersection occurred.
        // the last thing we need is the point of the intersection.
        // Ray provides us getPoint() function which returns a point
        // along the ray, supplying it with a distance value.

        // get the point where the intersection is
        intPos = mMouseRay.getPoint(result.second);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
bool
SelectionHelper::getFirstRaycasted(Ogre::RaySceneQueryResultEntry &result,
                                   Ogre::uint32 mask)
{
    ASSERT(mRaySceneQuery);

    // configure the ray
    mMouseRay = mCamera.getCameraToViewportRay(mMouseCursor.getXRelativePos(),
                                               mMouseCursor.getYRelativePos());
    mRaySceneQuery->setRay(mMouseRay);
    mRaySceneQuery->setQueryMask(mask); // all
    mRaySceneQuery->setSortByDistance(true, 1); // the first one

    // execute the ray and get the results
    Ogre::RaySceneQueryResult& queryResult = mRaySceneQuery->execute();
    if (queryResult.empty()) {
        return false;
    }
    result = queryResult.back();

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
SelectionHelper::performRaycast(const Ogre::Vector3& pos1,
                                const Ogre::Vector3& pos2,
                                Ogre::RaySceneQueryResultEntry &result,
                                Ogre::uint32 mask)
{
    ASSERT(mRaySceneQuery);

    // configure the ray
    mMouseRay.setOrigin(pos1);
    mMouseRay.setDirection(pos2 - pos1);
    mRaySceneQuery->setRay(mMouseRay);
    mRaySceneQuery->setQueryMask(mask);
    //mRaySceneQuery->setQueryTypeMask(mask);
    mRaySceneQuery->setSortByDistance(true, 1); // the first one
    mRaySceneQuery->clearResults();

    // execute the ray and get the results
    Ogre::RaySceneQueryResult& queryResult = mRaySceneQuery->execute();
    if (queryResult.empty()) {
        return false;
    }
    result = queryResult.back();
    return true;
}

} /* namespace tool */
