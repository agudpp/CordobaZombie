/*
 * UnitGroup.h
 *
 *  Created on: 22/02/2012
 *      Author: agustin
 */

#ifndef UNITGROUP_H_
#define UNITGROUP_H_

#include <vector>

#ifdef DEBUG
#include <algorithm>
#endif

#include "DebugUtil.h"
#include "UnitGroupFormation.h"
#include "UnitAttackStrategy.h"
#include "LevelManager.h"

class GameUnit;

class UnitGroup
{
public:
	typedef std::vector<GameUnit *>	Container;
public:
	UnitGroup() :
		mCreator(0),
		mFormation(0),
		mAttackStrategy(0)
	{
		ASSERT(mLevelManager);
#ifdef DEBUG
		mBuilded = false;
#endif
	};

	virtual ~UnitGroup(){};

	/**
	 * Set/Get the creator of the group (the unit who will update the group
	 * logic)
	 */
	inline void setCreator(GameUnit *c);
	inline GameUnit *getCreator(void);

	/**
	 * Remove all the units from the group
	 */
	void removeAllUnits(void);

	/**
	 * Add a Unit to the group
	 */
	void addUnit(GameUnit *unit);

	/**
	 * Returns all the units
	 */
	inline Container &getAllUnits(void);

	/**
	 * Set the actual formation
	 */
	inline void setFormation(UnitGroupFormation *formation);

	/**
	 * This function calculates a path for all the units calling the
	 * PathfinderManager using the GroupRadius and use the formation setted
	 * before to calculate the new path for all the units.
	 *
	 * @param	to	The position where we have to send all the group
	 */
	virtual void moveGroupTo(const sm::Vector2 &to) = 0;

	/**
	 * Set the attack strategy if we have
	 */
	inline void setAttackStrategy(UnitAttackStrategy *as);

	/**
	 * Plan some attack strategy..
	 */
	virtual void planStrategy(GameUnit *target) = 0;
	virtual void planStrategy(std::vector<GameUnit *> &targets) = 0;

	/**
	 * This function must be called before use the logic mathods (moveGroupTo or
	 * planStrategy). This will configure all the things that they need.
	 */
	void build(void);

	/**
	 * Set the level manager used by this class
	 */
	static inline void setLevelManager(LevelManager	*lm);

protected:
	/**
	 * Calculate the path to a certain place using PathfinderManager and stores
	 * the result in the path member.
	 * @returns pfr		The PathfinderManager result.
	 */
	inline int calculatePathWithRadius(const sm::Vector2 &start,
			const sm::Vector2 &end,
			float radius);

	/**
	 * Function used to assign the path calculated to all the units in the
	 * order that the mFormation return the positions to the mUnits.
	 * It clear the path of all the units
	 */
	void setPathToUnits(void);


protected:
	GameUnit					*mCreator;
	Container					mUnits;
	UnitGroupFormation 			*mFormation;
	UnitAttackStrategy			*mAttackStrategy;
	float						mGroupRadius;
	PathfindingManager::Path 	mPath;

	static LevelManager			*mLevelManager;
#ifdef DEBUG
	bool						mBuilded;
#endif
};




///////							IMPLEMENTATION							////////


////////////////////////////////////////////////////////////////////////////////
inline int UnitGroup::calculatePathWithRadius(const sm::Vector2 &start,
		const sm::Vector2 &end,
		float radius)
{
	return mLevelManager->getPathfinderManager()->getPath(start, end, mPath, radius);
}


////////////////////////////////////////////////////////////////////////////////
inline void UnitGroup::setCreator(GameUnit *c)
{
	ASSERT(c);
	mCreator = c;
}
inline GameUnit *UnitGroup::getCreator(void)
{
	return mCreator;
}

////////////////////////////////////////////////////////////////////////////////
inline UnitGroup::Container &UnitGroup::getAllUnits(void)
{
	return mUnits;
}

////////////////////////////////////////////////////////////////////////////////
inline void UnitGroup::setFormation(UnitGroupFormation *formation)
{
	mFormation = formation;
#ifdef DEBUG
		mBuilded = false;
#endif
}

////////////////////////////////////////////////////////////////////////////////
inline void UnitGroup::setAttackStrategy(UnitAttackStrategy *as)
{
	mAttackStrategy = as;
#ifdef DEBUG
		mBuilded = false;
#endif
}


////////////////////////////////////////////////////////////////////////////////
inline void UnitGroup::setLevelManager(LevelManager	*lm)
{
	mLevelManager = lm;
}


#endif /* UNITGROUP_H_ */
