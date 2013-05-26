/*
 * RaycastManager.cpp
 *
 *  Created on: 20/03/2012
 *      Author: agustin
 *
 */

#include "DebugUtil.h"
#include "RaycastManager.h"


RaycastManager::RaycastManager() :
mCollMngr(0),
mRaySceneQuery(0)
{
    mRaySceneQuery = GLOBAL_SCN_MNGR->createRayQuery(mMouseRay,~0);
}

RaycastManager::~RaycastManager()
{
	GLOBAL_SCN_MNGR->destroyQuery(mRaySceneQuery);
}

/**
 * Set the collision manager to use.
 */
void
RaycastManager::setCollisionManager(CollisionManager *cm)
{
	ASSERT(cm);
	mCollMngr = cm;
}

/**
 * This function get the size of the collision manager and creates a plane
 * used to get the "intersection of the mouse raycast. This plane will have
 * the same size of the CollisionManager.
 * @param	yHeight		This is the height where the plane will be putted.
 */
void
RaycastManager::build(float y)
{
	ASSERT(mCollMngr);

	// create the plane
	mPlane.redefine(Ogre::Vector3::UNIT_Y, Ogre::Vector3(0,y,0));
}


/**
 * Perform a raycast using the actual relative position of the mouse.
 * Get all the objects using the collision manager and returns that ones
 * that are seted as userDefined (casting it automatically).
 * @param	xrm		X relative mouse position
 * @param	yrm		Y relative mouse position
 * @param	mask	The mask to be used.
 * @param	result	The result
 */
template<typename T>
void
RaycastManager::rayCast(float xrm, float yrm,
                        mask_t mask,
                        std::vector<T> &result) const
{
	ASSERT(mCollMngr);

	math::Vector2 p;

	// get the point in the plane
	getPoint(xrm, yrm, p);

	// now get all the
	mCollMngr->getCollisionObjects(p,mask, mResult);

	result.clear();
	for(int i = mResult.size()-1; i >= 0; --i) {
		ASSERT(mResult[i]->userDefined);
		result.push_back(static_cast<T>(mResult[i]->userDefined));
	}

}

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
T
RaycastManager::rayCast(float xrm, float yrm, mask_t mask) const
{
	ASSERT(mCollMngr);

	math::Vector2 p;

	// get the point in the plane
	getPoint(xrm, yrm, p);

	// now get all the
	mCollMngr->getCollisionObjects(p,mask, mResult);

	int index = 0;
	float min = 99999999.9f;


	if(mResult.empty()){
		return 0;
	} else {
		index = mResult.size()-1;
		min = mResult[index]->getPosition().squaredDistance(p);
	}

	float aux;

	for(int i = index-1; i >= 0; --i) {
		aux = mResult[i]->getPosition().squaredDistance(p);
		if(aux < min){
			index = i;
			min = aux;
		}
	}

	ASSERT(index >= 0);
	ASSERT(mResult[index]->userDefined);

	return static_cast<T>(mResult[index]->userDefined);
}


/**
 * Performs a Ogre Raycast from a position of the mouse and the camera.
 * @param	xrm		X relative mouse position
 * @param	yrm		Y relative mouse position
 * @param	mask	The mask to be used
 * @param	sort	If we want to sort the results by distance
 * @param	numR	Number of results we want to obtain
 * @return	All the objects obtained
 */
Ogre::RaySceneQueryResult &
RaycastManager::performOgreRay(float xrm, float yrm,
		                       uint32 mask,
		                       bool sort,
		                       int numR) const
{
	ASSERT(mRaySceneQuery);
	mMouseRay = GLOBAL_CAMERA->getCameraToViewportRay(xrm, yrm);
	mRaySceneQuery->setRay(mMouseRay);
	mRaySceneQuery->setQueryMask(mask);
	mRaySceneQuery->setSortByDistance(sort, numR);

	return mRaySceneQuery->execute();
}
