/*
 * GEdge.cpp
 *
 *  Created on: 25/01/2012
 *      Author: agustin
 */

#include <cmath>

#include "GEdge.h"
#include "GNode.h"

// Sets the 2 vertexs that the nodes shares by this edge
void GEdge::setSharedVertexs(void)
{
	ASSERT(mNode1 && mNode2);
	const Triangle *t1 = mNode1->getTriangle();
	const Triangle *t2 = mNode2->getTriangle();



	if(t1->v1 == t2->v1 || t1->v1 == t2->v2 || t1->v1 == t2->v3){
		mSharedVertexs[0] = t1->v1;
	}
	if(mSharedVertexs[0]){
		if(t1->v2 == t2->v1 || t1->v2 == t2->v2 || t1->v2 == t2->v3){
			mSharedVertexs[1] = t1->v2;
	} else if(t1->v3 == t2->v1 || t1->v3 == t2->v2 || t1->v3 == t2->v3){
			mSharedVertexs[1] = t1->v3;
		} else {
			// error, there are not 2 vertexs
			ASSERT(false);
		}
	} else {
		// the first is not set
		if(t1->v2 == t2->v1 || t1->v2 == t2->v2 || t1->v2 == t2->v3){
			mSharedVertexs[0] = t1->v2;
		}
		if(t1->v3 == t2->v1 || t1->v3 == t2->v2 || t1->v3 == t2->v3){
			mSharedVertexs[1] = t1->v3;
		} else {
			// error, there are not 2 vertexs
			ASSERT(false);
		}
	}
	// set the radius
	mWeight = std::sqrt(mSharedVertexs[0]->squaredDistance(*mSharedVertexs[1]));
}
