/*
 * WayPointGraphBuilder.cpp
 *
 *  Created on: Aug 25, 2013
 *      Author: agustin
 */
#include "WayPointGraphBuilder.h"

#include <debug/DebugUtil.h>

namespace gps {

////////////////////////////////////////////////////////////////////////////////
void
WayPointGraphBuilder::setWaypointPosition(const VertexStackVec& positions)
{
    mNodes.resize(positions.size());
    for (unsigned int i = 0, size = mNodes.size(); i < size; ++i) {
        mNodes[i].reset();
        mNodes[i].position = positions[i];
    }
}

void
WayPointGraphBuilder::setWaypointPosition(const std::vector<Vertex>& positions)
{
    mNodes.resize(positions.size());
    for (unsigned int i = 0, size = mNodes.size(); i < size; ++i) {
        mNodes[i].reset();
        mNodes[i].position = positions[i];
    }
}

////////////////////////////////////////////////////////////////////////////////
void
WayPointGraphBuilder::linkWaypoints(index_t wpIndex1, index_t wpIndex2)
{
    ASSERT(wpIndex1 < mNodes.size());
    ASSERT(wpIndex2 < mNodes.size());

    WayPointNode& n1 = mNodes[wpIndex1];
    WayPointNode& n2 = mNodes[wpIndex2];

    if (!n1.hasNeighbor(wpIndex2)) {
        n1.addNeighbor(wpIndex2);
    }
    if (!n2.hasNeighbor(wpIndex1)) {
        n2.addNeighbor(wpIndex1);
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
WayPointGraphBuilder::buildGraph(WayPointGraph& graph) const
{
    return graph.build(mNodes);
}

}



