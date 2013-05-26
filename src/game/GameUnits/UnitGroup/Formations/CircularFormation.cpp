/*
 * CircularFormation.cpp
 *
 *  Created on: 04/03/2012
 *      Author: agustin
 */

#include "CircularFormation.h"

CircularFormation::CircularFormation()
{
}

CircularFormation::~CircularFormation()
{
}


/**
 * Preconfigure the formation with the info associate to be calculated
 * @param	unitRadius		The radius of the unit that form the group
 * @param	unitPos			The start position of the units, this order
 * 							we will use to calculate all the position
 * 							when the calculate() method is called
 * @param	formRadius		Returns the that contain all the units in the
 * 							formation
 */
void CircularFormation::configure(float unitRadius,
		const std::vector<GameUnit *> &unitPos,
		float &formRadius)
{
	ASSERT(unitPos.size() > 0);
	// we will do a very simple formation (we have to change this)
	// tODO: cambiar esto para que sea mejor
	mPositions.resize(unitPos.size());
	const int size = unitPos.size();
	ASSERT(mPositions.size() <= 4);
	mPositions[0].x = 1.0f; mPositions[0].y = 0.0f;
	mPositions[0].normalize();
	unitRadius += 2.0f;	// separate the units a little
	formRadius = 4.0f * unitRadius;	// two units side by side
	mPositions[0] *= unitRadius;
	for(int i = 1; i < size ; ++i){
		mPositions[i] = mPositions[i-1];
		mPositions[i].transformToNormal();
	}
}

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
void CircularFormation::calculate(const math::Vector2 &actualPos,
		const math::Vector2 &middlePos,
		const math::Vector2 &endPos,
		std::vector<math::Vector2> &result)
{
	// TODO: cambiar todo esto
	const int size = mPositions.size();
	result.clear();
	for(int i = 0; i < size; ++i){
		result.push_back(mPositions[i] + middlePos);
	}
}
