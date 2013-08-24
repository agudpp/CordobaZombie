/*
 * TwoDimCell.h
 *
 *  Created on: Feb 20, 2013
 *      Author: agustin
 */

#ifndef TWODIMCELL_H_
#define TWODIMCELL_H_

#include <vector>

#include <math/AABB.h>
#include <math/Vector2.h>
#include <debug/DebugUtil.h>

#include "Object.h"
#include "Defines.h"

namespace coll {

// This represent a two dimensional cell where the space is divided like this
// (0,N)
//  ^                         o (X+, Y+)
//  |
//  |
//  |
//  |
//  |
//  |
//  |
//  |
//  |------------------------->
// (0,0)                      (N,0)

class TwoDimCell {
public:
    TwoDimCell(){}
    ~TwoDimCell(){}

    /**
     * @brief Add a new object to the cell. The object should be in the space
     *        of this cell.
     * @param obj   The object to be added
     */
    inline void addObject(const Object *obj);

    /**
     * @brief Remove a object from the cell
     * @param obj   The object to be removed
     */
    inline void removeObject(const Object *obj);
    inline void removeAll(void);

    /**
     * @brief Get the objects that "collide" for a specific one (use the groupMask)
     * @param obj       The object to be checked against
     * @param result    The list of objects that collide against the obj
     */
    inline void getCollisions(const Object *obj,
                              std::vector<const Object *> &result) const;

    // Query
    //

    /**
     * @brief Get the objects that collide against an AABB and using a mask
     * @param AABB      The AABB to be used
     * @param result    The vector where the results will be put
     * @param mask      The mask to be used if we want to use one
     */
    template<typename _T>
    inline void getCollisionQuery(const math::AABB<_T> &AABB,
                                  std::vector<const Object *> &result,
                                  const uint32_t mask = ~0) const;

    /**
     * @brief Get the objects that intersect a point and using a mask
     * @param point     The point to be used
     * @param result    The vector where the results will be put
     * @param mask      The mask to be used if we want to use one
     */
    template<typename _T>
    inline void getCollisionQuery(const math::Vector2<_T> &point,
                                  std::vector<const Object *> &result,
                                  const uint32_t mask = ~0) const;

private:
    // Auxiliary function to remove an object from the vector (and do a swap)
    //
    inline void swapAndRemove(const Object *obj);
    inline void swapAndRemove(const size_t index);

private:
    ConstObjectVec mObjects;
};

// Inline implementations
//

inline void
TwoDimCell::swapAndRemove(const size_t index)
{
    ASSERT(index < mObjects.size());
    mObjects[index] = mObjects[mObjects.size()-1];
    mObjects.pop_back();
}

inline void
TwoDimCell::swapAndRemove(const Object *obj)
{
    // ensure we have the object
    ASSERT(obj != 0);
    for(size_t i = 0, size = mObjects.size(); i < size; ++i){
        if (mObjects[i] == obj) {
            swapAndRemove(i);
            return;
        }
    }
    // ensure we are not trying to remove an invalid object
    ASSERT(false);
}

inline void
TwoDimCell::addObject(const Object *obj)
{
    ASSERT(obj != 0);
#ifdef DEBUG
    // ensure we haven't that object
    for(size_t i = 0, size = mObjects.size(); i < size; ++i){
        if (mObjects[i] == obj) {
            ASSERT(false);
        }
    }
#endif
    mObjects.push_back(obj);
}

inline void
TwoDimCell::removeObject(const Object *obj)
{
    swapAndRemove(obj);
}
inline void
TwoDimCell::removeAll(void)
{
    mObjects.clear();
}


inline void
TwoDimCell::getCollisions(const Object *obj,
                          std::vector<const Object *> &result) const
{
    // TODO: probably we can improve this using other data structure instead
    // of an vector (taking into account add / remove / find object velocity

    // get the dynamic ones
    for(size_t i = 0, size = mObjects.size(); i < size; ++i){
        const Object *aux = mObjects[i];
        if((obj != aux) &&
                (obj->mGroupMask & aux->mGroupMask) &&
                (obj->mAABB.collide(aux->mAABB))){
            // check for furder collision here... or not?
            result.push_back(aux);
        }
    }
}

template<typename _T>
inline void
TwoDimCell::getCollisionQuery(const math::AABB<_T> &AABB,
                              std::vector<const Object *> &result,
                              const uint32_t mask) const
{
    // get the dynamic ones
    for(size_t i = 0, size = mObjects.size(); i < size; ++i){
        const Object *aux = mObjects[i];
        if((aux->mGroupMask & mask) &&
                (AABB.collide(aux->mAABB))){
            // check for furder collision here... or not?
            result.push_back(aux);
        }
    }
}

template<typename _T>
inline void
TwoDimCell::getCollisionQuery(const math::Vector2<_T> &point,
                              std::vector<const Object *> &result,
                              const uint32_t mask) const
{
    // get the dynamic ones
    for(size_t i = 0, size = mObjects.size(); i < size; ++i){
        const Object *aux = mObjects[i];
        if((aux->mGroupMask & mask) &&
                (aux->mAABB.checkPointInside(point))){
            // check for furder collision here... or not?
            result.push_back(aux);
        }
    }
}

}

#endif /* TWODIMCELL_H_ */
