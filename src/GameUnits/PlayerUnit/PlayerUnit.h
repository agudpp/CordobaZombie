/*
 * PlayerUnit.h
 *
 *  Created on: 02/03/2012
 *      Author: agustin
 */

#ifndef PLAYERUNIT_H_
#define PLAYERUNIT_H_


#include <boost/shared_ptr.hpp>

#include "FSM/StateMachine.h"
#include "GameUnit.h"
#include "PlayerDefs.h"
#include "Weapon.h"
#include "Bomb.h"
#include "GameItem.h"
#include "PlayerWorldContext.h"
#include "CollectableObject.h"
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreVector3.h>
#include "CollectableObject.h"
#include "Backpack.h"
#include "BackpackItem.h"
#include "GeneralTypedefs.h"
#include "BackpackItemAssociator.h"
#include "BillboardManager.h"

//#include "CollectableObjTypes.h"

class PlayerUnit;

// zombie state machine
typedef	ia::StateMachine<PlayerUnit *> PlayerStateMachine;
typedef ia::SMTransitionTable<PlayerUnit *> PlayerSMTTable;

typedef GenericFunctor<void, int>		PlayerCallback;

class PlayerUnit : public GameUnit
{
	// The time that have to pass to update the path in engaging state
	static const float	UPDATE_PATH_TIME;

public:
	typedef std::vector<GameUnit *>	ZombieUnitVec;
	typedef std::vector<PlayerUnit *>	PlayerVector;
	typedef std::vector<GameItem *>		ItemsVec;
	typedef std::vector<Bomb *>			BombsVec;

public:
	// enum the animations

	enum {
		ANIM_IDLE,
		ANIM_WALK,
		ANIM_RUN,
		ANIM_ATTACK_BODY,
		ANIM_ATTACK_SHORT,
		ANIM_ATTACK_SHORT2,
		ANIM_RELOAD_SHORT,
		ANIM_ATTACK_LONG,
		ANIM_ATTACK_LONG2,
		ANIM_RELOAD_LONG,
		ANIM_ATTACK_GRANADE,
		ANIM_PICK_OBJECT,
		ANIM_PLANT_BOMB,
		ANIM_GAB_MOV_OBJ,
		ANIM_PUSH_MOV_OBJ,
		ANIM_PULL_MOV_OBJ,
		ANIM_DIE_L,
		ANIM_DIE_R,
		ANIM_DIE_B,
		ANIM_DIE_F,
		ANIM_HIT_F,
		ANIM_HIT_B
	};

	// Players sound codes
	enum {
		SS_COMMAND_ACCEPTED_CODE,	// "Yes sir!"
		SS_COMMAND_REJECTED_CODE,	// "Can't do that"
		SS_WOUNDED_CODE,			// Hit by zombie
		SS_DYING_CODE,				// Killed
		SS_HEALED_CODE,				// Healed
		SS_CIVILIAN_FOLLOW_CODE,	// To civilian: "Follow me"
		SS_CIVILIAN_STAY_CODE		// To civilian: "Stay here"
	};

	// Enum the possible events of the unit
	enum {
		E_DONE,
		E_MOVE_TO,
		E_BEEN_HIT,
		E_ENGAGE_UNIT,
		E_ENEMY_FAR_AWAY,			// when the enemy is far away from the range
									// of fire of the unit
		E_ENEMY_SEEN,				// When we see the enemy and is ready to
									// attack
		E_ENEMY_LOST,				// when the enemy disapear...
		E_ENEMY_DIE,
		E_NEED_RELOAD,
		E_PLANT_BOMB,
		E_PICK_OBJECT,
		E_DYING,

	};


	// The internal attacking behavior
	enum {
		BH_NORMAL,
		BH_AGRESIVE
	};

public:
	PlayerUnit();
	virtual ~PlayerUnit();

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
	virtual void unitClose(GameUnit *unit) ;


	/* Function used to advise the unit for a certain new event */
	virtual void newExternalEvent(SMEvent e);

	// Update the unit:
	// TODO: probablemente no haga falta hacerla virtual a esta funcion si no
	// que simplemente haciendo mActualAnim->addTime(GLOBAL_TIME); y
	// mStateMachine.update(); alcance.... nos ahorramos una virtual call.
	virtual void update(void);


	///////						Function of this class					//////

	/**
	 * @brief Set/Get the player ID
	 */
	inline PlayerID playerID(void) const;
	inline void setPlayerID(PlayerID pid);

	/**
	 * Set the callback to be called when the life of the player change
	 * @param 	cb		the callback
	 */
	inline void setLifeCallback(PlayerCallback *cb);
	inline void clearLifeCallback(void);


	////////////////////////////////////////////////////////////////////////////
	////				Weapon / Bombs / Items Functions					////
	////////////////////////////////////////////////////////////////////////////

	/**
	 * Add a new weapon to this unit
	 */
	void addNewWeapon(Weapon *w);

	/**
	 * Check if the unit has a weapon type
	 */
	inline bool hasWeaponType(int wtype);

	/**
	 * Check the weapon associated to a type of weapon. Or 0 if has not got that
	 * kind of weapon
	 */
	inline Weapon *getWeapon(int wtype);

	/**
	 * Returns the actual weapon, or 0 if has not got
	 */
	inline Weapon *getActualWeapon(void);

	/**
	 * Remove a weapon
	 */
	void removeWeapon(int wtype);

	/**
	 * Set the actual weapon used by this player
	 */
	void setWeapon(int wtype);

	/**
	 * Change the best "next" weapon that we can use.
	 * This function is called when we need to change the weapon fast to kill
	 * some zombie and the actual weapon have no more ammunition
	 */
	void changeBestNextWeapon(void);

	/**
	 * Bomb Functions
	 * @note	Once we add a new Bomb this class will be the owner of the memory
	 */
	void addBomb(Bomb *b);
	void removeBomb(Bomb *b);
	inline Bomb *getActualBomb(void);
	inline bool hasBomb(Bomb *);
	inline BombsVec &getBombs(void);

	/**
	 * Function used to plant a bomb in some place specified by the user.
	 * @return	True	if it can put the bomb in that place, false otherwise
	 */
	bool plantBomb(Bomb *b, const sm::Vector2 &position);

	/**
	 * GameItems functions
	 * @note	Once we add an item to the player the player is the owner of the
	 * 			memory.
	 */
	void addItem(GameItem *i);
	void removeItem(GameItem *i);
	void useItem(GameItem *i);
	inline bool hasItem(GameItem *i);
	inline ItemsVec &getItems(void);


	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////

	/**
	 * Need to update the path to the actual engaging unit
	 */
	inline bool needUpdatePath(void);

	////////////////////////////////////////////////////////////////////////////

	/**
	 * Returns the UnitWorldContext of the unit or 0 if not have
	 */
	inline PlayerWorldContext &getWorldContext(void);

	/**
	 * Function used to obtain all the zombies that are closer to this
	 * Player.
	 * @return The mNearbyZombies vector.
	 */
	inline ZombieUnitVec &getNearbyZombies(void);

	/**
	 * Function used to get all the Player that are closer to this unit
	 */
	void getNearbyPlayers(PlayerVector &players);

	/**
	 * Say the unit to go to a certain point. Get the path associated to that
	 * point and move to there
	 * @param p		The position where we have to move the unit
	 */
	void moveUnitTo(const sm::Vector2 &p);

	/**
	 * Get/Set the internal Attacking behavior
	 */
	inline int getAttackingBehavior(void) const;
	inline void setAttackingBehavior(int b);

	/**
	 * Set/Returns the State Machine
	 */
	inline PlayerStateMachine &getFSM(void);

	/**
	 * Set the SMTransitionTable<PlayerUnit *> for this kind of objects
	 */
	static void setSMTransitionTable(PlayerSMTTable *tt);

	/**
	 * Check if we can attack a unit with the actual weapon and assume that
	 * we haveLineOfSight with the unit t.
	 */
	inline bool canAttackTarget(const GameUnit *t);

	/**
	 * Function used to start the "engaging" logic of the player. This will trigger
	 * the E_ENGAGE_UNIT event
	 * @param t		The target that we want to attack
	 */
	inline void engageUnit(GameUnit *t);

	/**
	 * Function called when player wants to collect a collectable object from the scene.
	 */
	void collectObject(CollectableObject *c);

	/**
	 * Function used to add a CollectableObject to the Player (for example, once
	 * the object is picked, this function must be called with the object
	 * itself).
	 * This function will create the Backpack representation and put the object
	 * into the Backpack
	 * @note	This function will not change the state of the CollectableObject
	 */
	void addCollectObeject(CollectableObject *c);

	/**
	 * Set/get the Target collectable object to be picked by the player.
	 */
	inline CollectableObject *getTargetCollObject(void);
	inline void setTargetCollObject(CollectableObject *newObj);

	/**
	 * DESCRIPTION: checks if this player is able to pick his target object.
	 * RETURNS: true if he can, false otherwise.
	 * NOTES: the result of this functions depends on diferent parameters. For
	 * example some player may not be able to pick some kind of objects, or
	 * some player may have to many of some kind of objects and does not being
	 * able for them to carry any more of that kind.
	 */
	bool canPickObject(void);

	/**
	 * Returns the Backpack of the player
	 */
	inline Backpack *getBackpack(void);

protected:
	/**
	 * Attach and detach a weapon from the unit
	 */
	void attachWeaponToBone(Weapon *newWeapon, Weapon *oldWeapn);


	/**
	 * Add/Remove objects from the backpack
	 */
	inline void addBackPackItem(BackpackItem *item);
	inline void removeBackPackItem(BackpackItem *item);

	/**
	 * This function must be called when we add pick a object or add a new
	 * item/weapon/bomb to the player. This function will create the "visual"
	 * element to be added in the backpack
	 * @param	e		The element to be added into the backpack
	 */
	inline void createBackpackInterface(Weapon *e);
	inline void createBackpackInterface(Bomb *e);
	inline void createBackpackInterface(GameItem *e);



private:
	// Avoid copying players
	//
	PlayerUnit(const PlayerUnit&);
	PlayerUnit &operator=(const PlayerUnit&);

	/**
	 * Obtain all the nearby zombies to this unit and save the result in
	 * mNearbyZombies vector
	 * This function although advise that we are closer (this is faster than
	 * every zombie check always for player unit closer).
	 */
	void obtainAndAdviseNearbyZombies(void);

	/**
	 * Load the Billboard associated to the players
	 */
	void loadBillboards(void);




private:
	PlayerStateMachine              mFSM;
	GameUnit                        *mTarget;
	int                             mAttackingBehavior;
	ZombieUnitVec                   mNearbyZombies;
	Weapon                          *mWeapons[Weapon::W_NONE];
	int                             mActualWeapon;
	PlayerWorldContext              mWorldContext;
	Ogre::Billboard                 *mSelBillboard;
	float                           mUpdatePathTime;
	Bomb                            *mActualBomb;
	CollectableObject               *mTargetColObject;
	Backpack                        mBackPack;
	PlayerCallback                  *mLifeChangeCb;
	BombsVec                        mBombs;
	ItemsVec                        mItems;
	PlayerID                        mPlayerID;



	// The billboard Manager used by all the units
	static	BillboardManager				*mBillboardMngr;

	// The transition table used by all the players.. if we will use a unique
	// otherwise we have to delete this
	static PlayerSMTTable		*mSMTT;
};


// typedef
//
typedef boost::shared_ptr<PlayerUnit> PlayerUnitPtr;


///////							IMPLEMENTATION							////////


////////////////////////////////////////////////////////////////////////////////
inline PlayerID
PlayerUnit::playerID(void) const
{
    return mPlayerID;
}
inline void
PlayerUnit::setPlayerID(PlayerID pid)
{
    mPlayerID = pid;
}

////////////////////////////////////////////////////////////////////////////////
inline void PlayerUnit::setLifeCallback(PlayerCallback *cb)
{
	ASSERT(cb);
	mLifeChangeCb = cb;
}
inline void PlayerUnit::clearLifeCallback(void)
{
	mLifeChangeCb = 0;
}

////////////////////////////////////////////////////////////////////////////////
inline bool PlayerUnit::hasWeaponType(int wtype)
{
#ifdef DEBUG
	if(wtype >= Weapon::W_NONE || wtype < 0){
		debugERROR("Invalid weapon wtype: %d\n", wtype);
	}
#endif
	ASSERT(wtype < Weapon::W_NONE);
	ASSERT(wtype >= 0);
	return mWeapons[wtype] != 0;
}

////////////////////////////////////////////////////////////////////////////////
inline Weapon *PlayerUnit::getWeapon(int wtype)
{
	ASSERT(wtype < Weapon::W_NONE);
	return mWeapons[wtype];
}

////////////////////////////////////////////////////////////////////////////////
inline Weapon *PlayerUnit::getActualWeapon(void)
{
	return mWeapons[mActualWeapon];
}

////////////////////////////////////////////////////////////////////////////////
inline Bomb *PlayerUnit::getActualBomb(void)
{
	return mActualBomb;
}
inline bool PlayerUnit::hasBomb(Bomb *b)
{
	for(register int i = mBombs.size()-1; i >= 0; --i)
		if(mBombs[i] == b)
			return true;
	return false;
}
inline PlayerUnit::BombsVec &PlayerUnit::PlayerUnit::getBombs(void)
{
	return mBombs;
}

////////////////////////////////////////////////////////////////////////////////
inline bool PlayerUnit::hasItem(GameItem *it)
{
	for(register int i = mItems.size()-1; i >= 0; --i)
		if(mItems[i] == it)
			return true;
	return false;
}
inline PlayerUnit::ItemsVec &PlayerUnit::getItems(void)
{
	return mItems;
}

////////////////////////////////////////////////////////////////////////////////
inline bool PlayerUnit::needUpdatePath(void)
{
	mUpdatePathTime += GLOBAL_TIME_FRAME;
	if(mUpdatePathTime > UPDATE_PATH_TIME){
		mUpdatePathTime = 0.0f;
		return true;
	} else {
		return false;
	}
}

////////////////////////////////////////////////////////////////////////////////
inline PlayerWorldContext &PlayerUnit::getWorldContext(void)
{
	return mWorldContext;
}

////////////////////////////////////////////////////////////////////////////////
inline PlayerUnit::ZombieUnitVec &PlayerUnit::getNearbyZombies(void)
{
	return mNearbyZombies;
}

////////////////////////////////////////////////////////////////////////////////
inline int PlayerUnit::getAttackingBehavior(void) const
{
	return mAttackingBehavior;
}

////////////////////////////////////////////////////////////////////////////////
inline void PlayerUnit::setAttackingBehavior(int b)
{
	mAttackingBehavior = b;
}


////////////////////////////////////////////////////////////////////////////////
inline PlayerStateMachine &PlayerUnit::getFSM(void)
{
	return mFSM;
}

////////////////////////////////////////////////////////////////////////////////
inline bool PlayerUnit::canAttackTarget(const GameUnit *t)
{
	ASSERT(t);
	ASSERT(getActualWeapon());
	return  (getActualWeapon()->getSqrRange() >
			getPosition().squaredDistance(t->getPosition()));
}

////////////////////////////////////////////////////////////////////////////////
inline void PlayerUnit::engageUnit(GameUnit *t)
{
	ASSERT(t);
	setActualTartet(t);
	mFSM.newEvent(E_ENGAGE_UNIT);
}

////////////////////////////////////////////////////////////////////////////////
inline CollectableObject *PlayerUnit::getTargetCollObject(void){
	return mTargetColObject;
}

////////////////////////////////////////////////////////////////////////////////
inline void PlayerUnit::setTargetCollObject(CollectableObject *newObj){
	mTargetColObject = newObj;
}

////////////////////////////////////////////////////////////////////////////////
inline void PlayerUnit::addBackPackItem(BackpackItem *item){
	ASSERT(item);
	mBackPack.addBackpackItem(item);
}

////////////////////////////////////////////////////////////////////////////////
inline void PlayerUnit::removeBackPackItem(BackpackItem *item){
	ASSERT(item);
	mBackPack.removeBackpackItem(item);
}

////////////////////////////////////////////////////////////////////////////////
inline Backpack *PlayerUnit::getBackpack(void)
{
	return &mBackPack;
}

////////////////////////////////////////////////////////////////////////////////
inline void PlayerUnit::createBackpackInterface(Weapon *e)
{
	// check the object not already exists in the backpack
	ASSERT(!mBackPack.hasBackpackItemUserDef(e));
	BackpackItem *bi = BackpackItemAssociator::createBackpackItem(e, this);
	ASSERT(bi);

	mBackPack.addBackpackItem(bi);
}

////////////////////////////////////////////////////////////////////////////////
inline void PlayerUnit::createBackpackInterface(Bomb *e)
{
	// check the object not already exists in the backpack
	ASSERT(!mBackPack.hasBackpackItemUserDef(e));
	BackpackItem *bi = BackpackItemAssociator::createBackpackItem(e, this);
	ASSERT(bi);
	mBackPack.addBackpackItem(bi);
}

////////////////////////////////////////////////////////////////////////////////
inline void PlayerUnit::createBackpackInterface(GameItem *e)
{
	// check the object not already exists in the backpack
	ASSERT(!mBackPack.hasBackpackItemUserDef(e));
	BackpackItem *bi = BackpackItemAssociator::createBackpackItem(e, this);
	ASSERT(bi);
	mBackPack.addBackpackItem(bi);
}

#endif /* PLAYERUNIT_H_ */
