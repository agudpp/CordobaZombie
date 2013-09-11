/*
 * ZSRunning.cpp
 *
 *  Created on: Sep 9, 2013
 *      Author: agustin
 */

#include "ZSRunning.h"

#include <OgreAnimationState.h>

#include <zombie_unit/ZombieUnit.h>
#include <debug/DebugUtil.h>
#include <ia/steering_behavior/SteeringBehavior.h>
#include <ia/path_handler/PathHandler.h>
#include <global_data/GlobalData.h>

namespace cz {


void
ZSRunning::enter(ZombieUnit* ref)
{
    ASSERT(ref);

    // start the animation
    if (ref->isAnimActive(ZombieUnit::Anim::ZA_RUN1)) {
        // nothing to do
        return;
    }
    Ogre::AnimationState* anim = ref->animState(ZombieUnit::Anim::ZA_RUN1);
    anim->setLoop(true);
    ref->startAnim(ZombieUnit::Anim::ZA_RUN1);
}

void
ZSRunning::update(ZombieUnit* ref)
{
    ASSERT(ref);
    PathHandler& ph = ref->pathHandler();

    // check if we still have more points to go
    if (!ph.morePointsToGo()) {
        // no.
        /*debugWARNING("What we should do here? Kill the player? the game ends?\n");
        // for now we want to kill the zombie.
        ref->newEvent(ZombieEvent::ZE_DONE);*/
        return;
    }

    // check if we are close enough to the next point we have to move
    //
    const core::Vector2& currentPos = ref->position();
    core::Vector2 auxVec;
    if (ph.getNextPoint(currentPos, auxVec)) {
        // yes we are, skip this point and return to do it in the next iteration
        ph.removeCurrent();
        return;
    }

    // calculate the movement advance vector
    core::Vector2 forceVec =
        cz::SteeringBehavior::seekForce(ref->position(),
                                        ref->velocity(),
                                        auxVec);

    // calculate the collision steering behavior
    auxVec = cz::SteeringBehavior::repealingForce(GlobalData::collHandler,
                                                  ref->collisionObjectPtr(),
                                                  ref->getNormalizedDir(),
                                                  ref->sqrRadius(),
                                                  ref->velocity());

    // move the unit the corresponding size
    forceVec += auxVec;
    forceVec.truncate(ref->velocity());
    // TODO: clamp here to max vel
    ref->moveAndLook(forceVec);
}

void
ZSRunning::exit(ZombieUnit* ref)
{
    ASSERT(ref);
}

} /* namespace cz */
