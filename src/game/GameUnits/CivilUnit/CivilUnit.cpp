/*
 * CivilUnit.cpp
 *
 *  Created on: 12/07/2012
 *      Author: agustin
 */

#include "CivilUnit.h"

#include <CollisionSystem/CollisionTypedefs.h>
#include <SelectionSystem/SelectionType.h>

#include "GameUnitDefines.h"



CivilUnit::GameUnitVec				CivilUnit::mNearbyZombies;
CivilSMTTable						*CivilUnit::mSMTTable = 0;


const float  CivilUnit::SAFE_SQR_DIST = 15.0f*15.0f;


////////////////////////////////////////////////////////////////////////////////
void CivilUnit::setSMTTable(CivilSMTTable *t)
{
	ASSERT(t);
	mSMTTable = t;
}

////////////////////////////////////////////////////////////////////////////////
void CivilUnit::adviseNearbyZombies(void)
{
	// check all the objects that are close
	getNearbyObjects(ZOMBIE_MAX_VISIBILITY_DIST, ZOMBIE_MAX_VISIBILITY_DIST,
			c_p::COL_FLAG_UNIT_ZOMBIE,
			reinterpret_cast<std::vector<GameObject *> &>(mNearbyZombies));

	// now for every object we put it into the nearbyZombies
	GameUnit *gu;
	for(int i = mNearbyZombies.size()-1; i >= 0; --i ){
		gu = static_cast<GameUnit *>(mNearbyZombies[i]);
		// advise the zu that we are closer
		gu->unitClose(this);
	}
}




CivilUnit::CivilUnit() :
		mFollowingUnit(0),
		mFSM(this)
{
	// by default we use the mSMTT
	ASSERT(mSMTTable);	// we must set this first
	mFSM.setTransitionTable(mSMTTable);

	// set the type of this unit
	setType(selection::Type::SEL_TYPE_CIVIL);
}

CivilUnit::~CivilUnit()
{
	// TODO Auto-generated destructor stub
}

////////////////////////////////////////////////////////////////////////////////
void CivilUnit::objectSelected(void)
{
	ASSERT(false);
	// TODO:
}

////////////////////////////////////////////////////////////////////////////////
void CivilUnit::objectUnselected(void)
{
	ASSERT(false);
	// TODO:
}

////////////////////////////////////////////////////////////////////////////////
void CivilUnit::mouseOverObject(void)
{
	ASSERT(false);
	// tODO?
}

////////////////////////////////////////////////////////////////////////////////
void CivilUnit::mouseExitObject(void)
{
	ASSERT(false);
	// tODO?
}

////////////////////////////////////////////////////////////////////////////////
void CivilUnit::beenHit(const Hit_t &hit)
{
	mLastHit = hit;
	// decrease the life
	setLife(getLife() - hit.power);
	// emmit a new event
	debug("Civil been hit: %d\n", getLife());
	mFSM.newEvent(E_BEEN_HIT);
}

////////////////////////////////////////////////////////////////////////////////
void CivilUnit::borning(void)
{
	setActiveCollision(true);
}

////////////////////////////////////////////////////////////////////////////////
void CivilUnit::dead(void)
{
	debugWARNING("TODO: Que hacemos si se muere un civil?, perdemos el objetivo?"
				" Deberiamos poder avisar al MissionManager o algo asi de alguna "
				"forma que un civil murio\n");

	// avoid collision detection after die
	setActiveCollision(false);

}

////////////////////////////////////////////////////////////////////////////////
bool CivilUnit::build(void)
{

	// load the animations of the entity
	std::vector<Ogre::String> anims;

	anims.push_back(CIVIL_ANIM_IDLE);
	anims.push_back(CIVIL_ANIM_IDLE2);
	anims.push_back(CIVIL_ANIM_IDLE3);
	anims.push_back(CIVIL_ANIM_RUN);
	anims.push_back(CIVIL_ANIM_RUN2);
	anims.push_back(CIVIL_ANIM_RUN_ESCAPE);
	anims.push_back(CIVIL_ANIM_DIE_L);
	anims.push_back(CIVIL_ANIM_DIE_R);
	anims.push_back(CIVIL_ANIM_DIE_B);
	anims.push_back(CIVIL_ANIM_DIE_F);
	anims.push_back(CIVIL_ANIM_HIT_F);
	anims.push_back(CIVIL_ANIM_HIT_B);

	loadAnimations(anims);

	// Attach unit's SceneNode to the SoundAPI
	ASSERT(getSceneNode() != 0);
	configureSoundAPI();

	// config the bounding box
	float w,h;
	getAABBFromEntity(w,h);
	configCollObj(w,h, c_p::COL_FLAG_UNIT_CIVIL, c_p::COL_GRFLAG_UNIT_CIVIL);

	// configure the unit path
	configureUnitPath();

	// Start the FSM
	mFSM.start();


	return true;

}

////////////////////////////////////////////////////////////////////////////////
void CivilUnit::unitClose(GameUnit *unit)
{
	// we don't want to use this function
	ASSERT(false);
}

////////////////////////////////////////////////////////////////////////////////
void CivilUnit::newExternalEvent(SMEvent e)
{
	mFSM.newEvent(e);
}

////////////////////////////////////////////////////////////////////////////////
void CivilUnit::update(void)
{
	// advise all the zombies closer to this unit
	adviseNearbyZombies();

	// update the actual animation
	mActualAnim->addTime(GLOBAL_TIME_FRAME * mAnimAccelCoef);
	mFSM.update();
}


////////////////////////////////////////////////////////////////////////////
///						Function of this class							////
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void CivilUnit::followUnit(GameUnit *gu)
{
	ASSERT(gu);
	mFollowingUnit = gu;
	mFSM.newEvent(E_FOLLOW_UNIT);
}
