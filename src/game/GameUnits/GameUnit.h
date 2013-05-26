/*
 * GameUnit.h
 *
 *  Created on: 09/02/2012
 *      Author: agustin
 *
 */

#ifndef GAMEUNIT_H_
#define GAMEUNIT_H_

#include <OgreAnimationState.h>

#include <vector>

#include "DebugUtil.h"
#include "GameUnitDefines.h"
#include "PathfindingManager.h"
#include "SMDefines.h"
#include "GameObject.h"
#include "CommonMath.h"
#include "GlobalObjects.h"
#include "UnitPath.h"
#include "UnitWorldContext.h"
#include "UnitGroup.h"
#include "SoundFamilyTable.h"
#include "SoundAPI.h"


class GameUnit : public GameObject {
	// set the distance that determine if we are in the pathpoint or not
	// squared distance)
	static const float SQRD_PATHPOINT_DIST;

public:
	GameUnit();
	virtual ~GameUnit();

	/**
	 * This function have to be implemented that reproduce the effects when
	 * the object is selected
	 */
	virtual void objectSelected(void) = 0;

	/**
	 * Function called when the object is not anymore selected
	 */
	virtual void objectUnselected(void) = 0;

	/**
	 * Function called when the mouse is over the object / unit
	 */
	virtual void mouseOverObject(void) = 0;

	/**
	 * Function called when the mouse is exit of the unit
	 */
	virtual void mouseExitObject(void) = 0;

	/* Unit been hit by some other object */
	virtual void beenHit(const Hit_t &hit){mLastHit = hit;ASSERT(false);};

	/* Function called when the unit is getting alive. This function
	 * automatically reset the StateMachine
	 */
	virtual void borning(void){ASSERT(false);};

	/**
	 * Function called when the unit is dead, if we have to do some
	 * cleanup or something specialized
	 */
	virtual void dead(void){ASSERT(false);};

	/* Returns the actual life of the game unit */
	inline live_t getLife(void) const;
	inline void setLife(live_t l);

	/**
	 * Function used to build the GameUnit, this will load all the animations
	 * and all the necessary things to get work property the class
	 */
	virtual bool build(void) = 0;

	/**
	 * Function used to advise that some unit "unit" is close to this one.
	 * @param	unit	The unit that is close to "this" unit
	 */
	virtual void unitClose(GameUnit *unit) {ASSERT(false);};

	/**
	 * Returns the last Hit_t Object that "hit" this GameUnit
	 */
	inline const Hit_t &getLastHit(void) const;

	/**
	 * Returns where hits the last hit to the player.
	 * @return	(Hit_t::FRONT | Hit_t::BACK | Hit_t::LEFT..)
	 */
	int getLastHitImpactDir(void) const;

	/**
	 * Returns the actual unit "IA" State, this will be used by the other units
	 * that need to know what this unit is doing...
	 */
	inline UnitIAState &getIAState(void);

	/**
	 * Function called when this unit is added to a new group
	 */
	inline void addedToGroup(UnitGroup *g);

	/**
	 * Function called when the unit is removed from the group
	 */
	inline void removedFromGroup(UnitGroup *g);

	/**
	 * Returns the UnitGroup of this unit if it have or 0 otherwise
	 */
	inline UnitGroup *getUnitGroup(void);

	/**
	 * Check if we are in some group
	 */
	inline bool isInGroup(void);

	/**
	 * Check if the unit is the creator of the group
	 */
	inline bool isCreatorOfGroup(void);

	/**
	 * Returns the unit radius... mmm We have the getSqrRadius in GameObject
	 * class and this funcion here... fucking ugly
	 */
	inline float getRadius(void) const;

	////////////////////////////////////////////////////////////////////////////
	/////////////			START PATH FUNCTIONS				////////////////
	////////////////////////////////////////////////////////////////////////////

	/* Returns the last path calculated for this unit */
	inline PathfindingManager::Path &getPath(void);
	inline void setPath(const PathfindingManager::Path &p);

	/* Calculates the path to a certain position from the unit position to
	 * "goal" point. This function not necessary returns the path instantaneously
	 * this means that we have to check if the path is ready or not using
	 * "isPathReady()".
	 * The path will be set to mPath
	 * @return	true if it can find a path or false otherwise (note that
	 * 			if we can find a path and is of size 0 the path then we
	 * 			can move directly).
	 */
	inline bool getPathTo(const Ogre::Vector3 &p);
	bool getPathTo(const math::Vector2 &p);
	inline bool isPathReady(void) const {return mPathReady;}

	/**
	 * Reset the path indexing (set the index at the begining point)
	 */
	inline void restartPath(void);

	/**
	 * Returns true if there are more points to travel, false otherwise
	 */
	inline bool hasMorePathPoints(void) const;

	/**
	 * Return the UnitPath associated to the unit
	 */
	inline UnitPath &getUnitPath(void);
	inline const UnitPath &getUnitPath(void) const;

	/**
	 * Returns the actual pathPoint
	 */
	inline const math::Vector2 &getActualPathPoint(void) const;
	inline int getActualPathIndex(void) const;

	/**
	 * DESCRIPTION: Makes the game unit follow the path determined by the
	 * steering behaviors functions.
	 * RETURN VALUES: Returns true if he is going to move, false otherwise.
	 * NOTES:
	 */
	bool moveThroughPath(void);

	///////////////////////////////////////////////////////////////////////////
	/////////////			END PATH FUNCTIONS					///////////////
	///////////////////////////////////////////////////////////////////////////


	/* Function used to advise the unit for a certain new event */
	virtual void newExternalEvent(SMEvent e){ASSERT(false);};

	// Update the unit:
	// TODO: probablemente no haga falta hacerla virtual a esta funcion si no
	// que simplemente haciendo mActualAnim->addTime(GLOBAL_TIME); y
	// mStateMachine.update(); alcance.... nos ahorramos una virtual call.
	virtual void update(void){ASSERT(false);};

	/* Returns the actual animation */
	inline Ogre::AnimationState *getActualAnimation(void) const;
	inline int getActualAnimID(void) const;

	/* Check if the actual animation has ended */
	inline bool hasActualAnimEnd(void) const;

	/* Change the actual animation */
	inline void changeAnimation(int animID);

	/**
	 * Restart the actual animation to the begin pos
	 */
	inline void restartActualAnimation(void);

	/**
	 * Set/Set the animation acceleration coefficient (if coef is greater than
	 * 1 then it will run the animation faster, if coef < 1 then it will run
	 * slower)
	 */
	inline void setAnimationAccelCoef(float coef);
	inline float getAnimationAccelCoef(void) const;

	/**
	 * Set/Get the max and min velocity of the unit. This values will
	 * be used in some of the steering behaviors functions
	 */
	inline void setMaxVelocity(float v);
	inline float getMaxVelocity(void) const;
	inline void setMinVelocity(float v);
	inline float getMinVelocity(void) const;

	/**
	 * Set/Get the actual velocity to the unit.
	 */
	inline void setVelocity(float v);
	inline float getVelocity(void) const;

	/**
	 * Returns the left and right vectors representing the unit vision.
	 * @param unitDir	Is the unit direction movement.
	 * @param left		The left line of vision
	 * @param right		The right line of vision
	 */
	inline void getFieldOfVision(math::Vector2 &unitDir,
			math::Vector2 &left, math::Vector2 &right) const;

	/**
	 * Function used to move the unit in a certain direction.
	 * This function automatically use the last time frame elapsed to
	 * multiply the vector, translate the object and rotate the unit to
	 * see to the translated direction.
	 * @note	This function NOT NORMALIZE the move vector, assume is normalized
	 * 			AND the vector is modified for performance
	 */
	inline void move(math::Vector2 &move);


	/**
	 * Sets/Gets the unit enemy target to attack
	 */
	inline GameUnit *getActualTarget(void);
	inline const GameUnit *getActualTarget(void) const;
	inline void setActualTartet(GameUnit *t);

	inline float distToActualTarget(void);

	/**
	 * This funcion is used to update the path from this unit to the TargetUnit.
	 * Once this function is called the UnitPath of this unit is updated and if
	 * we follow the "path" then we will get to the target
	 * @requires	TargetUnit != 0
	 *
	 * @return		True if we can get the path, or false otherwise
	 */
	bool updateTargetPath(void);


	////////////////////////////////////////////////////////////////////////////
	////////////			STEERING BEHAVIORS				////////////////////
	////////////////////////////////////////////////////////////////////////////

	/**
	 * Calculates the accumulated steering force that "avoid" unit overlapping.
	 * We will add all the "repelling" forces of every collision object.
	 * @param result	The result "force"
	 * @param objects	The list of objects that intersect the unit
	 * TODO: probablemente podamos eliminar objects y obtenerlo directamente
	 * 		 leyendo el miembro protected...
	 */
	void repellingSteerVec(math::Vector2 &result, const CollisionResult &objects);

	/**
	 * Calculate the seek vector given a target pos. This returns the "move"
	 * vector.
	 * @param result	The result "force"
	 * @param target	The target position to move
	 */
	void seekSteerVec(math::Vector2 &result, const math::Vector2 &target);

	/**
	 * Calculate the force needed to follow a path. This use the internal
	 * Path. In case that we have no more pathPoints to follow then this
	 * returns false
	 * @param	result	The result force
	 * @return	false	if no more pathPoints, true otherwise
	 */
	bool followPathSteerVec(math::Vector2 &result);

	/**
	 * Calculate the vector corresponding to avoid a "movable" object in the
	 * world.
	 * @param result	The result "force"
	 * @param obj		The object to avoid
	 */
	void avoidanceSteerVec(math::Vector2 &result, const GameObject *obj);

	//-------------------	GROUP STEERING BEHAVIORS	-----------------------/

	/**
	 * Calculate the vector associated to the cohesion steering vector using
	 * the UnitGroup units
	 * @param reslut 	The resulting cohesion vector
	 */
	void cohesionSteerVec(math::Vector2 &result);

	/**
	 * Calculate the vector associated to the alignment steering vector using
	 * the UnitGroup units
	 * @param reslut 	The resulting alignment vector
	 */
	void alignmentSteerVec(math::Vector2 &result);

	/**
	 * Calculate the vector associated to the separation steering vector using
	 * the UnitGroup units
	 * @param reslut 	The resulting separation vector
	 */
	void separationSteerVec(math::Vector2 &result);


	//-------------------	END GROUP STEERING BEHAVIORS	-------------------/

	////////////////////////////////////////////////////////////////////////////
	////////////			END STEERING BEHAVIORS				////////////////
	////////////////////////////////////////////////////////////////////////////

	/**
	 * Function used to check if this unit can see another game unit but in
	 * a roughly mode. Using line of sight matrix
	 * @param	unit		The other unit to check if we can see.
	 * @param	maxSqrDist	The maximun distance that we can see
	 */
	bool haveLineOfSightTo(const GameUnit *other, float maxSqrDist) const;

	/**
	 * Return a reference to the GameUnit's SoundAPI.
	 */
	inline mm::SoundAPI& getSoundAPI();

	/**
	 * Attaches the GameUnit's SceneNode to the SoundAPI.
	 * This way played sounds will stick to the unit's dynamic position.
	 */
	inline void configureSoundAPI();

	/**
	 * TODO: agregar el tema de manejo de grupo
	 */

	/*
	 * Timer setter and getter
	 */
	inline void setTimer(float time);
	inline float getTimer(void);

protected:
	/* Function used to load a list of animations in the order given by
	 * the list. This function creates the mAnimations array */
	void loadAnimations(const std::vector<Ogre::String> &anims);

	/**
	 * Configure the unit path of this unit
	 */
	void configureUnitPath(void);


protected:
	live_t						mLife;
	Ogre::AnimationState		**mAnimations;
	Ogre::AnimationState		*mActualAnim;
	int 						mActualAnimID;
	float						mAnimAccelCoef;
	PathfindingManager::Path	mPath;
	UnitPath					mUnitPath;
	int							mPathIndex;
	bool						mPathReady;
	float						mMaxVelocity;
	float						mMinVelocity;
	float						mActualVelocity;
	Hit_t						mLastHit;
	UnitGroup					*mUnitGroup;
	GameUnit					*mTarget;
	const GNode					*mLastTargetPos;
	UnitIAState					mIAState;
	float						mTimer;

public:
	/* XXX	Es al pedo hacer una intefaz para manipular los sonidos.     *
	 *		No añade nada, e interpone otra capa de llamada a funciones. */
	mm::SoundAPI				mSAPI;
	mm::SoundFamilyTable		mSounds;
};






///////							IMPLEMENTATION							////////



////////////////////////////////////////////////////////////////////////////////
inline const Hit_t &GameUnit::getLastHit(void) const
{
	return mLastHit;
}

////////////////////////////////////////////////////////////////////////////////
inline UnitIAState &GameUnit::getIAState(void)
{
	return mIAState;
}

////////////////////////////////////////////////////////////////////////////////
inline void GameUnit::addedToGroup(UnitGroup *g)
{
	ASSERT(mUnitGroup == 0);
	ASSERT(g);
	mUnitGroup = g;
}

////////////////////////////////////////////////////////////////////////////////
inline void GameUnit::removedFromGroup(UnitGroup *g)
{
	ASSERT(mUnitGroup == g);
	mUnitGroup = 0;

}

////////////////////////////////////////////////////////////////////////////////
inline UnitGroup *GameUnit::getUnitGroup(void)
{
	return mUnitGroup;
}

////////////////////////////////////////////////////////////////////////////////
inline bool GameUnit::isInGroup(void)
{
	return mUnitGroup != 0;
}

////////////////////////////////////////////////////////////////////////////////
inline bool GameUnit::isCreatorOfGroup(void)
{
	ASSERT(mUnitGroup);
	return mUnitGroup->getCreator() == this;
}

////////////////////////////////////////////////////////////////////////////////
inline float GameUnit::getRadius(void) const
{
	return mUnitPath.getRadius();
}

////////////////////////////////////////////////////////////////////////////////
inline live_t GameUnit::getLife(void) const
{
	return mLife;
}

////////////////////////////////////////////////////////////////////////////////
inline void GameUnit::setLife(live_t l)
{
	mLife = l;
}

////////////////////////////////////////////////////////////////////////////////
inline PathfindingManager::Path &GameUnit::getPath(void)
{
	return mUnitPath.getPointPath();
}

////////////////////////////////////////////////////////////////////////////////
inline void GameUnit::setPath(const PathfindingManager::Path &p)
{
//	mPath = p; mPathReady = true;
	mUnitPath.setPath(p);
	restartPath();
}

////////////////////////////////////////////////////////////////////////////////
inline bool GameUnit::getPathTo(const Ogre::Vector3 &p)
{
	return getPathTo(math::Vector2(p.x, p.z));
}

////////////////////////////////////////////////////////////////////////////////
inline void GameUnit::restartPath(void)
{
	mUnitPath.update();
	// we avoid the first position that is the "user" position
//	mPathIndex = 1;
}


////////////////////////////////////////////////////////////////////////////////
inline bool GameUnit::hasMorePathPoints(void) const
{
	return mUnitPath.hasMorePathPoints();
}

////////////////////////////////////////////////////////////////////////////////
inline UnitPath &GameUnit::getUnitPath(void)
{
	return mUnitPath;
}

////////////////////////////////////////////////////////////////////////////////
inline const UnitPath &GameUnit::getUnitPath(void) const
{
	return mUnitPath;
}

////////////////////////////////////////////////////////////////////////////////
inline const math::Vector2 &GameUnit::getActualPathPoint(void) const
{
	return mUnitPath.getActualPoint();
}

inline int GameUnit::getActualPathIndex(void) const
{
	return mUnitPath.getActualIndex();
}


////////////////////////////////////////////////////////////////////////////////
inline Ogre::AnimationState *GameUnit::getActualAnimation(void) const
{
	return mActualAnim;
}

////////////////////////////////////////////////////////////////////////////////
inline int GameUnit::getActualAnimID(void) const
{
	return mActualAnimID;
}

////////////////////////////////////////////////////////////////////////////////
inline bool GameUnit::hasActualAnimEnd(void) const
{
	ASSERT(mActualAnim);
	return mActualAnim->hasEnded();
}

////////////////////////////////////////////////////////////////////////////////
inline void GameUnit::changeAnimation(int animID)
{
	ASSERT(mActualAnim);
	mActualAnim->setEnabled(false);
	mActualAnim = mAnimations[animID];
	mActualAnimID = animID;
	mActualAnim->setTimePosition(0.0f);
	mActualAnim->setEnabled(true);
}

////////////////////////////////////////////////////////////////////////////////
inline void GameUnit::restartActualAnimation(void)
{
	mActualAnim->setTimePosition(0.0f);
	if(!mActualAnim->getEnabled()){mActualAnim->setEnabled(true);}
}

////////////////////////////////////////////////////////////////////////////////
inline void GameUnit::setAnimationAccelCoef(float coef)
{
	ASSERT(coef > 0);
	mAnimAccelCoef = coef;
}
inline float GameUnit::getAnimationAccelCoef(void) const
{
	return mAnimAccelCoef;
}

////////////////////////////////////////////////////////////////////////////////
inline void GameUnit::setMaxVelocity(float v)
{
	ASSERT(v > 0.0f);
	mMaxVelocity = v;
}
inline float GameUnit::getMaxVelocity(void) const
{
	return mMaxVelocity;
}
inline void GameUnit::setMinVelocity(float v)
{
	ASSERT(v > 0.0f);
	mMinVelocity = v;
}
inline float GameUnit::getMinVelocity(void) const
{
	return mMinVelocity;
}

////////////////////////////////////////////////////////////////////////////////
inline void GameUnit::setVelocity(float v)
{
	ASSERT(v > 0.0f);
	mActualVelocity = v;
}
inline float GameUnit::getVelocity(void) const
{
	return mActualVelocity;
}


////////////////////////////////////////////////////////////////////////////////
inline void GameUnit::getFieldOfVision(math::Vector2 &unitDir,
			math::Vector2 &left, math::Vector2 &right) const
{
	getDirection(unitDir);
	left = unitDir;
	left.transformToNormal();
	right = left;
	right *= -1.0f;
}

////////////////////////////////////////////////////////////////////////////////
inline void GameUnit::move(math::Vector2 &move)
{
	move *= GLOBAL_TIME_FRAME;
	translate(move);
	lookAt(getPosition() + move);
}

////////////////////////////////////////////////////////////////////////////////
inline GameUnit *GameUnit::getActualTarget(void)
{
	return mTarget;
}
inline const GameUnit *GameUnit::getActualTarget(void) const
{
	return mTarget;
}

////////////////////////////////////////////////////////////////////////////////
inline void GameUnit::setActualTartet(GameUnit *t) { mTarget = t; }

////////////////////////////////////////////////////////////////////////////////
inline float GameUnit::distToActualTarget(void){
	return mTarget->getPosition().squaredDistance(getPosition());
}

////////////////////////////////////////////////////////////////////////////////
inline mm::SoundAPI& GameUnit::getSoundAPI() { return mSAPI; }

////////////////////////////////////////////////////////////////////////////////
inline void GameUnit::configureSoundAPI() { mSAPI.setSceneNode(getSceneNode());}

////////////////////////////////////////////////////////////////////////////////
inline void GameUnit::setTimer(float time)
{
	mTimer = time;
}

////////////////////////////////////////////////////////////////////////////////
inline float GameUnit::getTimer(void)
{
	return mTimer;
}

#endif /* GAMEUNIT_H_ */
