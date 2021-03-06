/*
 * LevelManager.h
 *
 *  Created on: 26/02/2012
 *      Author: agustin
 *
 */

#ifndef LEVELMANAGER_H_
#define LEVELMANAGER_H_

#ifdef DEBUG
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#endif

#include "PathfindingManager.h"
#include "CollisionManager.h"
#include "LineOfSightManager.h"
#include "RaycastManager.h"

// Define the LineOfSightManager size here (once we have found the "better" size)
// 50 x 50 give use 781256 bytes = 781 kb... probably to much?
typedef LineOfSightManager<50,50>	LOSManager;

#include "DebugUtil.h"

class TiXmlElement;

class LevelManager {
public:
	/**
	 * Automatically sets to the GlobalObjects
	 */
	LevelManager();
	~LevelManager();

	/**
	 * Returns the PathFindingMngrInstance
	 * TODO: transformarlo a const const
	 */
	inline PathfindingManager *getPathfinderManager();

	/**
	 * Returns the CollisionManager instance
	 */
	inline CollisionManager *getCollisionManager();

	/**
	 * Returns the LineOfSightManager...
	 */
	inline LOSManager *getLOSManager(void);
	inline const LOSManager *getLOSManager(void) const;

	/**
	 * Returns the RaycastManager
	 */
	inline RaycastManager *getRaycastManger(void);

	/**
	 * Get the level size (collision level size or the x,z plane level size)
	 */
	inline void getLevelSize(float &x, float &y) const;

	/**
	 * Clear the level
	 */
	void clear(void);

	/**
	 * Load a level from an xml
	 *
	 * UNDEFINED - DO NOT USE
	 */
	void load(const TiXmlElement *elem);


#ifdef DEBUG
	/**
	 * @param ssX	The space size X of the "world"
	 * @param ssY	The space size Y of the "world"
	 * @param cnX	The number of cells used in X
	 * @param cnY	The number of cells used in Y
	 */
	void load(const Ogre::Entity *levelMesh,
			cScalar ssX, cScalar ssY, int cnX, int cnY);

	/**
	 * Show all the triangles...
	 */
	void showTriangles(void);
#endif

private:
#ifdef DEBUG
	void drawTriangle(const Triangle *t);
#endif

private:
	PathfindingManager	*mPathfindingMngr;
	CollisionManager	*mCollMngr;
	LOSManager			mLOSManager;
	RaycastManager		mRaycastManager;
	PolyStructsContainer<sm::Vertex *> mVertexs;
	PolyStructsContainer<Triangle *> mTriangles;

};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
inline PathfindingManager *LevelManager::getPathfinderManager()
{
	return mPathfindingMngr;
}

////////////////////////////////////////////////////////////////////////////////
inline CollisionManager *LevelManager::getCollisionManager()
{
	return mCollMngr;
}

////////////////////////////////////////////////////////////////////////////////
inline LOSManager *LevelManager::getLOSManager(void)
{
	return &mLOSManager;
}
inline const LOSManager *LevelManager::getLOSManager(void) const
{
	return &mLOSManager;
}

////////////////////////////////////////////////////////////////////////////////
inline RaycastManager *LevelManager::getRaycastManger(void)
{
	return &mRaycastManager;
}

////////////////////////////////////////////////////////////////////////////////
inline void LevelManager::getLevelSize(float &x, float &y) const
{
	x = mCollMngr->getWorldXSize();
	y = mCollMngr->getWorldYSize();
}


#endif /* LEVELMANAGER_H_ */
