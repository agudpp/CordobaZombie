/*
 * SelectionHelper.h
 *
 *  Created on: Jun 30, 2013
 *      Author: agustin
 */

#ifndef SELECTIONHELPER_H_
#define SELECTIONHELPER_H_

#include <vector>

#include <OgreVector2.h>
#include <OgreSceneNode.h>
#include <OgreRay.h>
#include <OgrePlane.h>
#include <OgreSceneQuery.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>

#include <debug/DebugUtil.h>
#include <cursor/MouseCursor.h>


namespace tool {

class SelectionHelper
{
public:
    // @brief We need the SceneManager reference that we will use and also the
    //        camera used to perform the raycasts as well as the MouseCursor
    //
    SelectionHelper(Ogre::SceneManager& sm,
                    Ogre::Camera& camera,
                    const ui::MouseCursor& mouseCursor);
    ~SelectionHelper();

    // @brief Configure the plane for which we want to use as the base for
    //        intersections to create objects or positionate them.
    // @param plane     The plane we will use
    //
    void
    setBaseIntersectionPlane(const Ogre::Plane& plane);

    // @brief Get the interesction point in the plane for a given raycast using
    //        the current position of the mouse cursor
    // @param intPos    The intersection position from the raycast into the plane
    // @return true if we get an intersection and intPos contains the intersection
    //         point. False if we didn't intersect the plane.
    //
    bool
    getPlaneIntersection(Ogre::Vector3& intPos);

    // @brief Pick the first object in the raycast using the current mouse cursor
    //        position
    // @param result    the resulting entry if we hit something
    // @param queryMask The mask to be used in the query.
    // @returns true if we hit something (result), false otherwise
    //
    bool
    getFirstRaycasted(Ogre::RaySceneQueryResultEntry &result, Ogre::uint32 mask = ~0);

    // @brief Perform a raycast between 2 different positions and return the
    //        first object that intersects the ray (starting from pos1 and going
    //        to pos2).
    // @param pos1      The starting position of the ray
    // @param pos2      The last position of the ray
    // @param result    the resulting entry if we hit something
    // @param mask      The mask to be used
    // @returns true if we hit something (result), false otherwise
    //
    bool
    performRaycast(const Ogre::Vector3& pos1,
                   const Ogre::Vector3& pos2,
                   Ogre::RaySceneQueryResultEntry &result,
                   Ogre::uint32 mask = ~0);

private:
    Ogre::SceneManager& mSceneMngr;
    Ogre::Camera& mCamera;
    const ui::MouseCursor& mMouseCursor;
    Ogre::Plane mPlane;
    Ogre::Ray mMouseRay;
    Ogre::RaySceneQuery *mRaySceneQuery;
};


} /* namespace tool */
#endif /* SELECTIONHELPER_H_ */
