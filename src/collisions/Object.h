/*
 * Object.h
 *
 *  Created on: Feb 17, 2013
 *      Author: agustin
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include <math/Vec2.h>
#include <math/AABB.h>
#include <types/basics.h>


namespace coll {


struct Object {
    // mask object (for collision groups)
    core::uint32_t groupMask;
    // the associated AABB
    core::AABB  aabb;
    // user defined type
    void *userDefined;

    Object() :
        userDefined(0)
    {}

private:
    friend class SpacePartition;
    friend class TwoDimCell;
    // the id of the object (index in manager), this id shouldn't me modified
    // by hand
    core::uint32_t id;
};


}

#endif /* OBJECT_H_ */
