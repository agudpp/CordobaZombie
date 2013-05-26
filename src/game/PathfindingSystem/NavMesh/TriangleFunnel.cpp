/*
 * TriangleFunnel.cpp
 *
 *  Created on: 31/01/2012
 *      Author: agustin
 */

#ifdef DEBUG
#include <OgreEntity.h>
#include "GlobalObjects.h"
#include "PrimitiveDrawer.h"
#endif

#include "TriangleFunnel.h"


#ifdef DEBUG
	void TriangleFunnel::drawRightAndLeftVertices(void)
	{
		DRAWER.destroyAllVPoints();
		debug("Drawing Left Vertices: %zd\n", mLeftVertices.size());
		for(int i = 0; i < mLeftVertices.size(); ++i){
			DRAWER.createNewVPoint(Ogre::Vector3(mLeftVertices[i]->x, 0,
					mLeftVertices[i]->y), PrimitiveDrawer::COLOR_GREEN);
//			debug("Green(%f,%f)\n", mLeftVertices[i].x, mLeftVertices[i].y);
		}

		debug("Drawing Right Vertices: %zd\n", mRightVertices.size());
		for(int i = 0; i <mRightVertices.size(); ++i){
			DRAWER.createNewVPoint(Ogre::Vector3(mRightVertices[i]->x, 0,
					mRightVertices[i]->y), PrimitiveDrawer::COLOR_RED);
//			debug("Red(%f,%f)\n", mRightVertices[i].x, mRightVertices[i].y);
		}
	}
#endif



static inline float triarea2(const core::Vertex &a, const core::Vertex &b, const core::Vertex &c)
{
 return (c.x - a.x)*(b.y - a.y) - (b.x - a.x)*(c.y-a.y);
}


// get all the right and left vertices
void
TriangleFunnel::getSortedVertices(TriangleAStar::Path p,
                                  const core::Point &start,
                                  const core::Point &goal,
                                  int size) const
{

	int count = size - 1;
	const GEdge *edge = 0;
	ASSERT(count >= 1);

	mLeftVertices.clear();
	mRightVertices.clear();

//	// TODO :Remove this
	mLeftVertices.push_back(&start);
	mRightVertices.push_back(&start);

	ASSERT(p[count] && p[count-1]);
	edge = p[count]->getSharedEdge(p[count-1]);
	ASSERT(edge);

	core::Vertex v1,v2;
	core::Vertex auxV1 = *(edge->getSharedVertex()[0]) - start;
	core::Vertex auxV2 = *(edge->getSharedVertex()[1]) - start;
	if (isClockwise(auxV1, auxV2))
	{
		mLeftVertices.push_back(edge->getSharedVertex()[0]);
		mRightVertices.push_back(edge->getSharedVertex()[1]);
	}
	else
	{
		mLeftVertices.push_back(edge->getSharedVertex()[1]);
		mRightVertices.push_back(edge->getSharedVertex()[0]);
	}


	--count;
	while (count > 0)
	{
		edge = p[count - 1]->getSharedEdge(p[count]);
		ASSERT(edge);
		if (mRightVertices.back() == edge->getSharedVertex()[0])
		{
			mLeftVertices.push_back(edge->getSharedVertex()[1]);
			mRightVertices.push_back(edge->getSharedVertex()[0]);
		}
		else if (mRightVertices.back() == edge->getSharedVertex()[1])
		{
			mLeftVertices.push_back(edge->getSharedVertex()[0]);
			mRightVertices.push_back(edge->getSharedVertex()[1]);
		}

		else if (mLeftVertices.back() == edge->getSharedVertex()[0])
		{
			mLeftVertices.push_back(edge->getSharedVertex()[0]);
			mRightVertices.push_back(edge->getSharedVertex()[1]);
		}
		else if (mLeftVertices.back() == edge->getSharedVertex()[1])
		{
			mRightVertices.push_back(edge->getSharedVertex()[0]);
			mLeftVertices.push_back(edge->getSharedVertex()[1]);
		}

		--count;
	}

	// add starting core::Point to both vectors
	mLeftVertices.push_back(&goal);
	mRightVertices.push_back(&goal);

}


// PostProcess the path to create the new path using a radius
void
TriangleFunnel::processRadius(std::vector<core::Vertex> &resultPath,
                              float radius,
                              float delta) const
{
	ASSERT(radius > 0.0f);

	// we will not process the first and the last core::Point
	const int size = resultPath.size() - 1;
	core::Vertex vec1, vec2, vec3;
	for(int i = 1; i < size; ++i){
		ASSERT((float)rand()/(float)RAND_MAX >= 0);
		// get the first vector
		vec1 = resultPath[i] - resultPath[i-1];
		vec2 = resultPath[i] - resultPath[i+1]; //reverse direction

		// get the normalized position

		// TODO: chequear como optimizar aca
		vec1.normalize();
		vec2.normalize();
		vec1 += vec2;
		vec1.normalize();
		if(delta > 0.0f)
			vec1 *= (radius + delta * ((float)rand()/(float)RAND_MAX));
		else
			vec1 *= radius;
		resultPath[i] += vec1;

	}
}



TriangleFunnel::TriangleFunnel()
{
}

TriangleFunnel::~TriangleFunnel()
{
}

// Process and get the list of pathcore::Points
bool
TriangleFunnel::getShortestPath(TriangleAStar::Path p,
                                std::vector<core::Vertex> &resultPath,
                                int size,
                                const core::Point &start,
                                const core::Point &goal,
                                float radius,
                                float delta) const
{
	if(size < 2){
		debug("Funnel with 1 nodes, no sense\n");
		ASSERT(false);
		return false;
	}

	getSortedVertices(p, start, goal, size);

	// extracted from http://digestingduck.blogspot.com/2010/03/simple-stupid-funnel-algorithm.html
	// Init scan state
	const core::Vertex *portalApex, *portalLeft, *portalRight;
	int apexIndex = 0, leftIndex = 0, rightIndex = 0;
	int numPortals = mLeftVertices.size();

	ASSERT(mLeftVertices.size() == mRightVertices.size());

	portalApex = mLeftVertices.front();
	portalLeft = mLeftVertices.front();
	portalRight = mRightVertices.front();

	resultPath.clear();

	// Add start core::Point.
	resultPath.push_back(start);


	for (int i = 1; i < numPortals; ++i) {
		const core::Vertex* left = mLeftVertices[i];
		const core::Vertex* right = mRightVertices[i];

		// Update right core::Vertex.
		if (triarea2(*portalApex, *portalRight, *right) <= 0.0f) {
			if (portalApex == portalRight || triarea2(*portalApex,
					*portalLeft, *right) > 0.0f) {
				// Tighten the funnel.
				portalRight = right;

				rightIndex = i;
			} else {
				// Right over left, insert left to path and restart scan from portal left core::Point.
				// check if we already add this portal
				if(portalApex != portalLeft){
					resultPath.push_back(*portalLeft);
				}

				// Make current left the new apex.
				portalApex = portalLeft;
				apexIndex = leftIndex;
				// Reset portal
				portalLeft = portalApex;
				portalRight = portalApex;
				leftIndex = apexIndex;
				rightIndex = apexIndex;
				// Restart scan
				i = apexIndex;
				continue;
			}
		}

		// Update left core::Vertex.
		if (triarea2(*portalApex, *portalLeft, *left) >= 0.0f) {
			if (portalApex == portalLeft || triarea2(*portalApex,
					*portalRight, *left) < 0.0f) {
				// Tighten the funnel.
				portalLeft = left;
				leftIndex = i;
			} else {
				// Left over right, insert right to path and restart scan from portal right core::Point.
				// check if we already add this portal
				if(portalApex != portalRight){
					resultPath.push_back(*portalRight);
				}
				// Make current right the new apex.
				//                              vcpy(portalApex, portalRight);
				portalApex = portalRight;
				apexIndex = rightIndex;
				// Reset portal
				portalLeft = portalApex;
				portalRight = portalApex;
				//                              vcpy(portalLeft, portalApex);
				//                              vcpy(portalRight, portalApex);
				leftIndex = apexIndex;
				rightIndex = apexIndex;
				// Restart scan
				i = apexIndex;
				continue;
			}
		}
	}
	// Append last core::Point to path.
	if(resultPath.back() != goal){
		resultPath.push_back(goal);
	}

	// Check for radius to update the positions
	if(radius > 0.0f){
		// check if we have to apply the radius
		processRadius(resultPath, radius, delta);
	}
//
//#ifdef DEBUG
//	drawRightAndLeftVertices();
//#endif

	return true;
}
