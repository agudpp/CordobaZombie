/*
 * ZombieIdleState.cpp
 *
 *  Created on: 22/02/2012
 *      Author: agustin
 */

#include <OgreMath.h>
#include "GlobalObjects.h"
#include "ZombieIdleState.h"
#ifdef DEBUG
#include "PrimitiveDrawer.h"
#endif


////////////////////////////////////////////////////////////////////////////////
void ZombieIdleState::configDoingNothingState(ZombieUnit *ref)
{
//	debug("Doing Nothing\n");
	ref->changeAnimation(ZombieUnit::ANIM_IDLE_BASE
				+ std::rand() % ANIM_IDLE_COUNT);
}

////////////////////////////////////////////////////////////////////////////////
void ZombieIdleState::configWalkingAroundState(ZombieUnit *ref)
{
//	debug("Walking Around\n");
	int numNodes = std::rand() % (RND_NUM_NODES_MAX - RND_NUM_NODES_MIN);
	numNodes += RND_NUM_NODES_MIN;

	// We will get a random path and set it to the zombie unit
	if(PATHFINDER_INSTANCE.getRandomPath(ref->getPosition(),
			ref->getPath(), 0.0f, numNodes) != PathfindingManager::NORMAL_PATH){
		// mmm some problem?
		configDoingNothingState(ref);
		return;
	}
//#ifdef DEBUG
//	static DrawablePath dp;
//	dp.clearPath();
//	dp.drawPath(ref->getPath());
//#endif

	ref->restartPath();

	// set the walking anim

	ref->changeAnimation(ZombieUnit::ANIM_WALK_BASE
				+ std::rand() % ANIM_WALK_COUNT);
	ref->getActualAnimation()->setLoop(true);
	ref->setVelocity(ref->getWalkVelocity());
}



ZombieIdleState::ZombieIdleState()
{
	// TODO Auto-generated constructor stub

}

ZombieIdleState::~ZombieIdleState()
{
	// TODO Auto-generated destructor stub
}


// Enter the state
void ZombieIdleState::enter(ZombieUnit *ref)
{
	getRandomChoice(ref);
}

// exit state
void ZombieIdleState::exit(ZombieUnit *ref)
{
	// clear the path
	ref->getPath().clear();
	ref->restartPath();
}

// update state
void ZombieIdleState::update(ZombieUnit *ref)
{
	static std::vector<GameUnit *> targets;

	// first of all we have to check if we can see an enemy
	if(ref->seeTarges(targets)){
		// TODO: aca decidimos cual es la mejor target para este zombie..
		// probablemente podemos poner una funcion en la clase zombie
		// que dada una lista de targets devuelva cual es la mejor

		// By the way we will set first target
		ref->setActualTartet(targets[0]);

		// There's some chance of making noise
		if (std::rand()%100 < GRUNT_CHANCE) {
			const Ogre::String* grunt(ref->mSounds.getRandomSound(ZombieUnit::SS_GRUNT_CODE));
			if (grunt) {
				ref->mSAPI.play(*grunt);
			} else {
				debugWARNING("No zombie grunt sounds loaded.%s", "\n");
			}
		}

		// we now inform the event and return
		ref->getFSM().newEvent(ZombieUnit::E_PERCEIVE_TARGET);
		return;
	}


	// depending on the actual anim we check what to do.
	if(ZombieUnit::ANIM_IDLE_BASE <= ref->getActualAnimID() &&
			ref->getActualAnimID() < ZombieUnit::ANIM_IDLE_BASE +
			ANIM_IDLE_COUNT){
		// idle
		if(ref->hasActualAnimEnd()){
			// animation end... we have to do a random choice again
			getRandomChoice(ref);
			// There's some chance of making noise
			if (std::rand()%100 < GRUNT_CHANCE) {
				const Ogre::String* grunt(ref->mSounds.getRandomSound(ZombieUnit::SS_GRUNT_CODE));
				if (grunt) {
					ref->mSAPI.play(*grunt);
				} else {
					debugWARNING("No zombie grunt sounds loaded.%s", "\n");
				}
			}
			return;
		}
		// else: nothing to do! xD

	} else {
		// walking around
		sm::Vector2 trans;

		if(!ref->followPathSteerVec(trans)){
			// we have no more pathPoints.. random choice
			getRandomChoice(ref);
			// There's some chance of making noise
			if (std::rand()%100 < GRUNT_CHANCE) {
				const Ogre::String* grunt(ref->mSounds.getRandomSound(ZombieUnit::SS_GRUNT_CODE));
				if (grunt) {
					ref->mSAPI.play(*grunt);
				} else {
					debugWARNING("No zombie grunt sounds loaded.%s", "\n");
				}
			}
			return;
		}

		// else we have to continue moving
		// perform the collision detection
		if(!ref->obtainCollisionObjects().empty()){
			// we collide
			sm::Vector2 avoidVec;
			ref->repellingSteerVec(avoidVec, ref->getCollideObjects());
			trans += avoidVec;
		}

		trans.normalize();
		trans *= ref->getVelocity();

		ref->move(trans);
	}
}
