/*
 * ZSDying.cpp
 *
 *  Created on: Nov 5, 2013
 *      Author: agustin
 */

#include "ZSDying.h"

#include <zombie_unit/ZombieUnit.h>
#include <zombie_unit/ZombieBody.h>

namespace cz {

////////////////////////////////////////////////////////////////////////////////
ZSDying::ZSDying() :
    mIsRagdollDead(false)
{
}
////////////////////////////////////////////////////////////////////////////////
ZSDying::~ZSDying()
{
}

////////////////////////////////////////////////////////////////////////////////
void
ZSDying::enter(ZombieUnit* ref)
{
    ASSERT(ref);
    ASSERT(ref->hasLife() == false);

    // we will check if we have to reproduce an animation or wait until the
    // ragdoll finish its animation
    //
    mIsRagdollDead = ref->zombieBody().isRagdollEnabled();

    if (!mIsRagdollDead) {
        // then we have to get which is the associated animation
        ASSERT(false && "This is not possible, since we always will die with the "
            "ragdoll!!!!");
    }

}

////////////////////////////////////////////////////////////////////////////////
void
ZSDying::update(ZombieUnit* ref)
{
    ASSERT(ref);

    // we have to check until the ragdoll is not valid anymore, and of course
    // we have to update it every time
    ASSERT(mIsRagdollDead);

    ZombieBody& body = ref->zombieBody();

    if (!body.update()) {
        // the ragdoll has ended
        body.setRagdollEnable(false);

        // call the dead method to let the zombie unavailable again
        ref->dead();
    }

    // else we have to continue waiting
}

////////////////////////////////////////////////////////////////////////////////
void
ZSDying::exit(ZombieUnit* ref)
{
    ASSERT(ref);

    // nothing to do!!
}

} /* namespace cz */
