/*
 * WPPathfinder.h
 *
 *  Created on: Aug 25, 2013
 *      Author: agustin
 */

#ifndef WPPATHFINDER_H_
#define WPPATHFINDER_H_

#include <math/Vec2.h>

#include "WayPointDefines.h"
#include "WayPointGraph.h"
#include "WayPointAStar.h"

namespace gps {

// @brief This class will be in charge of handling the path finding given
//        two different positions in the world. We will use waypoints to get from
//        the starting point to the goal through waypoints only (we will not
//        check for open spaces nor anything related with that).
//


class WPPathfinder
{
public:
    WPPathfinder();
    ~WPPathfinder();

    // @brief Set the WayPointGraph to be used.
    // @param graph     The graph
    // @note  You MUST maintain the graph in memory until you delete this class.
    //
    void
    setGraph(WayPointGraph* graph);

    // @brief Get the path from the starting point (inclusive) to the goal point
    //        (inclusive). This will give the path [start, p1, p2, p3,.., goal],
    //        where pi : i >= 1, are the intermediate points through the graph
    //        In the case that no path is found, we return false and an empty
    //        path (size == 0).
    //        In the case that we only contain [start, goal] this means that we
    //        can go directly to the position without problem
    // @param start     The starting position
    // @param goal      The position where we want to go (end).
    // @param path      The resulting path.
    // @return true if a path is found | false otherwise
    //
    bool
    findPath(const core::Vector2& start,
             const core::Vector2& goal,
             WayPointPath& path);

    // @brief Helper methods to get the most closest waypoint of the graph for
    //        a given position. We will return the index of the node since
    //        is the id of the node.
    // @param pos       The position we want to check
    // @return the index of the closest waypoint (without taking into account
    //         if we can go directly to them or not... we will not check for
    //         collisions). INVALID_INDEX is returned if no waypoint is found.
    //
    index_t
    getClosestWayPoint(const core::Vector2& pos) const;

private:
    const WPNodeCont* mNodeCont;
    WayPointAStar mAStar;
};

} /* namespace gps */
#endif /* WPPATHFINDER_H_ */
