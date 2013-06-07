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

// Verify if two Graphs are equal
//
bool
Graph::operator==(const Graph &other) const
{
    if (&other == this) {
        return true;
    }

    // fast check
    const size_t edgesCount = mEdges.size();
    const size_t nodesCount = mNodes.size();

    if (edgesCount != other.mEdges.size() ||
            nodesCount != other.mNodes.size()) {
        debug("The number of nodes: %zu, other: %zu\tNumber of edges: %zu, other: %zu\n",
                nodesCount, other.mNodes.size(), edgesCount, other.mEdges.size());
        return false;
    }

    // compare each of this edges and nodes
    for(size_t i = 0; i < edgesCount; ++ i){
        if (!(*mEdges[i] == *other.mEdges[i])) {
            return false;
        }
    }
    for(size_t i = 0; i < nodesCount; ++ i){
        if (!(*mNodes[i] == *other.mNodes[i])) {
            return false;
        }
    }

    return true;
}
