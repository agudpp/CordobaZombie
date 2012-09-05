/*
 * LoaderData.h
 *
 *  Created on: 16/06/2012
 *      Author: agustin
 */

#ifndef LOADERDATA_H_
#define LOADERDATA_H_

#include <vector>

// We will define all the info that we need to load all the resources /
// entities / etc. for the actual level

class GameObject;
class GameUnit;
class ZombieUnit;
class PlayerUnit;
class LoaderManager;
class UpdObjsManager;
class MenuManager;
class LevelManager;
class PathfindingManager;
class BillboardBatery;
class ZombieFactoryManager;
class CollisionManager;

typedef std::vector<GameObject *>			GameObjectVec;
typedef std::vector<GameUnit *>				GameUnitVec;
typedef std::vector<ZombieUnit *>			ZombieUnitVec;
typedef std::vector<PlayerUnit *>			PlayerUnitVec;

struct LoaderData {
	LoaderManager				*loaderManager;
	UpdObjsManager				*updatableManager;
	MenuManager					*menuManager;
	LevelManager				*levelManager;
	GameObjectVec				*gameObjects;
	GameUnitVec					*gameUnits;
	ZombieUnitVec				*zombieUnits;
	PlayerUnitVec				*playerUnits;
	PathfindingManager			*pathfindingManager;
	BillboardBatery				*bloodBatery;
	ZombieFactoryManager		*zombieFactoryManager;
	CollisionManager			*collisionManager;

	LoaderData() :
		loaderManager(0),
		updatableManager(0),
		menuManager(0),
		levelManager(0),
		gameObjects(0),
		gameUnits(0),
		zombieUnits(0),
		playerUnits(0),
		pathfindingManager(0),
		bloodBatery(0),
		zombieFactoryManager(0),
		collisionManager(0)
	{

	}

};

#endif /* LOADERDATA_H_ */
