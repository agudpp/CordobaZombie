/*
 * PathfindingManager.cpp
 *
 *  Created on: 01/02/2012
 *      Author: agustin
 */

#include "DebugUtil.h"
#include "PathfindingManager.h"
#include <cstdlib>


////////////////////////////////////////////////////////////////////////////////
void PathfindingManager::createAll(int numRows, int numColumns, int maxPathSize)
{
	ASSERT(!mNavMesh);
	ASSERT(!mAStar);
	ASSERT(!mFunnel);
	ASSERT(mTriangles.isEmpty());

	if(numRows < 0 || numColumns < 0){
		mNavMesh = new TriangleNavMesh;
	} else {
		mNavMesh = new TriangleNavMesh(numColumns, numRows);
	}
	if(maxPathSize < 0){
		mAStar = new TriangleAStar(mNavMesh);
	} else {
		mAStar = new TriangleAStar(mNavMesh, maxPathSize);
	}
	mFunnel = new TriangleFunnel;

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
PathfindingManager::PathfindingManager() :
mNavMesh(0),
mAStar(0),
mFunnel(0)
{
	// TODO Auto-generated constructor stub

}

////////////////////////////////////////////////////////////////////////////////
PathfindingManager::~PathfindingManager()
{
	// TODO Auto-generated destructor stub
}


////////////////////////////////////////////////////////////////////////////////
void PathfindingManager::unloadActualLevel(void)
{
	delete mFunnel; mFunnel = 0;
	delete mAStar; mAStar = 0;
	delete mNavMesh; mNavMesh = 0;
	mTriangles.removeAndFreeAll();
	mVertices.removeAndFreeAll();
}

////////////////////////////////////////////////////////////////////////////////
int PathfindingManager::loadLevel(const std::vector<Triangle *> &triangles,
		const std::vector<sm::Vertex *> &vertices)
{
	if(mNavMesh){
		debug("Warning: Loading a new level without removing the first one\n");
		unloadActualLevel();
	}

	createAll(-1,-1, -1);

	for(int i = vertices.size()-1; i >= 0; --i){
		mVertices.addObj(vertices[i]);
	}

	for(int i = triangles.size()-1; i >= 0; --i){
		mTriangles.addObj(triangles[i]);
		mNavMesh->addTriangle(triangles[i]);
	}

	return mNavMesh->build();
}


////////////////////////////////////////////////////////////////////////////////
int PathfindingManager::loadLevel(const Graph &g,
		const std::vector<Triangle *> &triangles,
		const std::vector<sm::Vertex *> &vertices)
{
	if(mNavMesh){
		debug("Warning: Loading a new level without removing the first one\n");
		unloadActualLevel();
	}

	createAll(-1,-1, -1);

	for(int i = vertices.size()-1; i >= 0; --i){
		mVertices.addObj(vertices[i]);
	}

	for(int i = triangles.size()-1; i >= 0; --i){
		mTriangles.addObj(triangles[i]);
	}

	return mNavMesh->buildFromGraph(g);
}


////////////////////////////////////////////////////////////////////////////////
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
 */
int PathfindingManager::getPath(const sm::Point &start, const sm::Point &goal,
		Path &path, float radius, float delta)
{
	ASSERT(mAStar);
	path.clear();

	// get the Path
	size_t size;
	float min = 0;
	const TriangleAStar::Path p = mAStar->getshortestPath(start, goal, size, radius, min);
	if(!p){
		return PATH_NOT_FOUND;
	}
	if(size == 1){
		// there are in the same triangle, straight line
		return LINE_PATH;
	}

	// else we have a path, we now perform funnel to get the shortest

	if(delta > min-2.0f*radius) delta = min-2.0f*radius;

	if(!mFunnel->getShortestPath(p, path, size, start, goal, radius, delta)){
		debug("Error getting funnel\n");
		return PATH_NOT_FOUND;
	}

	// there is a path
	return NORMAL_PATH;
}


/**
 * Get a random path from an specific position.
 * @param start		The starting point
 * @param numNodes	The number of nodes of the random path, if this
 * 					value is -1 then we create a random path using random
 * 					number of nodes
 * @path			The resulting path
 * @return errCode
 */
int PathfindingManager::getRandomPath(const sm::Point &start, Path &path,
		float radius, int numNodes)
{
	// get the starting point node
	const GNode *n = mNavMesh->getNodeFromPoint(start);
	if(!n){
		return PATH_NOT_FOUND;
	}

	if(numNodes < 0){
		// get no more than the 1/4 of the number of nodes
		numNodes = std::rand() % (mNavMesh->getGraph().getNodes().size() >> 2);
	}

	path.clear();

	// start to generate the path
	const GNode *old = n, *neigh;
	const GNode * neighbors[3];
	sm::Point aux;
	int rndInx, j;
	ASSERT(n->getTriangle());
	n->getTriangle()->getCenterPoint(aux);
	path.push_back(aux);
	for(int i = numNodes; i >=0 ; --i){
		n->getNeighbors(neighbors);
		rndInx = std::rand() % 3;
		for(j = 0; j < 3; ++j){
			neigh = neighbors[rndInx];
			if(!neigh || old == neigh){
				rndInx = (rndInx+1) % 3;
			} else
				break;

		}
		if(j == 3 || !neigh) { break; /* we are in trouble... return */}

		if(neigh->getTriangle()){
			neigh->getTriangle()->getRandomPoint(aux);
			path.push_back(aux);
			old = n;
			n = neigh;
		}
//		continue;
	}

	return NORMAL_PATH;
}

/**
 * Updates a given path and a new "end position". This function will modify
 * the given path and add (or remove) all the points that are not necessary
 * and apply the funnel algorithm if is nescesary
 * @param	path	The path to be modify
 * @param	np		New "end" position
 *
 * Returns:
 * 	PATH_NOT_FOUND	when the path is not found
 *	LINE_PATH		when the unit can get the goal in a straight line
 *	NORMAL_PATH		when there are a path
 */
int PathfindingManager::updatePath(Path &path, int ap, const sm::Point &np,
		float radius)
{
	ASSERT(!path.empty());
	ASSERT(ap <= path.size());

	// we will do a ugly thing here that is to recalculate the n/2 last points
	// of the path..
	const int rp = ((path.size() - ap) << 1) + ap;

	// get the new path
	int result = getPath(path[rp], np, mAuxPath, radius);

	if(!mAuxPath.empty()){
		// now put the result into the real path
		// TODO probablemente no haga falta hacer un inmundo resize, y poder
		// agregar lo que falta reemplazando los actuales...
		path.resize(rp);
		path.insert(path.end(), mAuxPath.begin()+1, mAuxPath.end());
	}

	return result;

}

