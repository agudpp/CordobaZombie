/*
 * SteeringBehavior.cpp
 *
 *  Created on: Sep 9, 2013
 *      Author: agustin
 */

#include "SteeringBehavior.h"

#include <cmath>

#include <debug/DebugUtil.h>
#include <collisions/CollisionHandler.h>


// helper methods
//
namespace {

// Helper class representing an object / agent in the world
//
struct SObject {
    core::Vector2 position;
    float sqrRadius;
    bool isStatic;
};

// @brief Calculate the repealing force for given two objects, the agent and
//        the obstacle.
// @param agent     The agent object
// @param obstacle  The obstacle
// @param maxForce  The max force length to be applied (in the case of
//                  static objects or when we are just inside the other object).
// @param result    The resulting force
inline void
calcRepealingForce(const SObject& agent,
                   const SObject& obstacle,
                   const float maxForce,
                   core::Vector2& result)
{
    result = agent.position - obstacle.position;
    const float sqrDistance = result.squaredLength();

    // now we will do as next:
    // - If sqrDistance is greater than the sum of the radius then they are
    //   not colliding.. skip this
    // - If the distance is less than some of the radius of the objects then
    //   apply the max force since we are inside of the object.
    // - If non of the above cases occur, then we are overlapping and we need
    //   to calculate how much we need to repel. To do this we will calculate
    //   the overlapping distance and make a factor over the total max distance
    //   and multiply by maxForce. This way if the overlapping distance is
    //   bigger, we will get more repealing force, if less, then minus force.
    //
    // As the last step we will check if the object is a dynamic or static one.
    // and multiply it by a factor (only if it is static). This way we will avoid
    // being inside of static objects (this factor will be used to have more
    // weight when comparing with dynamic collisions).
    //

    ASSERT(agent.sqrRadius > 0.f);
    ASSERT(obstacle.sqrRadius > 0.f);

    // 1)
    if (sqrDistance > (agent.sqrRadius + obstacle.sqrRadius)) {
        // nothing to do.
        result.x = result.y = 0.f;
        return;
    }

    // 2)
    if (sqrDistance < agent.sqrRadius || sqrDistance < obstacle.sqrRadius) {
        result.normalize();
        result *= maxForce;
        return;
    }

    // 3)
    // calculate the overlapping distance
    // ovSqrDist = distance - ((distance - r2) + (distance - r1))
    //           = r2 - distance + r1
    // this delta represent the intersection distance between the two radius
    // but is squared...
    //
    const float ovSqrDist = std::abs(agent.sqrRadius + obstacle.sqrRadius - sqrDistance);
    ASSERT(ovSqrDist <= sqrDistance);
    const float factor = ovSqrDist / sqrDistance;
    result.normalize();
    result *= factor * maxForce;
}

// @brief Calculate the distance between a shape

// @brief Calculate the repealing vector for a given object of radius r and
//        another object of BB bb.
// @param center    The center position of the object 1
// @param radius    The radius (squared) of the object 1
// @param bb        The bounding box of the second object.
// @param maxLen    The maximum force length
// @param result    The resulting steering vector
//
inline void
getRepealingForce(const core::Vector2& center,
                  const float radius,
                  const core::AABB& bb,
                  const float maxLen,
                  core::Vector2& result)
{
    // get radius of the bb
    float r2 = bb.calculateSquaredRadius();

    // calculate distance between center 1 and center 2
    result = center - bb.center();
    const float distance = result.squaredLength();
    result.normalize();

    // calculate the percentage depending how much we are inside of object 1
    // delta = distance - ((distance - r2) + (distance - r1))
    //       = r2 - distance + r1
    // this delta represent the intersection distance between the two radius
    // but is squared... we will use all the values in squared mode so it
    // should be fine.
    const float len = std::abs(r2 - distance - radius);
    result *= (len > maxLen) ? maxLen : len;
}

// @brief Get the repealing force for a point (intersection point) and the
//        object and its radius
// @param center    The center of the object
// @param sqrRad    The squared radius of the object
// @param collPoint The collision point in world coordinates.
//
inline void
getRepealingForce(const core::Vector2& center,
                  const float sqrRad,
                  const core::Vector2& collPoint,
                  core::Vector2& result)
{
    // TODO: improve this probably we need more information than this
    result = center - collPoint;
    result.normalize();
    result *= (sqrRad * 0.5f);
}

}

namespace cz {

namespace SteeringBehavior {

////////////////////////////////////////////////////////////////////////////////
core::Vector2
repealingForce(coll::CollisionHandler* collHandler,
               coll::CollObject* collObj,
               const core::Vector2& moveDir,
               float sqrRadius,
               float moveDist)
{
    ASSERT(collHandler);
    ASSERT(collObj);
    ASSERT(moveDist > 0);
    ASSERT(sqrRadius > 0);


    // What we gonna do:
    // 1) Get all the AABB collisions for the given object with static and
    //    dynamic objects.
    // 2) For all the objects that don't contain precise info we will calculate
    //    the force directly from the center of the object, and add this force
    //    to the resulting force taking into account the distance of the object
    //    and the sqrRadius.
    // 3) For all objects with precise info, we will calculate the intersection
    //    points and analyze each point separately (as if they were AABB objects).
    //
    static coll::QueryResult qr;
    coll::QueryArgs args; // default values
    core::Vector2 result(0,0);

    // we will check for all the objects that are in front of the object
    //
    if (!collHandler->performQuery(collObj, args, qr)) {
        return result;
    }

    // we have collisions, configure the agent
    SObject agent;
    const float agentRadius = collObj->boundingBox().calculateRadius();
    agent.position = collObj->center();
    agent.sqrRadius = collObj->boundingBox().calculateSquaredRadius();
    agent.isStatic = false;

    // check for that collisions
    coll::QueryResultVec& objs = qr.objects;
    coll::CollPointVec pointsVec;
    core::Vector2 auxVec;
    SObject obstacle;
    const float maxStaticRepealingForce = moveDist * 5;

    for (coll::CollObject** beg = objs.begin(), **end = objs.end(); beg != end;
            ++beg) {
        ASSERT(*beg);
        coll::CollObject* other = *beg;
        obstacle.position = other->center();
        obstacle.isStatic = other->isStatic();

        // now we will check/obtain the detailed information if and only if
        // the object contain precise information
        if (other->preciseInfo()) {
            // What we will do here is a little tricky. We will get the closest
            // distance between this object (obstacle) and the agent, this way
            // we can get the point that is closest to the agent, and that way
            // we can set the radius of the obstacle as if it where that point.
            // Note that this way we are always putting a new radius to the same
            // object bat this is not important because we are calculating the
            // avoidance vector on the fly.
            //
            core::Vector2 closestA, dummyVec;
            float distance;
            other->closestPoint(agent.position,
                                agentRadius,
                                closestA,
                                dummyVec,
                                distance);

            // get the radius of the obstacle?
            dummyVec = obstacle.position + closestA;
            obstacle.sqrRadius = dummyVec.squaredLength();

            // now calculate the repealing force
            calcRepealingForce(agent,
                               obstacle,
                               moveDist,
                               auxVec);
            result += auxVec;

            /*// TODO: for now we will just test for overlapping and we will create
            //       a repealing vector to move the object outside of the
            //       collision object. This should be improved for other
            //       type of objects
            //       Note that we are also assuming that objects with precise
            //       information are static! and that is not right!
            //
            if (collObj->collidePrecise(*other)) {
                // we will repeal with the the full force for now.
                core::Vector2 repForce = center - other->center();
                repForce.normalize();
                repForce *= maxStaticRepealingForce;
                result += repForce;
            }*/

            // contain precise information, we need to get the points
            /*if (collObj->collidePoints(*other, pointsVec)) {
                // iterate over all the points and add them
                for (core::Vector2* beg = pointsVec.begin(), *end = pointsVec.end();
                        beg != end; ++beg) {
                    getRepealingForce(center, collRadius, *beg, auxVec);
                    result += auxVec;
                }
            }*/
        } else {
            // set the radius for this obstacle
            obstacle.sqrRadius = other->boundingBox().calculateSquaredRadius();

            // is just a bb.
            calcRepealingForce(agent,
                               obstacle,
                               moveDist,
                               auxVec);
            result += auxVec;
        }
    }

    return result;
}

}

} /* namespace cz */
