/*
 * WayPointAStar.h
 *
 *  Created on: Aug 24, 2013
 *      Author: agustin
 */

#ifndef WAYPOINTASTAR_H_
#define WAYPOINTASTAR_H_

#include <types/BoolCountingMask.h>

#include "WayPointDefines.h"


namespace gps {

// Forward
//
class WayPointGraph;
class WayPointNode;

class WayPointAStar
{
    // we will define the maximum number of nodes that we can visit before we
    // get to the goal.
    //
    static const unsigned int MAX_NUM_NODES_TO_VISIT = 1024;

public:
    WayPointAStar();
    ~WayPointAStar();

    // @brief Set the graph pointer we will use.
    // @param graph the WayPointGraph to be used.
    // @note The graph shouldn't be removed (freed its memory) until you
    //       stop using this class.
    //
    void
    setGraph(const WayPointGraph* graph);

    // @brief Perform the A* algorithm given 2 nodes (start, end).
    // @param startIndex    The first node from where we start (index).
    // @param endIndex      The last node where we want to go (end) (index).
    // @param path          The resulting path to be filled
    // @return WayPointPath::Type
    //
    WayPointPath::Type
    getPath(const index_t startIndex, const index_t endIndex, WayPointPath& path);

private:
    const WayPointNode* mNodes;
    unsigned int mNodesCount;
    core::BoolCountingMask<> mClosedSet;
    core::BoolCountingMask<> mOpenSetChecker;
};

} /* namespace gps */
#endif /* WAYPOINTASTAR_H_ */
