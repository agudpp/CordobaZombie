/*
 * CivilUnit.h
 *
 *  Created on: 12/07/2012
 *      Author: agustin
 */

#ifndef CIVILUNIT_H_
#define CIVILUNIT_H_


#include "DebugUtil.h"
#include "CivilDefs.h"
#include "GameUnit.h"
#include "FSM/StateMachine.h"


class CivilUnit;
typedef	ia::StateMachine<CivilUnit *> CivilStateMachine;
typedef ia::SMTransitionTable<CivilUnit *> CivilSMTTable;

class CivilUnit : public GameUnit
{
public:

	// How much closer to the player we can be to be considered "safe"
	static const float	SAFE_SQR_DIST		=	15.0f*15.0f;

	// How many triangles we can ran away (for escaping)
	static const int	MIN_NUM_NODES		=	3;
	static const int	MAX_NUM_NODES		=	6;

	// Here we will list the animation Index
	enum {
		ANIM_IDLE_BASE = 0,
		ANIM_IDLE_2,
		ANIM_IDLE_3,
		ANIM_RUN_BASE,
		ANIM_RUN_2,
		ANIM_RUN_ESCAPE,
		ANIM_DIE_L,
		ANIM_DIE_R,
		ANIM_DIE_B,
		ANIM_DIE_F,
		ANIM_HIT_F,
		ANIM_HIT_B,
	};

	// Civilians sound codes
	enum {
		SS_COMMAND_OK_CODE,		// Received a player command
		SS_SCREAM_CODE,			// Random scream
		SS_WOUNDED_SCREAM_CODE,	// Was hit     by zombie
		SS_DYING_CODE,			// Was killed  by zombie
		SS_RESCUED_CODE			// Was rescued by chopper
	};

	// Define all the possible events
	enum {
		E_DONE,
		E_FOLLOW_UNIT,
		E_UNIT_LOST,
		E_BEEN_HIT,
		E_DYING,
		E_BEEN_RESCUED,
		E_ESCAPE,
	};

	typedef std::vector<GameUnit *>	GameUnitVec;

public:
	CivilUnit();
	virtual ~CivilUnit();


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
	virtual void beenHit(const Hit_t &hit);

	/* Function called when the unit is getting alive. This function
	 * automatically reset the StateMachine
	 */
	virtual void borning(void);

	/**
	 * Function called when the unit is dead, if we have to do some
	 * cleanup or something specialized
	 */
	virtual void dead(void);

	/**
	 * Function used to build the GameUnit, this will load all the animations
	 * and all the necessary things to get work property the class
	 */
	virtual bool build(void);

	/**
	 * Function used to advise that some unit "unit" is close to this one.
	 * @param	unit	The unit that is close to "this" unit
	 */
	virtual void unitClose(GameUnit *unit);

	/**
	 * Function used to advise the unit for a certain new event
	 */
	virtual void newExternalEvent(SMEvent e);

	/**
	 * Update function
	 */
	virtual void update(void);


	////////////////////////////////////////////////////////////////////////////
	///						Function of this class							////
	////////////////////////////////////////////////////////////////////////////

	/**
	 * Set the GameUnit to follow.
	 * @param	gu		GameUnit to follow
	 */
	void followUnit(GameUnit *gu);
	inline GameUnit *getFollowingUnit(void);
	inline void stopFollowUnit(void);

	/**
	 * Returns the FinitStateMachine
	 */
	inline CivilStateMachine &getFSM(void);

	/**
	 * Set the SMTTable of the civils
	 */
	static void setSMTTable(CivilSMTTable *t);



protected:

	/**
	 * Advise the nearby zombies that we are closer to they
	 */
	void adviseNearbyZombies(void);

protected:
	CivilStateMachine				mFSM;
	GameUnit						*mFollowingUnit;


	// the nearby zombies vector will be static because we dont need to track
	// the zombies closer to the civil, we only use this vector to advise the
	// zombies that we are close
	static GameUnitVec				mNearbyZombies;
	static CivilSMTTable			*mSMTTable;

};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
inline GameUnit *CivilUnit::getFollowingUnit(void)
{
	return mFollowingUnit;
}
inline void CivilUnit::stopFollowUnit(void)
{
	// TODO: emitir un evento de que deje de seguir a la unidad? o ya lo estamos
	// checkeando dentro del estado que mFollowingUnit != 0
	mFollowingUnit = 0;
}

////////////////////////////////////////////////////////////////////////////////
inline CivilStateMachine &CivilUnit::getFSM(void)
{
	return mFSM;
}

#endif /* CIVILUNIT_H_ */
