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
private:
	// this will be used to the bitset size
	static const int MAX_PATH_SIZE	=	150;
	static const int CACHE_SIZE		=	50;
public:
	typedef const GNode *		NodePtr;
	typedef NodePtr *			Path;
public:
	TriangleAStar(const TriangleNavMesh *tnm, int maxPathSize = 50);
	~TriangleAStar();

	/* Returns the shortest path given two sm::Points or 0 if no path is found
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
	const Path getshortestPath(const sm::Point &start, const sm::Point &end, size_t &size,
			float radius, float &min);

private:

	// We will define a priority node struct used to compare the distance from
	// 2 nodes
	struct PriorityNode {
		PriorityNode(){};
		inline PriorityNode(NodePtr n, float h_value, float g_value) :
			node(n),
			hValue(h_value),
			gValue(g_value)
		{
		}

		inline float getFValue(void) const
		{
			return gValue + hValue;
		}

//		inline bool operator<(const PriorityNode &other) const
//		{
//			return getFValue() > other.getFValue();
//		}

		float 		gValue;
		float		hValue;
		NodePtr 	node;
	};
	class priorityNodeCompare
	{
	public:
		bool operator()(const PriorityNode *lpn, const PriorityNode *rpn)
		{
			return lpn->getFValue() > rpn->getFValue();
		}
	};


	// Auxiliar class used to retrieve elements
	template<class T, size_t N = MAX_PATH_SIZE>
	class AuxContainer {
	public:
		AuxContainer():ptr(0){};
		~AuxContainer(){};

		inline T *getFreshObjt(void)
		{
			ASSERT(ptr < N);
			return &objs[ptr++];
		}

		inline T *getFreshObjt(size_t &id)
		{
			ASSERT(ptr < N);
			id = ptr;
			return &objs[ptr++];
		}

		inline T *operator[](size_t i)
		{
			ASSERT(i < ptr);
			return &objs[i];
		}

		inline bool isIDValid(size_t id) const {return id < ptr;}

		inline void restart(void) {ptr = 0;}

		inline size_t getID(const T *elem) const
		{
			return static_cast<size_t>(elem - &objs[0]);
		}


	private:
		size_t 	ptr;
		T		objs[N];
	};

private:
	// Returns a path if exists a path in the cache or 0 if not
	const Path getCachePath(NodePtr n1,	NodePtr n2, size_t &size) const;

	// Function used to add to the cache a path given 2 nodes
	void addToCache(Path p, size_t size, NodePtr n1, NodePtr n2, float radius) const;

	/* This is the main function that we apply A* to find the shortest path
	 * given 2 nodes. If there are no path, then 0 is returned
	 * Otherwise mPath is filled and size is returned */
	void findPath(size_t &size, NodePtr n1, NodePtr n2,
			const sm::Point &p1, const sm::Point &p2, float radius, float &min);




private:
	const TriangleNavMesh		*mNavMesh;
	mutable Path				mPath;
	int							mMaxPathSize;
	mutable TriangleCache mCache;
};

#endif /* TRIANGLEASTAR_H_ */
