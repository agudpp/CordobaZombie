/*
 * IInputState.h
 *
 *  Created on: 04/08/2012
 *      Author: agustin
 *
 */

#ifndef IINPUTSTATE_H_
#define IINPUTSTATE_H_

#include <OgreSceneQuery.h>
#include <OgreVector3.h>

#include "CollisionTypedefs.h"
#include "MouseCursor.h"
#include "LevelManager.h"
#include "InputManager.h"
#include "DebugUtil.h"
#include "CollisionManager.h"

class InputActionObject;


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
	static void setInpuManager(InputManager *im);
	static void setLevelManager(LevelManager *im);

	/**
	 * Enter the state and pass the action associated if we have one or 0 if
	 * not.
	 */
	virtual void enter(InputActionObject *iao = 0);

	/**
	 * Update the state
	 */
	virtual void update(void) = 0;

	/**
	 * Function called when we close this state to open another one.
	 */
	virtual void exti(void) = 0;

protected:
	//			Auxiliary functions to be used by the states.

	/**
	 * Performs a RaySceneQuery using the actual position of the mouse.
	 * @param	mask	The mask to be used (entities filter)
	 * @param	sort	If we want to sort the results (by distance, closest first)
	 * @param	numR	The number of objects that we want to obtain
	 */
	inline Ogre::RaySceneQueryResult &performRayQuery(uint32 mask, bool sort = true,
			int numR = 1);

	/**
	 * Perform a CollisionPoint query (this query is obtained by the own
	 * CollisionSystem).
	 * @param	point	The point where we want to realize the Query
	 * @param	mask	The mask to be used
	 */
	inline CollisionResult &performCollPointQuery(const Ogre::Vector3 &point,
			mask_t mask);

	/**
	 * Check if a point is in the Pathfinding "mesh" or is a place where the
	 * units can move.
	 * @param	pos		The position that we want to check (only x and z values)
	 */
	inline bool isPointInPath(const Ogre::Vector3 &pos);

	/**
	 * Returns the bit map of types of unit selected
	 */
	UnitType getUnitSelectedType(void);


protected:
	static InputManager		*mInputManager;
	static LevelManager		*mLevelManager;
	static CollisionResult	mCollObjs;

};


////////////////////////////////////////////////////////////////////////////////
inline Ogre::RaySceneQueryResult &IInputState::performRayQuery(uint32 mask,
		bool sort,	int numR)
{
	ASSERT(mLevelManager);
	ASSERT(mInputManager);
	MouseCursor *mc = mInputManager->getMouseCursor();
	return mLevelManager->getRaycastManger()->performOgreRay(
			mc->getXRelativePos(), mc->getYRelativePos(), mask, sort, numR);
}

////////////////////////////////////////////////////////////////////////////////
inline CollisionResult &IInputState::performCollPointQuery(
		const Ogre::Vector3 &point,mask_t mask)
{
	ASSERT(mLevelManager);
	mLevelManager->getCollisionManager()->getCollisionObjects(
				sm::Vector2(point.x, point.z), mask, mCollObjs);
	return mCollObjs;
}

////////////////////////////////////////////////////////////////////////////////
inline bool IInputState::isPointInPath(const Ogre::Vector3 &pos)
{
	ASSERT(mLevelManager);
	return mLevelManager->getPathfinderManager()->getNodeFromPoint(
			sm::Vector2(pos.x, pos.z));
}


#endif /* IINPUTSTATE_H_ */
