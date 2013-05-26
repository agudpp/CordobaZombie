/*
 * CircularFormation.h
 *
 *  Created on: 04/03/2012
 *      Author: agustin
 */

#ifndef CIRCULARFORMATION_H_
#define CIRCULARFORMATION_H_

#include "UnitGroupFormation.h"
#include "DebugUtil.h"

class GameUnit;

class CircularFormation : public UnitGroupFormation
{
public:
	CircularFormation();
	virtual ~CircularFormation();

	/**
	 * Preconfigure the formation with the info associate to be calculated
	 * @param	unitRadius		The radius of the unit that form the group
	 * @param	unitPos			The start position of the units, this order
	 * 							we will use to calculate all the position
	 * 							when the calculate() method is called
	 * @param	formRadius		Returns the that contain all the units in the
	 * 							formation
	 */
	void configure(float unitRadius,
			const std::vector<GameUnit *> &unitPos,
			float &formRadius);

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
	void calculate(const sm::Vector2 &actualPos,
			const sm::Vector2 &middlePos,
			const sm::Vector2 &endPos,
			std::vector<sm::Vector2> &result);
private:
	std::vector<sm::Vector2>	mPositions;
};

#endif /* CIRCULARFORMATION_H_ */
