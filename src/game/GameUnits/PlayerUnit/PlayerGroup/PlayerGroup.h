/*
 * PlayerGroup.h
 *
 *  Created on: 04/03/2012
 *      Author: agustin
 */

#ifndef PLAYERGROUP_H_
#define PLAYERGROUP_H_


#include "UnitGroup.h"


class GameUnit;

class PlayerGroup : public UnitGroup
{
public:
	PlayerGroup();
	virtual ~PlayerGroup();

	 /** This function calculates a path for all the units calling the
	 * PathfinderManager using the GroupRadius and use the formation setted
	 * before to calculate the new path for all the units.
	 *
	 * @param	to	The position where we have to send all the group
	 */
	void moveGroupTo(const core::Vector2 &to);

	/**
	 * Plan some attack strategy..
	 */
	void planStrategy(GameUnit *target);
	void planStrategy(std::vector<GameUnit *> &targets);
};

#endif /* PLAYERGROUP_H_ */
