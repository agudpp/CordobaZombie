/*
 * IInputState.h
 *
 *  Created on: 04/08/2012
 *      Author: agustin
 *
 */

#ifndef IINPUTSTATE_H_
#define IINPUTSTATE_H_

#include <boost/shared_ptr.hpp>

#include <OgreSceneQuery.h>
#include <OgreVector3.h>

#include <Common/DebugUtil/DebugUtil.h>
#include <Common/GlobalObjects/GlobalObjects.h>
#include <CollisionSystem/CollisionTypedefs.h>
#include <CollisionSystem/CollisionManager.h>
#include <Utils/MouseCursor/MouseCursor.h>
#include <LevelManager/LevelManager.h>
#include <SelectionSystem/SelectableObject.h>

#include "InputManager.h"


// forward declarations
//
namespace selection {
class SelectionManager;
class SelectionData;
}

namespace input {

class IInputState {

	enum UnitType {
		NONE		= 0,
		ZOMBIE_UNIT = 1,
		PLAYER_UNIT = 2,
		CIVIL_UNIT 	= 4,
	};

public:
	IInputState();
	virtual ~IInputState();

	/**
	 * Configure the Static instances needed for work properly
	 */
	static void
	setLevelManager(LevelManager *im);

	/**
	 * @brief Configure the State using the current selection data
	 * @param selData   The current selection data
	 */
	virtual void
	configure(const selection::SelectionData &selData) = 0;

    /**
     * @brief Perform the needed raycast depending on the last selection.
     *        This function will show all the neccessary stuff in the level,
     *        select whatever is needed, and so forth.
     *        We will use the GLOBAL_CURSOR position to generate the RayCast
     */
    virtual void
    executeRayCast(void) = 0;

    /**
     * @brief Returns the last raycasted object
     * @returns the last raycasted object for any of the InputStates
     */
    inline selection::SelectableObject *
    lastRaycastedObj(void);

    /**
     * @brief Reset to 0 the last raycasted object
     */
    inline void
    resetLastRaycastedObj(void);

	//			Auxiliary functions to be used by the states.

    /**
     * @brief Get the point in the plane of the level when performing the
     *        raycast (using the position of the mouse).
     * @returns intersection of the raycast in the plane level.
     */
    inline void
    performRayAgainstPlane(Ogre::Vector3 &result) const;

	/**
	 * Performs a RaySceneQuery using the actual position of the mouse.
	 * @param	mask	The mask to be used (entities filter)
	 * @param	sort	If we want to sort the results (by distance, closest first)
	 * @param	numR	The number of objects that we want to obtain
	 */
    inline Ogre::RaySceneQueryResult &
    performRayQuery(uint32 mask, bool sort = true, int numR = 1) const;

    /**
     * Perform a CollisionPoint query (this query is obtained by the own
     * CollisionSystem).
     * @param	point	The point where we want to realize the Query
     * @param	mask	The mask to be used
     */
    inline CollisionResult &
    performCollPointQuery(const Ogre::Vector3 &point, mask_t mask) const;

    /**
     * Check if a point is in the Pathfinding "mesh" or is a place where the
     * units can move.
     * @param	pos		The position that we want to check (only x and z values)
     */
    inline bool
    isPointInPath(const Ogre::Vector3 &pos) const;
    inline bool
    isPointInPath(const sm::Vector2 &pos) const;

    /**
     * Returns the bit map of types of unit selected
     */
    UnitType
    getUnitSelectedType(void);

    /**
     * @brief Function that need to be called every time a raycast is made and
     *        we want to call the event MouseOver to the raycasted object.
     *        We will keep the file until another call to this function where
     *        we will check if the new raycasted object is the same than before,
     *        it it is not, then we will advise the other object that he is not
     *        OnMouseOver anymore
     * @param selObject     The object that is being pointed by the mouse
     *                      If selObject is 0 then we call the MouseOverExit
     *                      to the last object
     */
    inline void
    newRaycastedObject(selection::SelectableObject *selObject);

    /**
     * @brief Auxiliary function to get the first SelectableObject associated
     *        to a movable object that has the UserAny (that is the SelectableObj).
     *        If no movable object is raycasted or the movable hasn't any UserAny
     *        then 0 is returned, otherwise the Selectable object is returned
     * @param RAYCAST_MASK  The mask to be used to execute the raycast
     * @returns selObject   The closest selectableObject raycasted, or 0 if not
     */
    inline selection::SelectableObject *
    getFirstRaycastedObj(uint32 RAYCAST_MASK);

protected:


	static selection::SelectionManager *sSelectionMngr;
	static LevelManager *sLevelManager;
	static CollisionResult mCollObjs;
	static selection::SelectableObject *sOnMouseOverObj;

};

// typedef the shared ptr of this
//
typedef boost::shared_ptr<IInputState> IInputStatePtr;


////////////////////////////////////////////////////////////////////////////////
inline void
IInputState::performRayAgainstPlane(Ogre::Vector3 &result) const
{
    const MouseCursor *mc = GLOBAL_CURSOR;
    sLevelManager->getRaycastManger()->getPoint(mc->getXRelativePos(),
                                                mc->getYRelativePos(),
                                                result);
}

////////////////////////////////////////////////////////////////////////////////
inline Ogre::RaySceneQueryResult &
IInputState::performRayQuery(uint32 mask, bool sort, int numR) const
{
	const MouseCursor *mc = GLOBAL_CURSOR;
	return sLevelManager->getRaycastManger()->performOgreRay(
			mc->getXRelativePos(), mc->getYRelativePos(), mask, sort, numR);
}

////////////////////////////////////////////////////////////////////////////////
inline CollisionResult &
IInputState::performCollPointQuery(const Ogre::Vector3 &point,mask_t mask) const
{
	sLevelManager->getCollisionManager()->getCollisionObjects(
				sm::Vector2(point.x, point.z), mask, mCollObjs);
	return mCollObjs;
}

////////////////////////////////////////////////////////////////////////////////
inline bool
IInputState::isPointInPath(const Ogre::Vector3 &pos) const
{
	return sLevelManager->getPathfinderManager()->getNodeFromPoint(
			sm::Vector2(pos.x, pos.z)) != 0;
}
////////////////////////////////////////////////////////////////////////////////
inline bool
IInputState::isPointInPath(const sm::Vector2 &pos) const
{
	return sLevelManager->getPathfinderManager()->getNodeFromPoint(pos) != 0;
}

////////////////////////////////////////////////////////////////////////////////
inline void
IInputState::newRaycastedObject(selection::SelectableObject *selObject)
{
    if (sOnMouseOverObj == selObject) {
        return;
    }
    if (sOnMouseOverObj != 0) {
        sOnMouseOverObj->mouseExitObject();
    }
    sOnMouseOverObj = selObject;
    if (sOnMouseOverObj != 0){
        sOnMouseOverObj->mouseOverObject();
    }
}

inline selection::SelectableObject *
IInputState::lastRaycastedObj(void)
{
    return sOnMouseOverObj;
}
inline void
IInputState::resetLastRaycastedObj(void)
{
    sOnMouseOverObj = 0;
}

inline selection::SelectableObject *
IInputState::getFirstRaycastedObj(uint32 RAYCAST_MASK)
{
    // We execute the Ogre raycast instead of getting the position in the
    // plane where the player will be moving.
    // We only want the first object that is closer to the camera over the ray
    Ogre::RaySceneQueryResult &results = performRayQuery(RAYCAST_MASK);
    if (results.empty()){
        // no results found
        return 0;
    }

    // else there are new raycasted objects, we want only the first one
    Ogre::RaySceneQueryResultEntry &obj = results.front();
    if (obj.movable == 0) {
        // no movable object (probably a world fragment?.. this shouldn't
        // be happening
        debugWARNING("Probably we had wrongly set the RAYCAST_FLAGS?\n");
        return 0;
    }

    // check if we have something in the user any
    const Ogre::Any &anyObj = obj.movable->getUserAny();
    if (anyObj.isEmpty()) {
        // no user defined
        return 0;
    }

    // advise the object that we are over him with the mouse
    // Thanks to Ogre we have to do a any_cast that is a string comparison!!!!
    // so nice! so efficient
    return Ogre::any_cast<selection::SelectableObject*>(anyObj);
}

}

#endif /* IINPUTSTATE_H_ */
