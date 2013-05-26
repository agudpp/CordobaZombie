/*
 * PlayerGroup.cpp
 *
 *  Created on: 04/03/2012
 *      Author: agustin
 */

#include "PlayerGroup.h"
#include "PlayerUnit.h"

PlayerGroup::PlayerGroup()
{

}

PlayerGroup::~PlayerGroup()
{
}

/** This function calculates a path for all the units calling the
 * PathfinderManager using the GroupRadius and use the formation setted
 * before to calculate the new path for all the units.
 *
 * @param	to	The position where we have to send all the group
 */
void PlayerGroup::moveGroupTo(const math::Vector2 &to)
{
	ASSERT(mCreator);
	// TODO: notar que tenemos que calcular los path de todas las unidades
	// no de una sola si estan muy alejadas o en particular si estan en
	// distintos triangulos y es mas dificil hacer que se unan que otra cosa
	// por el momento asumimos que estan todas las unidades en el mismo triangulo
	const int r = calculatePathWithRadius(mCreator->getPosition(),
			to, mGroupRadius);
	if(r == PathfindingManager::PATH_NOT_FOUND){
		debug("Path not found...\n");
		return;
	} else if(r == PathfindingManager::LINE_PATH){
		// work with line path, add twice the to vector and once the from vector
		// to use the setPathToUnits() method
		mPath.push_back(mCreator->getPosition());
		mPath.push_back(to);
		mPath.push_back(to);
		setPathToUnits();
	} else if(r == PathfindingManager::NORMAL_PATH){
		if(mPath.size() <= 2){
			mPath.push_back(to);
		}
		mPath.push_back(to);
		setPathToUnits();
	} else {
		debug("Error receive code %d\n", r);
		ASSERT(false);
	}

	// now advise all the units to move
	for(int i = mUnits.size()-1; i >= 0; --i) {
		mUnits[i]->newExternalEvent(PlayerUnit::E_MOVE_TO);
	}
}

/**
 * Plan some attack strategy..
 */
void PlayerGroup::planStrategy(GameUnit *target)
{
	ASSERT(false);
}
void PlayerGroup::planStrategy(std::vector<GameUnit *> &targets)
{
	ASSERT(false);
}
