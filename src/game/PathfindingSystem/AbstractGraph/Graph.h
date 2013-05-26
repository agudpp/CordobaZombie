/*
 * Graph.h
 *
 *  Created on: 25/01/2012
 *      Author: agustin
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <vector>
#include <cstddef>

// Forward declarations
//
class GNode;
class GEdge;

class Graph
{
public:
    Graph(){};
    ~Graph(){};

    // builds the graph from a list of nodes and edges
    void
    setNodesAndEdges(const std::vector<GNode *> &nodes,
                     const std::vector<GEdge *> &edges);

    // Gets the edges
    inline const std::vector<GEdge *> &
    getEdges(void) const;

    // Gets the edges
    inline const std::vector<GNode *> &
    getNodes(void) const;

    // Remove all nodes
    void
    removeNodes(void);
    void
    removeAndDestroyNodes(void);

    // Remove all edges
    void
    removeEdges(void);
    void
    removeAndDestroyEdeges(void);

    // Returns the max number of nodes (this is used to get the GNodes.ID's
    // ranges (that it will be between [0, numNodes)
    //
    inline std::size_t
    getNumNodes(void) const;

	// Verify if two Graphs are equal
	//
	bool operator==(const Graph &other) const;

private:
    Graph(const Graph &other);
    Graph &operator=(const Graph &other);


private:
    std::vector<GNode *> mNodes;
    std::vector<GEdge *> mEdges;
};


// Inline implementations
//
inline const std::vector<GEdge *> &
Graph::getEdges(void) const
{
    return mEdges;
}

// Gets the edges
inline const std::vector<GNode *> &
Graph::getNodes(void) const
{
    return mNodes;
}

inline std::size_t
Graph::getNumNodes(void) const
{
    return mNodes.size();
}

#endif /* GRAPH_H_ */
