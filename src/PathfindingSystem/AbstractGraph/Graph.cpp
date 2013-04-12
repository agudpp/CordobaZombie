/*
 * Graph.cpp
 *
 *  Created on: 25/01/2012
 *      Author: agustin
 */

#include "Graph.h"

#include "GEdge.h"
#include "GNode.h"

// builds the graph from a list of nodes and edges
void
Graph::setNodesAndEdges(const std::vector<GNode *> &nodes,
                        const std::vector<GEdge *> &edges)
{
    if (!mNodes.empty()) {
        debug("Warning: the graph already have nodes\n");
        ASSERT(false);
        removeNodes();
    }
    if (!mEdges.empty()) {
        debug("Warning: the graph already have edges\n");
        ASSERT(false);
        removeEdges();
    }

    mNodes = nodes;
    mEdges = edges;

    // set the ids for the nodes
    for (size_t i = 0, size = mNodes.size(); i < size; ++i) {
        mNodes[i]->mID = i;
    }
}

// Remove all nodes
void
Graph::removeNodes(void)
{
    mNodes.clear();
}
void
Graph::removeAndDestroyNodes(void)
{
    for (size_t i = 0, size = mNodes.size(); i < size; +i) {
        delete mNodes[i];
    }

    mNodes.clear();
}

// Remove all edges
void
Graph::removeEdges(void)
{
    mEdges.clear();
}
void
Graph::removeAndDestroyEdeges(void)
{
    for (size_t i = 0, size = mEdges.size(); i < size; +i) {
        delete mEdges[i];
    }
    mEdges.clear();
}

