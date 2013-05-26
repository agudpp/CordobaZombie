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

	// Process and get the list of pathcore::Points
	bool getShortestPath(TriangleAStar::Path p,
	                     std::vector<core::Vertex> &resultPath,
	                     int size,
	                     const core::Point &start,
	                     const core::Point &goal,
	                     float radius = 0.0f,
	                     float delta = -1) const;

private:
	typedef std::vector<const core::Vertex*>	 VertexVec;
	typedef std::pair<bool, const core::Vertex *>	TunnelVertex;
	typedef std::vector<TunnelVertex>		TunnelVertexVec;

private:
	// get all the right and left vertices
	void getSortedVertices(TriangleAStar::Path p,
	                       const core::Point &start,
	                       const core::Point &goal,
	                       int size) const;

	// PostProcess the path to create the new path using a radius
	void processRadius(std::vector<core::Vertex> &resultPath,
	                   float radius,
	                   float delta) const;

	// Function that given a edge and a radius returns the 2 new vertex points
	// moved from the vertices "radius" units over the line formed between this
	// two vertices
	inline void getNewRadiusPoints(const GEdge *e,
	                               float radius,
	                               core::Vertex &v1,
	                               core::Vertex &v2) const;

	// Is p1 clockwise to p2?
	inline bool isClockwise(const core::Point &p1, const core::Point &p2) const;
	inline bool isCounterClockwise(const core::Point &p1, const core::Point &p2) const;



#ifdef DEBUG
	void drawRightAndLeftVertices(void);
#endif

private:
	mutable VertexVec mLeftVertices;
	mutable VertexVec mRightVertices;

};



// Inline impl
//
inline void
TriangleFunnel::getNewRadiusPoints(const GEdge *e,
                                   float radius,
                                   core::Vertex &v1,
                                   core::Vertex &v2) const
{
    // we will use vertex as vectors
    core::Vertex nv = *(e->getSharedVertex()[0]);
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

inline bool
TriangleFunnel::isClockwise(const core::Point &p1, const core::Point &p2) const
{
    return p1.crossProduct(p2) < 0.0f;
}

inline bool
TriangleFunnel::isCounterClockwise(const core::Point &p1, const core::Point &p2) const
{
    return p1.crossProduct(p2) > 0.0f;
}

#endif /* TRIANGLEFUNNEL_H_ */
