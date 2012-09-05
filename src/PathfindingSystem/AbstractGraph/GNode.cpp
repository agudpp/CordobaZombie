/*
 * GNode.cpp
 *
 *  Created on: 24/01/2012
 *      Author: agustin
 */

#include "GNode.h"


bool GNode::setNewEdge(const GEdge *e)
{
	if (e == mEdge1 || e == mEdge2 || e == mEdge3)
	{
		return false;
	}

	if (mEdge1 == 0)
	{
		mEdge1 = e;
		return true;
	}
	else if (mEdge2 == 0)
	{
		mEdge2 = e;
		return true;
	}
	else if (mEdge3 == 0)
	{
		mEdge3 = e;
		return true;
	}
	return false;
}

int GNode::getNumCommonVertexs(const GNode &other) const
{
	ASSERT(mTriangle && other.mTriangle);

	int count = 0;
	if (mTriangle->v1 == other.mTriangle->v1) count++;
	else if (mTriangle->v1 == other.mTriangle->v2) count++;
	else if (mTriangle->v1 == other.mTriangle->v3) count++;

	if (mTriangle->v2 == other.mTriangle->v1) count++;
	else if (mTriangle->v2 == other.mTriangle->v2) count++;
	else if (mTriangle->v2 == other.mTriangle->v3) count++;

	if (mTriangle->v3 == other.mTriangle->v1) count++;
	else if (mTriangle->v3 == other.mTriangle->v2) count++;
	else if (mTriangle->v3 == other.mTriangle->v3) count++;

	return count;
}
