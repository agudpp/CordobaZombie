/*
 * ZombieUnit.h
 *
 *  Created on: 13/02/2012
 *      Author: agustin
 */

#ifndef ZOMBIEUNIT_H_
#define ZOMBIEUNIT_H_

#include <vector>
#include <set>

#include <BillboardManager/BillboardManager.h>
#include <BillboardManager/BillboardBatery.h>
#include <BillboardManager/BillboardWrapper.h>

#include "ZombieDefs.h"
#include "FSM/StateMachine.h"
#include "GameUnit.h"
#include "ZombieFactory.h"




// zombie state machine
typedef	ia::StateMachine<ZombieUnit *> ZombieStateMachine;
typedef ia::SMTransitionTable<ZombieUnit *> ZombieSMTTable;


class ZombieUnit : public GameUnit
{
public:
	// Here we will list the animation Index
	enum {
		ANIM_IDLE_BASE,
		ANIM_IDLE_2,
		ANIM_WALK_BASE,
		ANIM_WALK_2,
		ANIM_RUN_BASE,
		ANIM_ATTACK_BASE,
		ANIM_ATTACK_2,
		ANIM_DIE_L,
		ANIM_DIE_R,
		ANIM_DIE_B,
		ANIM_DIE_F,
		ANIM_HIT_F,
		ANIM_HIT_B,
		ANIM_SPREAD_BASE,
		ANIM_SPREAD_2,
	};

	// Zombies sound codes
	enum {
		SS_GRUNT_CODE,			// grunt (or bark, if zombie-dog)
		SS_MOB_GRUNT_CODE,		// many zombies performing a group attack
		SS_HIT_CODE,			// hit player
		SS_DYING_CODE			// killed by player
	};

	// Define all the possible events
	enum {
		E_SPREAD = 0,
		E_DONE,
		E_TARGET_FAR,
		E_TARGET_CLOSE,
		E_PERCEIVE_TARGET,
		E_TARGET_LOST,
		E_TARGET_KILLED,
		E_BEEN_HIT,
		E_DYING,
		E_FOLLOW_SOUND,
		E_ENGAGE,

	};


	////////////////////////////////////////////////////////////////////////////
	// 				Some constants used by the zombies						////
	////////////////////////////////////////////////////////////////////////////

	// Visibility radius (for targets)
	static const float	MAX_RADIUS_VISIBILITY;

	typedef std::vector<ZombieUnit *>	ZombieUnitVec;
	typedef std::set<GameUnit *>		TargetList;

public:
	ZombieUnit();
	virtual ~ZombieUnit();

	//////			FUNCTION INHERITED				///////

	/**
	 * This function have to be implemented that reproduce the effects when
	 * the object is selected
	 */
	virtual void objectSelected(void);

	/**
	 * Function called when the object is not anymore selected
	 */
	virtual void objectUnselected(void);

	/**
	 * Function called when the mouse is over the object / unit
	 */
	virtual void mouseOverObject(void);

	/**
	 * Function called when the mouse is exit of the unit
	 */
	virtual void mouseExitObject(void);

	/* Unit been hit by some other object */
	// TODO: ver que conviene pasar aca si un puntero a objeto o que cosa
	virtual void beenHit(const Hit_t &hit);

	/* Function called when the unit is getting alive. This function
	 * automatically reset the StateMachine
	 */
	virtual void borning(void);

	/* Function called when the unit is dead */
	virtual void dead(void);

	/**
	 * Function used to build the GameUnit, this will load all the animations
	 * and all the necessary things to get work property the class
	 */
	virtual bool build(void);

	/**
	 * Function used to advise the unit for a certain new event
	 */
	virtual void newExternalEvent(SMEvent e);

	// Update the unit:
	// TODO: probablemente no haga falta hacerla virtual a esta funcion si no
	// que simplemente haciendo mActualAnim->addTime(GLOBAL_TIME); y
	// mStateMachine.update(); alcance.... nos ahorramos una virtual call.
	virtual void update(void);

	 /** Function used to advise that some unit "unit" is close to this one.
	 * @param	unit	The unit that is close to "this" unit
	 */
	virtual void unitClose(GameUnit *unit);


	///////						Function of this class					//////


	/**
	 * Set the SMTransitionTable<ZombieUnit *> for this kind of objects
	 */
	static void setSMTransitionTable(ZombieSMTTable *tt);
	static ZombieSMTTable *getSMTransitionTable(void);

	/**
	 * Set the Blood BillboardBatery
	 */
	static void setBillboardBBlood(billboard::BillboardBatery *bbb);

	/**
	 * Set the ZombieQueue
	 */
	static void setQueue(ZombieQueue *q);

	/**
	 * Returns the walk velocity of the zombie
	 */
	inline float getWalkVelocity(void) const;
	inline void setWalkVelocity(float v);

	/**
	 * Set/Get the Attack velocity of the zombie
	 */
	inline float getAttackVelocity(void) const;
	inline void setAttackVelocity(float v);

	/**
	 * Set/Get attack power of the zombie
	 */
	inline float getAttackPower(void) const;
	inline void setAttackPower(float p);

	/**
	 * Get the squared attack range distance
	 */
	inline float getSqrAttackRange(void) const;

	/**
	 * Set/Returns the State Machine
	 */
	//void setFSM(StateMachine<ZombieUnit *>)
	inline ZombieStateMachine &getFSM(void);

	/**
	 * Function used to obtain all the zombies that are closer to this
	 * Zombie.
	 * The result is put in the mNearbyZombies;
	 * @return The mNearbyZombies vector.
	 * The second funcion (getNearbyZombies) do not perform any operation,
	 * only returns the last result obtained in findNearbyZombies()
	 */
	ZombieUnitVec &findNearbyZombies(float range = NEAR_ZOMBIES);
	inline ZombieUnitVec &getNearbyZombies(void);

	/**
	 * Advise the nearby zombies that we have seen a target and we are engaging
	 * it.
	 * TODO: hacer esta funcion
	 */
	void adviseHaveSeenTarget(void);

	/**
	 * Function used to check and get all the enemys/targets that this unit
	 * can see.
	 * If we perceive some target then is returned by param in the vector
	 * @param 	targets	The vector filled with all the targets seen
	 * @return 	True if we can see some targets
	 * 			flase otherwise
	 */
	bool seeTarges(std::vector<GameUnit *> &targets);

	/**
	 * Check if we can attack the actual target or not
	 */
	inline bool canAttackActualTarget(void) const;

	/*
	 * Called to make a sound get the attention of the zombie.
	 */
	void followSound(const Ogre::SceneNode * target);

	/*
	 * Sound target setter:
	 */
	inline void setSoundTarget(const Ogre::SceneNode * target);

	/*
	 * Sound target getter:
	 */
	inline const Ogre::SceneNode *getSoundTarget(void);

	/*
	 * Setter and getter for the spreading target
	 */

	inline void setSpreader(GameUnit *starget);
	inline GameUnit * getSpreader(void);


private:
	ZombieStateMachine				mFSM;
	float							mAttackPower;
	ZombieUnitVec					mNearbyZombies;
	TargetList						mTargets;
	const Ogre::SceneNode			*mSoundTarget;
	GameUnit						*mSpreader;
	billboard::BillboardWrapper		mSelBillboard;

	// The transition table used by all the zombies
	static ZombieSMTTable		*mSMTT;
	static ZombieQueue			*mQueue;
	static billboard::BillboardBatery		*mBloodBatery;
	static billboard::BillboardManager &mBillboardMngr;
};







///////							IMPLEMENTATION							////////


////////////////////////////////////////////////////////////////////////////////
inline float ZombieUnit::getWalkVelocity(void) const
{
	return mMinVelocity;
}
inline void ZombieUnit::setWalkVelocity(float v)
{
	setMinVelocity(v);
}

////////////////////////////////////////////////////////////////////////////////
inline float ZombieUnit::getAttackVelocity(void) const
{
	return mMaxVelocity;
}
inline void ZombieUnit::setAttackVelocity(float v)
{
	// we will use the "Max velocity as Attack velocity"
	setMaxVelocity(v);
}

////////////////////////////////////////////////////////////////////////////////
inline float ZombieUnit::getAttackPower(void) const
{
	return mAttackPower;
}
inline void ZombieUnit::setAttackPower(float p)
{
	ASSERT(p > 0.0f);
	mAttackPower = p;
}

////////////////////////////////////////////////////////////////////////////////
inline float ZombieUnit::getSqrAttackRange(void) const
{
	// we will use the radius as attack range by now
	return 1.5f*getSqrRadius();
}

////////////////////////////////////////////////////////////////////////////////
inline ia::StateMachine<ZombieUnit *> &ZombieUnit::getFSM(void)
{
	return mFSM;
}

////////////////////////////////////////////////////////////////////////////////
inline ZombieUnit::ZombieUnitVec &ZombieUnit::getNearbyZombies(void)
{
	return mNearbyZombies;
}

////////////////////////////////////////////////////////////////////////////////
inline bool ZombieUnit::canAttackActualTarget(void) const
{
	ASSERT(getActualTarget());

	return getPosition().squaredDistance(getActualTarget()->getPosition()) <
			getSqrAttackRange();
}


////////////////////////////////////////////////////////////////////////////////

inline const Ogre::SceneNode *ZombieUnit::getSoundTarget(void){
	return mSoundTarget;
}

////////////////////////////////////////////////////////////////////////////////
inline void ZombieUnit::setSoundTarget(const Ogre::SceneNode * target){
	mSoundTarget = target;
}


////////////////////////////////////////////////////////////////////////////////

inline void ZombieUnit::setSpreader(GameUnit *s)
{
	/*debugRAUL("Aca el argumento deberia ser const pero deja de ser compatible "
				"cuando quiero usar setActualTarget de gameUnit. :S esta mal "
				"setActualTarget? el atributo actual target deberia ser const? o "
				"necesitamos modificarlo? se puede sobrecargar la cosa, y tendra "
				"eso algun beneficio?");*/
	ASSERT(s);
	mSpreader = s;
}

////////////////////////////////////////////////////////////////////////////////

inline GameUnit * ZombieUnit::getSpreader(void)
{
	return mSpreader;
}


////////////////////////////////////////////////////////////////////////////////


#endif /* ZOMBIEUNIT_H_ */
