/*
 * WayPointGraph.h
 *
 *  Created on: Aug 24, 2013
 *      Author: agustin
 */

#ifndef WAYPOINTGRAPH_H_
#define WAYPOINTGRAPH_H_

#include <cstring>
#include <vector>

#include <debug/DebugUtil.h>
#include <types/ContiguousCont.h>
#include <types/StackVector.h>

#include "WayPointDefines.h"

namespace gps {


// We will define some helper classes here used by the graph Node that
// will conform the graph. Each element should maintain a index to the elements
// that need to point to (instead of pointers, this way we will be able to
// save all the memory block into the disk and load it from there very easily)
//

struct WayPointNode {
    Vertex position;
    index_t neighborsCount;
    index_t neighbors[WPN_MAX_NUM_NEIGHBORS];

    // @note that this is ugly because we are wasting a lot of memory here
    //       and could be variable (the size) but this will complicate how we
    //       save into disc and load from the disk the information... It could be
    //       done but will require a little more of time.. Not now.

    ////////////////////////////////////////////////////////////////////////////
    //                              Helper methods                            //
    ////////////////////////////////////////////////////////////////////////////

    // @brief Reset all the data for this WayPointNode.
    //
    inline void
    reset(void);

    // @brief Add a new neighbor for this node
    // @param ni    The neighbor index
    //
    inline void
    addNeighbor(index_t ni);

    // @brief Check if we have an neighbor index or not
    // @param ni    The neighbor index
    //
    inline bool
    hasNeighbor(index_t ni) const;

};

// some useful typedefs
//
typedef core::ContiguousContainer<WayPointNode> WPNodeCont;
typedef core::StackVector<WayPointNode, WPG_MAX_NUM_NODES> WPNodeStackVec;

class WayPointGraph
{
public:
    WayPointGraph();
    ~WayPointGraph();

    ////////////////////////////////////////////////////////////////////////////

    // @brief Construct the Graph from a list of WayPointNodes
    // @param nodes     The list of nodes
    // @return true on success | false otherwise
    //
    bool
    build(const std::vector<WayPointNode>& nodes);
    bool
    build(const WPNodeStackVec& nodes);

    ////////////////////////////////////////////////////////////////////////////

    // @brief Load the graph from an already saved file.
    // @param filename  The filename where the graph is
    // @return true on success | false otherwise
    //
    bool
    build(const std::string& filename);

    // @brief Save the graph into a file
    // @param filename  The filename where we want to save the graph
    // @return true on success | false otherwise
    //
    bool
    save(const std::string& filename) const;

    // @brief Return all the nodes of this graph.
    //
    inline const WPNodeCont&
    nodes(void) const;

    // @brief Compare operator.
    // @param other The other graph to be compared with this one
    // @return true if they are equal | false otherwise
    //
    inline bool
    operator==(const WayPointGraph& other) const;

private:
    // avoid copying
    //
    WayPointGraph(const WayPointGraph&);
    WayPointGraph& operator=(const WayPointGraph&);

    // @brief Check if the current mNodes contain a valid information
    // @return true if it is, false otherwise
    //
    bool
    containValidInfo(void) const;

private:
    WPNodeCont mNodes;
};




////////////////////////////////////////////////////////////////////////////////
//                              Inline stuff                                  //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// WayPointNode
//

inline void
WayPointNode::reset(void)
{
    position.x = position.y = 0;
    neighborsCount = 0;
}

inline void
WayPointNode::addNeighbor(index_t ni)
{
    ASSERT(neighborsCount <= WPN_MAX_NUM_NEIGHBORS);
    neighbors[neighborsCount] = ni;
    ++neighborsCount;
}

inline bool
WayPointNode::hasNeighbor(index_t ni) const
{
    for (index_t i = 0; i < neighborsCount; ++i) {
        if (neighbors[i] == ni) {
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// WayPointGraph
//

inline const WPNodeCont&
WayPointGraph::nodes(void) const
{
    return mNodes;
}

inline bool
WayPointGraph::operator==(const WayPointGraph& other) const
{
    if (mNodes.size != other.mNodes.size) {
        return false;
    }

    const WayPointNode* o = other.nodes().data;
    for (unsigned int i = 0; i < mNodes.size; ++i) {
        const WayPointNode& a = mNodes[i];
        const WayPointNode& b = o[i];
        if (a.position != b.position ||
            a.neighborsCount != b.neighborsCount) {
            return false;
        }
        for (unsigned int j = 0; j < a.neighborsCount; ++j) {
            if (a.neighbors[j] != b.neighbors[j]) {
                return false;
            }
        }
    }
    return true;
}

} /* namespace gps */
#endif /* WAYPOINTGRAPH_H_ */
