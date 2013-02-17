/*
 * GNode.h
 *
 *  Created on: 24/01/2012
 *      Author: agustin
 */

#ifndef GNODE_H_
#define GNODE_H_

#include "PolygonStructs.h"
#include "Triangle.h"
#include "GEdge.h"

#include "DebugUtil.h"

class GNode
{
public:
	inline GNode(const Triangle *t = 0) :
	mTriangle(t), mEdge1(0), mEdge2(0), mEdge3(0)
	{
	}
	~GNode(){};

	// Sets a new edge if we can, returns true, else returns false
	bool setNewEdge(const GEdge *e);

	// Get the edges
	inline const GEdge *getEdge1(void) const {return mEdge1;}
	inline const GEdge *getEdge2(void) const {return mEdge2;}
	inline const GEdge *getEdge3(void) const {return mEdge3;}

	// Set the triangle
	void setTriangle(const Triangle *t) {ASSERT(t); mTriangle = t;}
	inline const Triangle* getTriangle(void) const {ASSERT(mTriangle);return mTriangle;}

	// returns the number of common vertex between 2 nodes
	int getNumCommonVertexs(const GNode &other) const;

	// Calculates the squared distance between 2 nodes
	inline float getSquaredDistance(const GNode &other) const
	{
		ASSERT(mTriangle && other.getTriangle());
		sm::Point p1, p2;
		mTriangle->getCenterPoint(p1);
		other.getTriangle()->getCenterPoint(p2);

		return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
	}

	// checks if have a neighbor associated
	inline bool hasNeighbor(const GNode *other) const
	{
		return ((mEdge1 && mEdge1->getNeighbor(this) == other) || (mEdge2
				&& mEdge2->getNeighbor(this) == other) ||
				(mEdge3 && mEdge3->getNeighbor(this) == other));
	}

	// Returns all the neighbors in an NULL terminated array. The array
	// must have at least 3 positions
	inline void getNeighbors(const GNode **neighbors) const
	{
		if(mEdge1) {
			neighbors[0] = mEdge1->getNeighbor(this);
		} else {
			neighbors[0] = 0;
			neighbors[1] = 0;
			neighbors[2] = 0;
			return;
		}

		if(mEdge2){
			neighbors[1] = mEdge2->getNeighbor(this);
		} else {
			neighbors[1] = 0;
			neighbors[2] = 0;
			return;
		}

		if(mEdge3){
			neighbors[2] = mEdge3->getNeighbor(this);
			return;
		} else {
			neighbors[2] = 0;
			return;
		}
	}

	// Returns the edge shared by this node and other node
	// REQUIRES: that the other node really share an edge
	inline const GEdge *getSharedEdge(const GNode *other) const
	{
		ASSERT(other);
		ASSERT(hasNeighbor(other));

		if(mEdge1->getNeighbor(this) == other) return mEdge1;
		if(mEdge2->getNeighbor(this) == other) return mEdge2;
		if(mEdge3->getNeighbor(this) == other) return mEdge3;

	}

	// Returns the vertex of the triangle that is shared by the edge that
	// contains V but is not the edge E
	inline const sm::Vertex *getOppositeVertex(const sm::Vertex *V, const GEdge *R) const
	{
		// get the edge that contains V but is not R
		if(mEdge1 && mEdge1 != R){
			// is a probably edge, check the easy case
			if(mEdge1->getSharedVertex()[0] == V){
				return mEdge1->getSharedVertex()[1];
			} else if(mEdge1->getSharedVertex()[1] == V){
				return mEdge1->getSharedVertex()[0];
			} else {
				// mEdge1 share some vertex with R, get it
				const sm::Vertex *shared = (R->getSharedVertex()[0] == V) ?
						R->getSharedVertex()[1] : R->getSharedVertex()[0];
				if(mEdge1->getSharedVertex()[0] == shared){
					return mEdge1->getSharedVertex()[1];
				} else {
					return mEdge1->getSharedVertex()[0];
				}
			}
		}
		if(mEdge2 && mEdge2 != R){
			if(mEdge2->getSharedVertex()[0] == V){
				return mEdge2->getSharedVertex()[1];
			} else if(mEdge2->getSharedVertex()[1] == V){
				return mEdge2->getSharedVertex()[0];
			} else {
				// mEdge1 share some vertex with R, get it
				const sm::Vertex *shared = (R->getSharedVertex()[0] == V) ?
						R->getSharedVertex()[1] : R->getSharedVertex()[0];
				if(mEdge2->getSharedVertex()[0] == shared){
					return mEdge2->getSharedVertex()[1];
				} else {
					return mEdge2->getSharedVertex()[0];
				}
			}
		}

		if(mEdge3 && mEdge3 != R){
			if(mEdge3->getSharedVertex()[0] == V){
				return mEdge3->getSharedVertex()[1];
			} else if(mEdge3->getSharedVertex()[1] == V){
				return mEdge3->getSharedVertex()[0];
			} else {
				// mEdge1 share some vertex with R, get it
				const sm::Vertex *shared = (R->getSharedVertex()[0] == V) ?
						R->getSharedVertex()[1] : R->getSharedVertex()[0];
				if(mEdge3->getSharedVertex()[0] == shared){
					return mEdge3->getSharedVertex()[1];
				} else {
					return mEdge3->getSharedVertex()[0];
				}
			}
		}

		ASSERT(false);
	}

	// Compare operator
	bool operator==(const GNode &other) const
    {
	    return (mTriangle == 0 && other.mTriangle == 0) ||
	            (*mTriangle == *other.mTriangle);
    }

private:
	const Triangle 	*mTriangle;
	const GEdge 	*mEdge1;
	const GEdge 	*mEdge2;
	const GEdge 	*mEdge3;
};

#endif /* GNODE_H_ */
