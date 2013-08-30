/*
 * CollCell.h
 *
 *  Created on: Aug 28, 2013
 *      Author: agustin
 */

#ifndef COLLCELL_H_
#define COLLCELL_H_


#include <vector>
#include <algorithm>

#include <types/BoolCountingMask.h>
#include <math/AABB.h>

#include "CollDefines.h"
#include "CollObject.h"


namespace coll {

// @brief This cells will contain the collision objects and we only will hold
//        them and check for collision at the bounding box level. More detailed
//        collision detection should be done in the Handler.
//        Each cell will handle two type of objects, the dynamic and the static
//        ones.
//


class CollCell
{
    // Helper class to sort the Static elements
    static inline bool cmpCollObj(const CollObject* a, const CollObject* b)
    {
        return (a->boundingBox().tl.x < b->boundingBox().tl.x);
    }

public:
    CollCell() {};
    ~CollCell() {};

    // @brief Add / remove a static element from this cell.
    //        The remove method will remove if exists only (and return true on
    //        such case, else false is returned)
    // @param sco       The static object to be added / removed.
    //
    inline void
    addStatic(CollObject* sco);
    inline bool
    removeStatic(CollObject* sco);

    // @brief Check if a static object is in this cell or not
    //
    inline bool
    containsStatic(CollObject* sco) const;

    // @brief Add / remove a dynamic object
    //        The remove method will remove if exists only (and return true on
    //        such case, else false is returned)
    // @param dco       The dynamic object to be added / removed.
    //
    inline void
    addDynamic(CollObject* dco);
    inline bool
    removeDynamic(CollObject* dco);

    // @brief Check if a dynamic object is in this cell or not
    //
    inline bool
    containsDynamic(CollObject* dco) const;

    // @brief Return all the collision objects that collide against co
    //        using the DetectType parameter (if we want to check static or
    //        dynamic or both).
    // @param co        The collision object we want to check against
    // @param dt        The detection type parameter (filter)
    // @param mask      The mask to be used
    // @param result    The resulting vector with the objects that collide with
    //                  co and satisfy the parameters.
    // @param bitset    The bitset used to avoid duplicated items
    // @returns true if some element collide with co, false otherwise
    // @note This method WILL NOT REMOVE THE PREVIOUS results of result.
    //
    inline bool
    getCollidingObjects(CollObject* co,
                        DetectType dt,
                        mask_t mask,
                        QueryResultVec& result,
                        core::BoolCountingMask<>& bitset);

    // @brief Clear the cell removing all its elements.
    //
    inline void
    clearAll(void);

private:
    // @brief Return the associated index of an element in an array or -1 if
    //        wasn't found.
    //
    template <typename T>
    inline int
    getIndex(const std::vector<T>& v, const T e) const;

private:
    std::vector<CollObject* > mStatics;
    std::vector<CollObject* > mDynamics;
};





////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

template <typename T>
inline int
CollCell::getIndex(const std::vector<T>& v, const T e) const
{
    for (int i = 0, size = v.size(); i < size; ++i) {
        if (v[i] == e) return i;
    }
    return -1;
}

inline void
CollCell::addStatic(CollObject* sco)
{
    ASSERT(sco);
    ASSERT(!containsStatic(sco));
    mStatics.push_back(sco);
    // maintain the order
    std::sort(mStatics.begin(), mStatics.end(), cmpCollObj);
}
inline bool
CollCell::removeStatic(CollObject* sco)
{
    ASSERT(sco);
    const int index = getIndex(mStatics, sco);
    if (index < 0) {
        return false;
    }
    // ugly slow remove but this will maintain the order
    mStatics.erase(mStatics.begin() + index);
    return true;
}

inline bool
CollCell::containsStatic(CollObject* sco) const
{
    return getIndex(mStatics, sco) >= 0;
}


inline void
CollCell::addDynamic(CollObject* dco)
{
    ASSERT(dco);
    ASSERT(!containsDynamic(dco));
    mDynamics.push_back(dco);
}
inline bool
CollCell::removeDynamic(CollObject* dco)
{
    ASSERT(dco);
    const int index = getIndex(mDynamics, dco);
    if (index < 0) {
        return false;
    }
    // swap with last because the order is not important
    mDynamics[index] = mDynamics.back();
    mDynamics.pop_back();
    return true;
}

inline bool
CollCell::containsDynamic(CollObject* dco) const
{
    ASSERT(dco);
    return getIndex(mDynamics, dco) >= 0;
}

inline bool
CollCell::getCollidingObjects(CollObject* co,
                              DetectType dt,
                              mask_t mask,
                              QueryResultVec& result,
                              core::BoolCountingMask<>& bitset)
{
    ASSERT(co);
    const unsigned int oldSize = result.size();

    // check for static first
    if ((dt & DetectType::CQ_Statics) && !mStatics.empty()) {
        for (std::vector<CollObject* >::iterator beg = mStatics.begin(),
                end = mStatics.end(); beg != end; ++beg) {
            CollObject* other = *beg;
            if (other->isCollisionsEnabled() &&   // check if is enable for collisions
                other != co &&                    // it is not the same object
                !(bitset[other->id]) &&           // the object was not set already
                (other->mask() & mask) &&         // matchs with the mask
                 other->collideBB(*co) // collide the bb's
                 ) {
                result.push_back(other); // put this into the resulting vector
                bitset.mark(other->id);  // mark it to avoid duplications
            }
        }
    }

    // check for dynamics now
    if ((dt & DetectType::CQ_Dynamics) && !mDynamics.empty()) {
        for (std::vector<CollObject* >::iterator beg = mDynamics.begin(),
                end = mDynamics.end(); beg != end; ++beg) {
            CollObject* other = *beg;
            if (other->isCollisionsEnabled() &&   // check if is enable for collisions
                other != co &&                    // it is not the same object
                !(bitset[other->id]) &&           // the object was not set already
                (other->mask() & mask) &&         // matchs with the mask
                 (other->boundingBox().collide(co->boundingBox())) // collide the bb's
                 ) {
                result.push_back(other); // put this into the resulting vector
                bitset.mark(other->id);  // mark it to avoid duplications
            }
        }
    }
    return oldSize != result.size();
}

inline void
CollCell::clearAll(void)
{
    mStatics.clear();
    mDynamics.clear();
}

} /* namespace coll */
#endif /* COLLCELL_H_ */
