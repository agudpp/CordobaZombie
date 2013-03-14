/*
 * PathfindingManager.h
 *
 *  Created on: 01/02/2012
 *      Author: agustin
 */

#ifndef PATHFINDINGMANAGER_H_
#define PATHFINDINGMANAGER_H_

#include <vector>

#include "PolyStructsContainer.h"

#include "TriangleNavMesh.h"
#include "TriangleFunnel.h"
#include "TriangleAStar.h"
#include "TriangleCache.h"


#define PATHFINDER_INSTANCE		PathfindingManager::getInstance()

class PathfindingManager
{
public:
	typedef std::vector<sm::Point>		Path;


	// The possibilities of the path
	enum {
		PATH_NOT_FOUND,		// when the path is not found
		LINE_PATH,			// when the unit can get the goal in a straight line
		NORMAL_PATH,
	};

public:

	// returns the instance
	static inline PathfindingManager &getInstance(void)
	{
		static PathfindingManager instance;
		return instance;
	}

	/* Unload the actual pathfinding level */
	void unloadActualLevel(void);

	/* Load a new level from a list of triangles and vertices.
	 * This triangles and vertices now belongs to the PathFinderManager.
	 * DO NOT DELET any triangle nor any vertex
	 */
	int loadLevel(const std::vector<Triangle *> &triangles,
	              const std::vector<sm::Vertex *> &vertices);

	/* Load a level from a Graph
	 * This triangles and vertices now belongs to the PathFinderManager.
	 * DO NOT DELET any triangle nor any vertex
	 */
	int loadLevel(const Graph &g, const std::vector<Triangle *> &triangles,
	              const std::vector<sm::Vertex *> &vertices);

	/* Function used to get the shortest path from 2 differents points
	 * Requires:
	 * 	@start		The starting point
	 * 	@goal		The ending point
	 * 	@path		The path were it will been put all the points where
	 * 				path[0] is the starting point and path[size] is the goal
	 * 				point
	 * 	@radius		The unit radius (used to calculate the best path)
	 *
	 * Returns:
	 * 	PATH_NOT_FOUND	when the path is not found
	 *	LINE_PATH		when the unit can get the goal in a straight line
	 *	NORMAL_PATH		when there are a path
	 */
	int getPath(const sm::Point &start, const sm::Point &end,
	            Path &path,
	            float radius = 0.0f,
	            float delta = 1);

	/**
	 * Get a random path from an specific position.
	 * @param start		The starting point
	 * @param numNodes	The number of nodes of the random path, if this
	 * 					value is -1 then we create a random path using random
	 * 					number of nodes
	 * @param radius	The radius used to calculate the path
	 * @path			The resulting path
	 * @return errCode
	 *
	 * @note	We do not apply funnel algorithm here..
	 */
	int getRandomPath(const sm::Point &start,
	                  Path &path,
	                  float radius = 0.0f,
	                  int numNodes = -1);

	/**
	 * Returns the associated GNode to a certain point or 0 if there are not.
	 * @param	p	The point where we will get the GNode
	 */
	inline const GNode *getNodeFromPoint(const sm::Point &p) const;

	/**
	 * Updates a given path and a new "end position". This function will modify
	 * the given path and add (or remove) all the points that are not necessary
	 * and apply the funnel algorithm if is nescesary
	 * @param	path	The path to be modify
	 * @param	ap		The actual position in the path (path[ap]).
	 * @param	np		New "end" position
	 *
	 * Returns:
	 * 	PATH_NOT_FOUND	when the path is not found
	 *	LINE_PATH		when the unit can get the goal in a straight line
	 *	NORMAL_PATH		when there are a path
	 */
	int updatePath(Path &path, int ap, const sm::Point &np, float radius = 0.0f);


private:
	PathfindingManager();
	~PathfindingManager();

	/* Create all the objetcs used by the manager */
	void createAll(int numRows, int numColumns, int maxPathSize);


private:
	typedef PolyStructsContainer<Triangle *>	TriangleCont;
	typedef PolyStructsContainer<sm::Vertex *>		VertexCont;

	TriangleNavMesh					*mNavMesh;
	TriangleAStar					*mAStar;
	TriangleFunnel					*mFunnel;

	TriangleCont					mTriangles;
	VertexCont						mVertices;
	Path							mAuxPath;
};





/**
 * Returns the associated GNode to a certain point or 0 if there are not.
 * @param	p	The point where we will get the GNode
 */
inline const GNode *
PathfindingManager::getNodeFromPoint(const sm::Point &p) const
{
	ASSERT(mNavMesh);
	return mNavMesh->getNodeFromPoint(p);
}

#endif /* PATHFINDINGMANAGER_H_ */
