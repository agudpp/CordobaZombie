/*
 * UnitGroupFormation.h
 *
 *  Created on: 03/03/2012
 *      Author: agustin
 */

#ifndef UNITGROUPFORMATION_H_
#define UNITGROUPFORMATION_H_

#include "CommonMath.h"
#include <vector>

class GameUnit;

class UnitGroupFormation
{
public:
	inline UnitGroupFormation(){};
	inline virtual ~UnitGroupFormation() {};

	/**
	 * Preconfigure the formation with the info associate to be calculated
	 * @param	unitRadius		The radius of the unit that form the group
	 * @param	units			The start position of the units, this order
	 * 							we will use to calculate all the position
	 * 							when the calculate() method is called
	 * @param	formRadius		Returns the that contain all the units in the
	 * 							formation
	 */
	virtual void configure(float unitRadius,
			const std::vector<GameUnit *> &unitPos,
			float &formRadius) = 0;

	/**
	 * Performe the calculation of the position of each of the units in the
	 * group to maintain the actual formation.
	 * @param	actualPos		The actual position
	 * @param	middlePos		The middle pos that we will calculate the pos
	 * @param	endPos			The end position.
	 * @param	result			The vector of points associated to each unit
	 * 							that have to go in the "middlePos"
	 * @note	We have to previously configure the formation
	 */
	virtual void calculate(const core::Vector2 &actualPos,
			const core::Vector2 &middlePos,
			const core::Vector2 &endPos,
			std::vector<core::Vector2> &result) = 0;
};

#endif /* UNITGROUPFORMATION_H_ */
