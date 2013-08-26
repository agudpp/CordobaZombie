/*
 * WayPointGraphBuilder.h
 *
 *  Created on: Aug 25, 2013
 *      Author: agustin
 */

#ifndef WAYPOINTGRAPHBUILDER_H_
#define WAYPOINTGRAPHBUILDER_H_

#include <vector>

#include <types/StackVector.h>

#include "WayPointDefines.h"
#include "WayPointGraph.h"

namespace gps {


typedef core::StackVector<Vertex, WPG_MAX_NUM_NODES> VertexStackVec;

class WayPointGraphBuilder
{
public:
    WayPointGraphBuilder(){};
    ~WayPointGraphBuilder(){};

    ////////////////////////////////////////////////////////////////////////////

    // @brief Helper methods used to construct a graph given its positions and
    //        then setting its connections.
    // @param positions     The positions of each waypoint.
    // @note If you call this method twice you will replace the old information
    //       This is only temporary info, until you call build().
    //
    void
    setWaypointPosition(const VertexStackVec& positions);
    void
    setWaypointPosition(const std::vector<Vertex>& positions);

    // @brief Set a relation between two nodes (waypoints). You should call
    //        setWaypointPosition before calling this method.
    // @param wpIndex1  The waypoint index 1.
    // @param wpIndex2  The waypoint index 2.
    // @requires {wpIndex1, wpIndex1} < positions.size().
    //
    void
    linkWaypoints(index_t wpIndex1, index_t wpIndex2);

    // @brief Build the graph with the current information.
    // @param graph The graph we want to build.
    // @return true on success | false otherwise
    //
    bool
    buildGraph(WayPointGraph& graph) const;

private:
    WPNodeStackVec mNodes;

};

} /* namespace gps */
#endif /* WAYPOINTGRAPHBUILDER_H_ */
