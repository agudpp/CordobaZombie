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


protected:
	//			Auxiliary functions to be used by the states.

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
    inline void newRaycastedObject(selection::SelectableObject *selObject);

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
			sm::Vector2(pos.x, pos.z));
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

}

#endif /* IINPUTSTATE_H_ */
