/*
 * WPPathfinder.cpp
 *
 *  Created on: Aug 25, 2013
 *      Author: agustin
 */

#include "WPPathfinder.h"

#include <limits>

#include <debug/DebugUtil.h>

#include "WayPointGraph.h"

namespace gps {



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
WPPathfinder::WPPathfinder() :
    mNodeCont(0)
{

}
////////////////////////////////////////////////////////////////////////////////
WPPathfinder::~WPPathfinder()
{

}

////////////////////////////////////////////////////////////////////////////////
void
WPPathfinder::setGraph(WayPointGraph* graph)
{
    ASSERT(graph);
    mNodeCont = &graph->nodes();
    mAStar.setGraph(graph);
}

////////////////////////////////////////////////////////////////////////////////
bool
WPPathfinder::findPath(const core::Vector2& start,
                       const core::Vector2& goal,
                       WayPointPath& path)
{
    ASSERT(mNodeCont);

    // reset output data
    path.size = 0;

    // get the closest waypoints for the given positions
    const index_t startIndex = getClosestWayPoint(start);
    if (startIndex == INVALID_INDEX) {
        // no start waypoint found!
        return false;
    }
    const index_t goalIndex = getClosestWayPoint(goal);
    if (goalIndex == INVALID_INDEX) {
        // no goal waypoint found!
        return false;
    }

    ASSERT(startIndex < mNodeCont->size);
    ASSERT(goalIndex < mNodeCont->size);

    // there are the same waypoint? we can just return that
    if (startIndex == goalIndex) {
        path.size = 3;
        path.node[0] = start;
        path.node[0] = mNodeCont->data[startIndex].position;
        path.node[2] = goal;
        return true;
    }

    // get the path from the AStar algorithm... using an intermediate structure
    // since we need to invert the order of the path [goal,pn,...,p1,start] to
    // [start, p1,.., pn, goal].
    //
    WayPointPath tmpPath;
    const WayPointPath::Type ret = mAStar.getPath(startIndex, goalIndex, tmpPath);
    switch (ret) {
    case WayPointPath::Type::IMPOSSIBLE:
        // nothing to do!
        return false;
        break;

    case WayPointPath::Type::NORMAL:
    {
        // we will build the new path
        path.node[0] = start;
        path.size = 1;
        // copy in the reverse order
        for (int i = tmpPath.size-1; i >= 0; --i) {
            path.node[path.size] = tmpPath.node[i];
            ++(path.size);
        }
        path.node[path.size] = goal;
        ++(path.size);
    }
        break;

    case WayPointPath::Type::SAME_POINT:
        ASSERT(false); // this never could happen since we are checking this
                       // above
    default:
        // we are not contemplating this ?
        ASSERT(false);
    }


    // everything goes fine...
    ASSERT(path.size >= 2);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
index_t
WPPathfinder::getClosestWayPoint(const core::Vector2& pos) const
{
    ASSERT(mNodeCont);

    // TODO: improve this, we will iterate over all the nodes for now and get
    // the closest one, but this could be improved using space partition data
    // structures
    //
    float min = std::numeric_limits<float>::max();
    index_t result = INVALID_INDEX;

    for (index_t i = 0; i < mNodeCont->size; ++i) {
        const WayPointNode& node = mNodeCont->data[i];
        const float sqrDist = pos.squaredDistance(node.position);
        if (sqrDist < min) {
            min = sqrDist;
            result = i;
        }
    }

    return result;
}

} /* namespace gps */
