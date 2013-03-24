/*
 * ZombieUnit.cpp
 *
 *  Created on: 13/02/2012
 *      Author: agustin
 */

#include "ZombieUnit.h"
#include "ZombieDefs.h"
#include "SoundFamilyTable.h"

#include <SelectionSystem/SelectionType.h>
#include <Common/GlobalObjects/GlobalObjects.h>

ZombieSMTTable		*ZombieUnit::mSMTT = 0;
ZombieQueue			*ZombieUnit::mQueue = 0;
BillboardBatery		*ZombieUnit::mBloodBatery = 0;
BillboardManager	*ZombieUnit::mBillboardMngr = 0;

const float  ZombieUnit::MAX_RADIUS_VISIBILITY = ZOMBIE_MAX_VISIBILITY_DIST;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void ZombieUnit::loadBillboards(void)
{
	if(mBillboardMngr && mBillboardMngr->isCreated()){
		return;
	}

	mBillboardMngr = new BillboardManager;

	// create the billboard manager
	// ONLY 1 zombie at the same time we can select
	mBillboardMngr->createSet(getRadius(), ZOMBIE_BB_SEL_NAME, 2,
			ZOMBIE_BB_SEL_NUM);

	// set the size of the level (all the possible size...)
	// TODO: change this..?
	mBillboardMngr->setBounds(Ogre::AxisAlignedBox(
			Ogre::Vector3(-9999.9f,-9999.9f,-9999.9f),
			Ogre::Vector3(999999.9f,999999.9f,999999.9f)),999999.9f);
}



////////////////////////////////////////////////////////////////////////////////
ZombieUnit::ZombieUnit() :
		mFSM(this),
		mSoundTarget(0),
		mSpreader(0),
		mSelBillboard(0)
{
	ASSERT(mBloodBatery);
	ASSERT(mQueue);
	ASSERT(mSMTT);	// we must set this first
	mFSM.setTransitionTable(mSMTT);


	/************************************************************************
	 ** TODO  ¡Quitar este hardcodeo de acá!
	 **
	 ** Deberíamos estar cargando estos nombres desde un XML,
	 ** diferenciando el caso zombie normal / zombie perro.
	 **/
	Ogre::String soundsGrunt[7] = {"fxZ1.ogg", "fxZ2.ogg", "fxZ3.ogg",
								   "fxZ4.ogg", "fxZ5.ogg", "fxZ6.ogg",
								   "fxZ7.ogg"};
	Ogre::String soundsMobGrunt[1] = {"fxZ8.ogg"};
	Ogre::String soundsHit[2] = {"fxZ9.ogg", "fxZ10.ogg"};
	Ogre::String soundsDying[1] = {""};  // i.e. silence
	mSounds.addSounds(ZombieUnit::SS_GRUNT_CODE, soundsGrunt, 7);
	mSounds.addSounds(ZombieUnit::SS_MOB_GRUNT_CODE, soundsMobGrunt, 1);
	mSounds.addSounds(ZombieUnit::SS_HIT_CODE, soundsHit, 2);
	mSounds.addSounds(ZombieUnit::SS_DYING_CODE, soundsDying, 1);
	/**
	 ** TODO  ¡Quitar ese hardcodeo de acá!
	 ************************************************************************/

	mQueue->addNewObject(this);

	// Set the Selectable Type of this object
	setType(selection::Type::SEL_TYPE_ZOMBIE);
}

////////////////////////////////////////////////////////////////////////////////
ZombieUnit::~ZombieUnit()
{
	if(mBillboardMngr){
		if(mSelBillboard){
			mBillboardMngr->letAvailable(mSelBillboard);
			mSelBillboard = 0;
		}
		delete mBillboardMngr;
		mBillboardMngr = 0;
	}
}


//////			FUNCTION INHERITED				///////

////////////////////////////////////////////////////////////////////////////////
void ZombieUnit::objectSelected(void)
{
	if(mSelBillboard){
		// actually selected, change the atlas
		mBillboardMngr->changeAtlas(mSelBillboard, ZOMBIE_BB_SELECTION);
		return;
	}

	// get new billboard
	mSelBillboard = mBillboardMngr->getNewBillboard(ZOMBIE_BB_SELECTION);
}

////////////////////////////////////////////////////////////////////////////////
void ZombieUnit::objectUnselected(void)
{
	if(mSelBillboard){
		mBillboardMngr->letAvailable(mSelBillboard);
		mSelBillboard = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////
void ZombieUnit::mouseOverObject(void)
{
	// if is actually selected, we do nothing?
	if(mSelBillboard) return;
	// else we get new billboard and use it
	mSelBillboard = mBillboardMngr->getNewBillboard(ZOMBIE_BB_MOUSE_OVER);
}

////////////////////////////////////////////////////////////////////////////////
void ZombieUnit::mouseExitObject(void)
{
	// if we are already selected then we do nothing
	if(!mSelBillboard){
		return;
	}
	if(mBillboardMngr->getAtlas(mSelBillboard) == ZOMBIE_BB_SELECTION){
		// do nothing
		return;
	} else {
		// we are in "mouse over".. so we clear this billboard
		mBillboardMngr->letAvailable(mSelBillboard);
		mSelBillboard = 0;
	}

}

////////////////////////////////////////////////////////////////////////////////
void ZombieUnit::beenHit(const Hit_t &hit)
{
	mLastHit = hit;
	// decrease the life
	setLife(getLife() - hit.power);
	// emmit a new event
	if(getLife() <= 0){
		mFSM.newEvent(E_DYING);
		return;
	}
	// else the zombie has been hit
	debug("Zombie been hit: %d\n", getLife());
	mFSM.newEvent(E_BEEN_HIT);
}

/* Function called when the unit is getting alive. This function
 * automatically reset the StateMachine
 */
void ZombieUnit::borning(void)
{
	// TODO:
	setActiveCollision(true);
}

////////////////////////////////////////////////////////////////////////////////
void ZombieUnit::dead(void)
{
	// TODO: aca deberiamos limpiar la unidad, volverla algun contenedor..

	// deactivate the collision
	setActiveCollision(false);

	// return to the container
	mQueue->addNewObject(this);

	// add a new blood in this position
	mBloodBatery->showBillboard(mNode->getPosition());
}

/**
 * Function used to build the GameUnit, this will load all the animations
 * and all the necessary things to get work property the class
 */
bool ZombieUnit::build(void)
{
	// load the animations of the entity
	std::vector<Ogre::String> anims;

	anims.push_back(ZOMB_ANIM_IDLE);
	anims.push_back(ZOMB_ANIM_IDLE_2);
	anims.push_back(ZOMB_ANIM_WALK);
	anims.push_back(ZOMB_ANIM_WALK_2);
	anims.push_back(ZOMB_ANIM_RUN);
	anims.push_back(ZOMB_ANIM_ATTACK);
	anims.push_back(ZOMB_ANIM_ATTACK_2);
	anims.push_back(ZOMB_ANIM_DIE_L);
	anims.push_back(ZOMB_ANIM_DIE_R);
	anims.push_back(ZOMB_ANIM_DIE_B);
	anims.push_back(ZOMB_ANIM_DIE_F);
	anims.push_back(ZOMB_ANIM_HIT_F);
	anims.push_back(ZOMB_ANIM_HIT_B);
	anims.push_back(ZOMB_ANIM_SPREAD);
	anims.push_back(ZOMB_ANIM_SPREAD_2);

	loadAnimations(anims);

	// Attach unit's SceneNode to the SoundAPI
	ASSERT(getSceneNode() != 0);
	configureSoundAPI();

	// config the bounding box
	float w,h;
	getAABBFromEntity(w,h);
	configCollObj(w,h, COL_FLAG_UNIT_ZOMBIE, COL_GRFLAG_UNIT_ZOMBIE);

	// configure the unit path
	configureUnitPath();

	// create the billboards for every unit
	loadBillboards();

	// Start the FSM
	mFSM.start();

	return true;
}

/**
 * Function used to advise the unit for a certain new event
 */
void ZombieUnit::newExternalEvent(SMEvent e)
{
	// TODO:
}

// Update the unit:
// TODO: probablemente no haga falta hacerla virtual a esta funcion si no
// que simplemente haciendo mActualAnim->addTime(GLOBAL_TIME); y
// mStateMachine.update(); alcance.... nos ahorramos una virtual call.
void ZombieUnit::update(void)
{
	mActualAnim->addTime(GLOBAL_TIME_FRAME * mAnimAccelCoef);
	mFSM.update();

	// if we have billboard, update the position
	if(mSelBillboard){
		mSelBillboard->setPosition(getPosition().x, getPosYAxis(), getPosition().y);
		// TODO: probablemente queremos rotarlo?
	}

}

////////////////////////////////////////////////////////////////////////////////
void ZombieUnit::unitClose(GameUnit *unit)
{
	// check that the unit that is close is a player or a civil
	ASSERT(unit);
	ASSERT(unit->getCollisionObject().maskFlag &
			(COL_FLAG_UNIT_PLAYER | COL_FLAG_UNIT_CIVIL));

	mTargets.insert(unit);
}

///////						Function of this class					//////
////////////////////////////////////////////////////////////////////////////////
void ZombieUnit::setSMTransitionTable(ZombieSMTTable *tt)
{
	ASSERT(tt);
	mSMTT = tt;
}
ZombieSMTTable *ZombieUnit::getSMTransitionTable(void)
{
	return mSMTT;
}



////////////////////////////////////////////////////////////////////////////////
void ZombieUnit::setBillboardBBlood(BillboardBatery *bbb)
{
	ASSERT(bbb);
	mBloodBatery = bbb;
}

////////////////////////////////////////////////////////////////////////////////
void ZombieUnit::setQueue(ZombieQueue *q)
{
	ASSERT(q);
	mQueue = q;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Function used to obtain all the zombies that are closer to this
 * Zombie.
 * The result is put in the mNearbyZombies;
 * @return The mNearbyZombies vector.
 * The second funcion (getNearbyZombies) do not perform any operation,
 * only returns the last result obtained in findNearbyZombies()
 */
ZombieUnit::ZombieUnitVec &ZombieUnit::findNearbyZombies(float range)
{
	mNearbyZombies.clear();
	getNearbyObjects(range, range, static_cast<mask_t>(COL_FLAG_UNIT_ZOMBIE),
			reinterpret_cast<std::vector<GameObject *> &>(mNearbyZombies));
	// TODO:
}


////////////////////////////////////////////////////////////////////////////////
bool ZombieUnit::seeTarges(std::vector<GameUnit *> &targets)
{
	if(mTargets.empty()) return false;


	if(!targets.empty()) targets.clear();

	// returns all the targets that the zombie see
	TargetList::iterator it = mTargets.begin(),
			endIt = mTargets.end();

	for(; it != endIt; ++it){
		if(haveLineOfSightTo(*it, PLAYER_MAX_VISIBILITY_SQRDIST)){
			targets.push_back(*it);
		}
	}

	// we see targets if and only if we have targets
	return !targets.empty();
}



////////////////////////////////////////////////////////////////////////////////
void ZombieUnit::followSound(const Ogre::SceneNode * target)
{
	mSoundTarget = target;
	mFSM.newEvent(ZombieUnit::E_FOLLOW_SOUND);

}

