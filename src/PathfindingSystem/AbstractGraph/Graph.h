/*
 * Graph.h
 *
 *  Created on: 25/01/2012
 *      Author: agustin
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <vector>

#include "GEdge.h"
#include "GNode.h"

class Graph
{
public:
	Graph();
	~Graph();

	// builds the graph from a list of nodes and edges
	//
	void setNodesAndEdges(const std::vector<GNode *> &nodes,
			const std::vector<GEdge *> &edges);

	// Gets the edges
	//
	inline const std::vector<GEdge *> &getEdges(void) const {return mEdges;}

	// Gets the edges
	//
	inline const std::vector<GNode *> &getNodes(void) const {return mNodes;}

	// Remove all nodes
	//
	void removeNodes(void);
	void removeAndDestroyNodes(void);

	// Remove all edges
	//
	void removeEdges(void);
	void removeAndDestroyEdeges(void);

	// Verify if two Graphs are equal
	//
	bool operator==(const Graph &other) const;

private:
	Graph(const Graph &other);
	Graph &operator=(const Graph &other);


private:
	std::vector<GNode *>		mNodes;
	std::vector<GEdge *>		mEdges;
};

#endif /* GRAPH_H_ */
