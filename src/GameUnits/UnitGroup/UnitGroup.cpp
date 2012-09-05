/*
 * UnitGroup.cpp
 *
 *  Created on: 04/03/2012
 *      Author: agustin
 */

#include "GameUnit.h"
#include "UnitGroup.h"
#ifdef DEBUG
#include "PrimitiveDrawer.h"
#endif

LevelManager		*UnitGroup::mLevelManager = 0;

/**
 * Function used to assign the path calculated to all the units in the
 * order that the mFormation return the positions to the mUnits.
 * It clear the path of all the units
 */
void UnitGroup::setPathToUnits(void)
{
	ASSERT(mPath.size() > 2);
	ASSERT(mFormation);
	std::vector<sm::Vector2> pathPositions;
	pathPositions.reserve(mUnits.size());

	const int usize = mUnits.size();
	const int psize = mPath.size() - 1;

	// first clear all the units paths and set the first position
	for(int i = 0; i < usize; ++i){
		mUnits[i]->getPath().clear();
		// set the actual position
		mFormation->calculate(mPath[0], mPath[0], mPath[1], pathPositions);
		ASSERT(pathPositions.size() == usize);
		for(int j = 0; j < usize; ++j){
			mUnits[j]->getPath().push_back(pathPositions[j]);
		}

	}
	for(int i = 1; i < psize; ++i){
		mFormation->calculate(mPath[i-1], mPath[i], mPath[i+1], pathPositions);
		ASSERT(pathPositions.size() == usize);
		for(int j = 0; j < usize; ++j){
			mUnits[j]->getPath().push_back(pathPositions[j]);
#ifdef DEBUG
		DRAWER.createNewVPoint(Ogre::Vector3(pathPositions[j].x, 4, pathPositions[j].y));
#endif
		}
	}

	// update the path of every unit
	for(int i = 0; i < usize; ++i){
		mUnits[i]->restartPath();
	}

}



////////////////////////////////////////////////////////////////////////////////
void UnitGroup::removeAllUnits(void)
{
	// advise the units that we are removing from it
	for(int i = mUnits.size()-1; i >= 0; --i){
		mUnits[i]->removedFromGroup(this);
	}
	mUnits.clear();
#ifdef DEBUG
		mBuilded = false;
#endif
}

////////////////////////////////////////////////////////////////////////////////
void UnitGroup::addUnit(GameUnit *unit)
{
	ASSERT(unit);
	ASSERT(std::find(mUnits.begin(), mUnits.end(), unit) == mUnits.end());
	mUnits.push_back(unit);
	unit->addedToGroup(this);
#ifdef DEBUG
		mBuilded = false;
#endif

}
////////////////////////////////////////////////////////////////////////////////
void UnitGroup::build(void)
{
	ASSERT(!mBuilded);
	ASSERT(mCreator);

	// configure the formation
	mGroupRadius = 0.0f;
	if(mFormation){
		// TODO: we will use the
		mFormation->configure(mCreator->getRadius(), mUnits, mGroupRadius);
	}
	ASSERT(mGroupRadius != 0.0f);

	// if we have attack strategy we configure it to
	if(mAttackStrategy){
		ASSERT(false);
		// mAttackStrategy
	}

#ifdef DEBUG
		mBuilded = true;
#endif

//	ASSERT(false);
}

