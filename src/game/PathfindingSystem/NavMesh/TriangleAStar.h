/*
 * TriangleAStar.h
 *
 *  Created on: 24/01/2012
 *      Author: agustin
 */

#ifndef TRIANGLEASTAR_H_
#define TRIANGLEASTAR_H_

#include "TriangleNavMesh.h"
#include "Graph.h"
#include "DebugUtil.h"
#include "TriangleCache.h"

class TriangleAStar
{
public:

	typedef const GNode *       NodePtr;
	typedef NodePtr *           Path;

public:

	TriangleAStar(const TriangleNavMesh *tnm, int maxPathSize = 50);
	~TriangleAStar();

	/* Returns the shortest path given two math::Points or 0 if no path is found
	 * If path is returned, then the Path[0] is the last node and
	 * Path[size] is the start node
	 * Returns:
	 * 	@path	The list of TriangleNavMesh::Node * of the shortest path
	 * 			starting with the node that contains p1 and finishing with
	 * 			the node that contains the p2.
	 * 	@size	The size of the path
	 * Requires:
	 * 	@radius	The radius of the unit
	 */
	const Path
	getshortestPath(const math::Point &start,
	                const math::Point &end,
	                size_t &size,
	                float radius,
	                float &min);

private:
	// Returns a path if exists a path in the cache or 0 if not
	const Path getCachePath(NodePtr n1,	NodePtr n2, size_t &size) const;

	// Function used to add to the cache a path given 2 nodes
	void addToCache(Path p, size_t size, NodePtr n1, NodePtr n2, float radius) const;

	/* This is the main function that we apply A* to find the shortest path
	 * given 2 nodes. If there are no path, then 0 is returned
	 * Otherwise mPath is filled and size is returned */
	void findPath(size_t &size, NodePtr n1, NodePtr n2,
			const math::Point &p1, const math::Point &p2, float radius, float &min);

private:
	const TriangleNavMesh *mNavMesh;
	mutable Path mPath;
	int mMaxPathSize;
	mutable TriangleCache mCache;

	// Used for the A* algorithm
	std::vector<NodePtr> mMapCameFrom;
	std::vector<unsigned short> mClosedSetChecker;
	std::vector<unsigned short> mOpenSetChecker;
	unsigned short mCurrentCall;
};

#endif /* TRIANGLEASTAR_H_ */
