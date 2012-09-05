/*
 * TriangleFunnel.h
 *
 *  Created on: 31/01/2012
 *      Author: agustin
 */

#ifndef TRIANGLEFUNNEL_H_
#define TRIANGLEFUNNEL_H_

#include <vector>
#include <deque>

#include "TriangleAStar.h"

class TriangleFunnel
{
public:
	TriangleFunnel();
	~TriangleFunnel();

	// Process and get the list of pathsm::Points
	bool getShortestPath(TriangleAStar::Path p, std::vector<sm::Vertex> &resultPath,
			int size, const sm::Point &start, const sm::Point &goal,
			float radius = 0.0f, float delta = -1);

private:
	typedef std::vector<const sm::Vertex*>	 VertexVec;
	typedef std::pair<bool, const sm::Vertex *>	TunnelVertex;
	typedef std::vector<TunnelVertex>		TunnelVertexVec;

private:
	// get all the right and left vertices
	void getSortedVertices(TriangleAStar::Path p, const sm::Point &start,
			const sm::Point &goal, int size);

	// PostProcess the path to create the new path using a radius
	void processRadius(std::vector<sm::Vertex> &resultPath, float radius,
			float delta);

	// Function that given a edge and a radius returns the 2 new vertex points
	// moved from the vertices "radius" units over the line formed between this
	// two vertices
	inline void getNewRadiusPoints(const GEdge *e, float radius,
			sm::Vertex &v1, sm::Vertex &v2)
	{
		// we will use vertex as vectors
		sm::Vertex nv = *(e->getSharedVertex()[0]);
		nv -= *(e->getSharedVertex()[1]);
		nv.normalize();

		nv *= radius;
		v2 = *(e->getSharedVertex()[1]);
		v2 += nv;
		nv *= -1.0f;
		v1 = *(e->getSharedVertex()[0]);
		v1 += nv;

		debug("v1:(%f,%f), v2(%f,%f)\n", v1.x, v1.y, v2.x, v2.y);
	}

	// Is p1 clockwise to p2?
	inline bool isClockwise(const sm::Point &p1, const sm::Point &p2) const
	{
		return p1.crossProduct(p2) < 0.0f;
	}

	inline bool isCounterClockwise(const sm::Point &p1, const sm::Point &p2) const
	{
		return p1.crossProduct(p2) > 0.0f;
	}



#ifdef DEBUG
	void drawRightAndLeftVertices(void);
#endif

private:
	VertexVec			mLeftVertices;
	VertexVec			mRightVertices;

};

#endif /* TRIANGLEFUNNEL_H_ */
