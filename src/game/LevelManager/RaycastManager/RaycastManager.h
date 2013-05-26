/*
 * RaycastManager.h
 *
 *  Created on: 20/03/2012
 *      Author: agustin
 *
 */

#ifndef RAYCASTMANAGER_H_
#define RAYCASTMANAGER_H_

#include <OgreRay.h>
#include <OgrePlane.h>
#include <OgreSceneQuery.h>

#include <vector>

#include "CollisionManager.h"
#include "CommonMath.h"
#include "GlobalObjects.h"

class RaycastManager {
public:
	RaycastManager();
	~RaycastManager();

	/**
	 * Set the collision manager to use.
	 */
	void setCollisionManager(CollisionManager *cm);

	/**
	 * This function get the size of the collision manager and creates a plane
	 * used to get the "intersection of the mouse raycast. This plane will have
	 * the same size of the CollisionManager.
	 * @param	yHeight		This is the height where the plane will be putted.
	 */
	void build(float y);


	/**
	 * Perform a raycast using the actual relative position of the mouse.
	 * Get all the objects using the collision manager and returns that ones
	 * that are set as userDefined (casting it automatically).
	 * @param	xrm		X relative mouse position
	 * @param	yrm		Y relative mouse position
	 * @param	mask	The mask to be used.
	 * @param	result	The result
	 */
	template<typename T>
	void rayCast(float xrm, float yrm, mask_t mask, std::vector<T> &result) const;

	/**
	 * Perform a raycast using the actual relative position of the mouse.
	 * Returns only the closest object to that point.
	 *
	 * @param	xrm		X relative mouse position
	 * @param	yrm		Y relative mouse position
	 * @param	mask	The mask to be used.
	 *
	 * @return	The object picked or 0 if no object was found
	 */
	template<typename T>
	T rayCast(float xrm, float yrm, mask_t mask) const;

	/**
	 * Perform a raycast using the actual relative position of the mouse.
	 * @param	xrm		X relative mouse position
	 * @param	yrm		Y relative mouse position
	 * @param	result	The point associated to that position
	 */
	inline void getPoint(float xrm, float yrm, math::Vector2 &result) const;
	inline void getPoint(float xrm, float yrm, Ogre::Vector3 &result) const;

	/**
	 * Performs a Ogre Raycast from a position of the mouse and the camera.
	 * @param	xrm		X relative mouse position
	 * @param	yrm		Y relative mouse position
	 * @param	mask	The mask to be used
	 * @param	sort	If we want to sort the results by distance
	 * @param	numR	Number of results we want to obtain
	 * @return	All the objects obtained
	 */
	Ogre::RaySceneQueryResult &performOgreRay(float xrm,
	                                          float yrm,
	                                          uint32 mask,
	                                          bool sort = true,
	                                          int numR = 1) const;


private:
	/**
	 * Returns the point associated to the plane
	 */
	inline void getPlanePoint(float xrm, float yrm, math::Vector2 &result) const;


private:
	Ogre::Plane mPlane;
	CollisionManager *mCollMngr;
	mutable Ogre::Ray mMouseRay;
	mutable CollisionResult mResult; // cache
	Ogre::RaySceneQuery *mRaySceneQuery;

};




inline void
RaycastManager::getPlanePoint(float xrm, float yrm, math::Vector2 &r) const
{
	// set up the ray
    mMouseRay = GLOBAL_CAMERA->getCameraToViewportRay(xrm, yrm);

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
		Ogre::Vector3 v = mMouseRay.getPoint(result.second);
		r.x = v.x;
		r.y = v.z;
	}
}

inline void
RaycastManager::getPoint(float xrm, float yrm, math::Vector2 &result) const
{
    return getPoint(xrm, yrm, result);
}
inline void
RaycastManager::getPoint(float xrm, float yrm, Ogre::Vector3 &r) const
{
    // set up the ray
    mMouseRay = GLOBAL_CAMERA->getCameraToViewportRay(xrm, yrm);

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
        r = mMouseRay.getPoint(result.second);
    }
}


#endif /* RAYCASTMANAGER_H_ */
