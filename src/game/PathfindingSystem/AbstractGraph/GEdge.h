/*
 * GEdge.h
 *
 *  Created on: 24/01/2012
 *      Author: agustin
 */

#ifndef GEDGE_H_
#define GEDGE_H_

#include "PolygonStructs.h"
#include "DebugUtil.h"


class GNode;

class GEdge
{
public:
	inline GEdge(const GNode *n1 = 0, const GNode *n2 = 0) :
		mNode1(n1),
		mNode2(n2)
	{
		mSharedVertexs[0] = mSharedVertexs[1] = 0;
		if(n1 && n2) setSharedVertexs();
	}

	~GEdge(){};

	// By now we will use the weight as radius
	inline float getLenght(void) const {return mWeight;}

	// Set/Get the weight (distance or whatever)
	void setWeight(float w) {mWeight = w;}
	inline float getWeight(void) const {return mWeight;}

	// returns the neighbor of
	inline const GNode *getNeighbor(const GNode *n) const
	{ ASSERT(n == mNode1 || n == mNode2); return (n == mNode1) ? mNode2 : mNode1;}

	// Returns/Sets the nodes
	void setNode1(const GNode *n) {mNode1 = n; if(mNode2) setSharedVertexs();}
	inline const GNode *getNode1(void) const {return mNode1;}

	void setNode2(const GNode *n) {mNode2 = n; if(mNode1) setSharedVertexs();}
	inline const GNode *getNode2(void) const {return mNode2;}

	// Returns the vertexs that the nodes share (array of size 2)
	inline const core::Vertex *const *getSharedVertex(void) const {return mSharedVertexs;}

	// compare operator
	bool operator==(const GEdge &other) const
    {
	    // we will only compare the weight, instead of the connections,
	    // we need to do this check later
	    return mWeight == other.mWeight;
    }

private:
	// Sets the 2 vertexs that the nodes shares by this edge
	void setSharedVertexs(void);


private:
	const GNode		*mNode1;
	const GNode		*mNode2;
	float			mWeight;
	const core::Vertex 	*mSharedVertexs[2];

};

#endif /* GEDGE_H_ */
