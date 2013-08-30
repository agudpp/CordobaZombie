/*
 * COLLDefines.h
 *
 *  Created on: Feb 17, 2013
 *      Author: agustin
 */

#ifndef COLLDEFINES_H_
#define COLLDEFINES_H_


#include <types/basics.h>
#include <types/StackVector.h>

// we will define some of the types used in the collision system
//

namespace coll {

// Forward
//
class CollObject;

// Define the mask type
//
typedef core::uint16_t mask_t;

// index type
typedef core::uint16_t index_t;


// More precise information for collisions... we will define this here to
// avoid circular dependencies in the header files

struct PreciseInfo {
    // TODO: add here
};


// We will define the different type of queries results / arguments here.
//
static const unsigned int MAX_QUERY_RESULT = 45;
static const unsigned int MAX_COLL_POINTS = 5;

typedef core::StackVector<CollObject*, MAX_QUERY_RESULT> QueryResultVec;
typedef core::StackVector<core::Vector2, MAX_COLL_POINTS> CollPointVec;
typedef core::StackVector<CollPointVec, MAX_QUERY_RESULT> QueryResultPointVec;

// Define the arguments
//
enum DetectType {
    CQ_Statics   = (1 << 0),         // Collision Query Detect Statics
    CQ_Dynamics  = (1 << 1),         // Collision Query Detect Dynamic
    CQ_All       = (CQ_Statics | CQ_Dynamics),
};

enum PrecisionType {
    CQ_BoundingBox          = (1 << 0), // Bounding box only
    CQ_CollPoints           = (1 << 1), // Detect collision points also
};

struct QueryArgs {
    DetectType dtype;
    PrecisionType ptype;
    mask_t mask;

    QueryArgs(DetectType dt = DetectType::CQ_All,
              PrecisionType pt = PrecisionType::CQ_BoundingBox,
              mask_t m = ~0) :
        dtype(dt)
    ,   ptype(pt)
    ,   mask(m)
    {}
};

// Define the results
struct QueryResult {
    // This vector will contain the collision objects detected in the query.
    QueryResultVec objects;
    // This vector could (or not) contain the collision points (manifold points)
    QueryResultPointVec points;
};




}


#endif /* COLLDEFINES_H_ */
