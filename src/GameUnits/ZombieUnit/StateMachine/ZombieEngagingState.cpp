/*
 * ZombieEngagingState.cpp
 *
 *  Created on: 28/02/2012
 *      Author: agustin
 */

#include "ZombieEngagingState.h"
#include <iostream>
#include "CollisionTypedefs.h"
#ifdef DEBUG
#include "PrimitiveDrawer.h"
#endif
////////////////////////////////////////////////////////////////////////////////
void ZombieEngagingState::updatePath(ZombieUnit *ref)
{
	ASSERT(false);
}



////////////////////////////////////////////////////////////////////////////////
ZombieEngagingState::ZombieEngagingState()
{
}

////////////////////////////////////////////////////////////////////////////////
ZombieEngagingState::~ZombieEngagingState()
{
}



// Enter the state
void ZombieEngagingState::enter(ZombieUnit *ref)
{
//	debug("Entering Engage state\n");
	/* TODO:
	 * 1) Verificamos si estamos en grupo para armar la logica en base a eso
	 * 2) Si no armamos nosotros la logica
	 *
	 * Por el momento suponemos que estamos solos
	 */

	// We assume that we have a target
#ifdef DEBUG_CRITICAL
	if(!ref->getActualTarget()) {
		debug("Lost target while engaging.\n");
		return;
	}
#endif

	// check if we have a target to engage
	if(!ref->getActualTarget()){
		// TODO: tratamos de obtener una target nueva del grupo o lo que sea
		// si no encontramos una nueva... perdimos el target
		ref->getFSM().newEvent(ZombieUnit::E_TARGET_LOST);
		return;
	}

	// check if we have a target and is alive
	if(ref->getActualTarget()->getLife() < 0){
		// TODO: tratamos de buscar alguna nueva target?

		ref->setActualTartet(0);
		ref->getFSM().newEvent(ZombieUnit::E_TARGET_LOST);
		return;
	}

#ifdef DEBUG_CRITICAL
	debugWARNING("\nTODO: implementar el ataque grupal.\n"
			"Para ello hay que crear un nuevo estado \"SPREADING\","
			"previo a \"ATTACKING\".\n"
			"Se implementará un set global, que va acumulando "
			"todos los zombies avisados.\n"
			"Si el zombie entra a \"SPREADING\" por avistamiento de player,\n"
			"se marca a sí mismo como líder.\nSi entra a \"SPREADING\" "
			"porque le avisaron, no lo hace.\n"
			"Los zombies en \"SPREADING\" comienzan a hacer updates para\n"
			"ir dándose vuelta lentamente. Cuando terminan, pasan al estado\n"
			"\"ATTACKING\". Además, el líder emite gruñido grupal\n"
			"i.e. ref->playGroupGrunt()\n");
#endif
	// We will use the path assigned by the group or by us
	// assume that we can see the target
	ref->getUnitPath().clearPath();

	// set the running animation
	ref->changeAnimation(ZombieUnit::ANIM_RUN_BASE
				+ std::rand() % ANIM_RUN_COUNT);
	ref->getActualAnimation()->setLoop(true);
	ref->setVelocity(ref->getAttackVelocity());

	// There's some chance of playing group grunts
	if (std::rand()%100 < MOB_GRUNT_CHANCE) {
		const Ogre::String* mobGrunt(ref->mSounds.getSound(ZombieUnit::SS_MOB_GRUNT_CODE));
		if (mobGrunt) {
			ref->mSAPI.stop();
			ref->mSAPI.play(*mobGrunt);
		} else {
			debugWARNING("No zombie group grunt.%s", "\n");
		}
	}
}

// exit state
void ZombieEngagingState::exit(ZombieUnit *ref)
{
	// do nothing...?
}

// update state
void ZombieEngagingState::update(ZombieUnit *ref)
{
	GameUnit *target = ref->getActualTarget();

	// if have no more target then advise that we have lost the target?
	if(!target){
		ref->getFSM().newEvent(ZombieUnit::E_TARGET_LOST);
		return;
	}

	// first check if we are closer to the enemy, if we are, then attack to them
	if(ref->canAttackActualTarget()){
		ref->getFSM().newEvent(ZombieUnit::E_TARGET_CLOSE);
		return;
	}

	// here we can check if the target is TO FAR away and then emmit that
	// we have lost (TODO: usar la distancia calculada 1 sola vez)

	sm::Vector2 move, aux;
	// check the collision steering vector
	if(!ref->obtainCollisionObjects().empty()){
		ref->repellingSteerVec(move, ref->getCollideObjects());
	}

	// separate the 2 cases.. following a path or not
	if(ref->getUnitPath().isEmpty()){
		// we are moving just directly
		if(!ref->haveLineOfSightTo(target, ZOMBIE_MAX_VISIBILITY_SQRDIST)){
			// lost the target... calculate the possible path
			if(!ref->getPathTo(target->getPosition())){
				// we cannot move..
				ref->getFSM().newEvent(ZombieUnit::E_TARGET_LOST);
				debug("Target LOST!\n");
				return;
			} else {
				// we can move to that position
				//debugYELLOW("ref->getUnitPath().size(): %d\n",ref->getUnitPath().getPathSize());
//#ifdef DEBUG
//				for(int i = 0; i < ref->getUnitPath().getPathSize(); i++){
//					DRAWER.createNewVPoint(Ogre::Vector3(ref->getUnitPath().getPointPath()[i].x, 0, ref->getUnitPath().getPointPath()[i].y));
//				}
//#endif

				return;
			}
		} else {
			// we can see the enemy... go directly to the enemy
			ref->seekSteerVec(aux, target->getPosition());
			move += aux;
		}
	} else {
		// moving throw path
		if(ref->haveLineOfSightTo(target, ZOMBIE_MAX_VISIBILITY_SQRDIST)){
			// we see again the enemy... clear the path and follow directly
			ref->getUnitPath().clearPath();
			debug("Seen enemy again\n");
			return;
		}

		// TODO: aca podriamos ver si vemos otro enemigo?

		// else.. we cannot see the enemy... follow the path
		if(!ref->followPathSteerVec(aux)){
			// no more points... ASSERT that we can see the object...
			debug("We lost the enemy... Target lost?\n");
			ref->getFSM().newEvent(ZombieUnit::E_TARGET_LOST);
			return;
		}

		// If we are close to the target then we have to find a place around
		// it from where to attack:
		if( ref->distToActualTarget() < 4*ref->getSqrRadius()){
			aux *= 0.3;
			sm::Vector2 mv = aux;
			mv.transformToNormal();
			if(Ogre::Math::RangeRandom(0,1) != 0){
				mv.rotate180();
			}
			move += mv;
		}

		move += aux;

	}

	// move the unit
	ref->move(move);
}
