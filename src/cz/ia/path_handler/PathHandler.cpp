/*
 * PathHandler.cpp
 *
 *  Created on: Sep 9, 2013
 *      Author: agustin
 */

#include "PathHandler.h"

#include <pathfinding/WPPathfinder.h>

namespace cz {

const float PathHandler::CLOSE_DIST_FACTOR = 0.5f;
gps::WPPathfinder* PathHandler::sPathfinder = 0;


////////////////////////////////////////////////////////////////////////////////
PathHandler::PathHandler() :
    mCurrentIndex(0)
,   mCloseSqrDist(0)
{
    ASSERT(sPathfinder);
}

////////////////////////////////////////////////////////////////////////////////
PathHandler::~PathHandler()
{

}

////////////////////////////////////////////////////////////////////////////////
bool
PathHandler::calculatePath(const core::Vector2& start,
                           const core::Vector2& goal,
                           bool smooth)
{
    ASSERT(smooth == false); // TODO: not supported right now
    ASSERT(sPathfinder && "We should set this before constructing any of this object");

    // clear the current path
    clear();

    // get the path now
    if (!sPathfinder->findPath(start, goal, mPath)) {
        return false;
    }
    // else TODO: here we have to check if we need to do a spline or not
    // probably we want to use a local WaypointPath instead of mPath.
    //

    // we don't want to include [start] since we already are there..
    ++mCurrentIndex;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
PathHandler::getNextPoint(const core::Vector2& currentPos, core::Vector2& point)
{
    ASSERT(morePointsToGo()); // we need to ensure that we have more points to go

    // we will check the distance between currentPos and point and check if
    // we are close enough or not
    point = mPath.node[mCurrentIndex];
    const float sqrDistance = currentPos.squaredDistance(point);

    return sqrDistance < mCloseSqrDist;
}

} /* namespace cz */
