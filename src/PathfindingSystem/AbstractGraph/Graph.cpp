/*
 * Graph.cpp
 *
 *  Created on: 25/01/2012
 *      Author: agustin
 */

#include "Graph.h"

Graph::Graph()
{
	// TODO Auto-generated constructor stub

}

Graph::~Graph()
{
}


// builds the graph from a list of nodes and edges
void Graph::setNodesAndEdges(const std::vector<GNode *> &nodes,
		const std::vector<GEdge *> &edges)
{
	if(!mNodes.empty()){
		debug("Warning: the graph already have nodes\n");
		ASSERT(false);
		removeNodes();
	}
	if(!mEdges.empty()){
		debug("Warning: the graph already have edges\n");
		ASSERT(false);
		removeEdges();
	}

	mNodes = nodes;
	mEdges = edges;
}

// Remove all nodes
void Graph::removeNodes(void)
{
	mNodes.clear();
}
void Graph::removeAndDestroyNodes(void)
{
	for(int i = mNodes.size()-1; i >= 0; --i) delete mNodes[i];
	mNodes.clear();
}

// Remove all edges
void Graph::removeEdges(void)
{
	mEdges.clear();
}
void Graph::removeAndDestroyEdeges(void)
{
	for(int i = mEdges.size()-1; i >= 0; --i) delete mEdges[i];
	mEdges.clear();
}



