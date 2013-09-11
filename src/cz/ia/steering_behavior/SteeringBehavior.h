/*
 * SteeringBehavior.h
 *
 *  Created on: Sep 9, 2013
 *      Author: agustin
 */

#ifndef STEERINGBEHAVIOR_H_
#define STEERINGBEHAVIOR_H_


#include <math/Vec2.h>
#include <collisions/CollObject.h>


// forwards
//
namespace coll {
class CollisionHandler;
}

namespace cz {

// @brief This module will be used to handle the different steering behaviors
//        for the units in a more generic way.
//        We will also mix here some little code related with the collisions
//        to generate the associated forces to move the unit
//

namespace SteeringBehavior {

// @brief Calculate the repealing vector force for a given collision object and
//        its current maximum movement distance (its velocity).
// @param collHandler   The collision handler to be used.
// @param collObj       The current collision object we want to calculate its
//                      repealing force
// @param moveDir       The direction of the collObject (normalized).
// @param sqrRadius     The squared radius of the object.
// @param moveDist      The maximum movement distance for this unit, that probably
//                      it's equal to the velocity of its unit.
// @returns the associated repealing force that not necessarily is normalized
// @note We are assuming that collObj is a BB object.
//
core::Vector2
repealingForce(coll::CollisionHandler* collHandler,
               coll::CollObject* collObj,
               const core::Vector2& moveDir,
               float sqrRadius,
               float moveDist);

// @brief Calculate the seek force for a given object, velocity and
//        destination. This is an straightforward sample.
// @param position      The collision object position
// @param vel           Its velocity
// @param dest          The destination goal where we want to move
// @return the resulting force vector
//
inline core::Vector2
seekForce(const core::Vector2& position, float vel, const core::Vector2& dest)
{
    core::Vector2 result = dest - position;
    result.normalize();
    result *= vel;
    return result;
}


}

} /* namespace cz */
#endif /* STEERINGBEHAVIOR_H_ */
