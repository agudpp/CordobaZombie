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
    CollCell() : mStaticCount(0) {};
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

    // @brief Return all the collision objects that collide against a particular
    //        point.
    // @param point     The point to test against.
    // @param dt        The detection type parameter (filter)
    // @param mask      The mask to be used
    // @param result    The resulting vector with the objects that collide with
    //                  co and satisfy the parameters.
    // @param bitset    The bitset used to avoid duplicated items
    // @returns true if some element collide with co, false otherwise
    // @note This method WILL NOT REMOVE THE PREVIOUS results of result.
    //
    inline bool
    getCollidingObjects(const core::Vector2& point,
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
    inline int
    getStaticIndex(const CollObject* e) const;
    inline int
    getDynamicIndex(const CollObject* e) const;

private:
    std::vector<CollObject* > mObjects;
    unsigned int mStaticCount;
};





////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline int
CollCell::getStaticIndex(const CollObject* e) const
{
    for (int i = 0; i < mStaticCount; ++i) {
        if (mObjects[i] == e) return i;
    }
    return -1;
}
inline int
CollCell::getDynamicIndex(const CollObject* e) const
{
    for (int i = mStaticCount, size = mObjects.size(); i < size; ++i) {
        if (mObjects[i] == e) return i;
    }
    return -1;
}

inline void
CollCell::addStatic(CollObject* sco)
{
    ASSERT(sco);
    ASSERT(!containsStatic(sco));
    // we will put the element in the last element of the Static elements and
    // then sort it
    if (mObjects.size() == mStaticCount){
        // we can safetly put the element at the end since we have not
        // dynamic elements
        mObjects.push_back(sco);
    } else {
        // we need to move the first dynamic at the end and put the static
        // element in the last position of the static elements
        mObjects.push_back(mObjects[mStaticCount]);
        mObjects[mStaticCount] = sco;
    }

    // in any case increase the size of stati objects
    ++mStaticCount;

    // maintain the order of the static elements only
    std::sort(mObjects.begin(), mObjects.begin() + mStaticCount, cmpCollObj);
}
inline bool
CollCell::removeStatic(CollObject* sco)
{
    ASSERT(sco);
    const int index = getStaticIndex(sco);
    if (index < 0) {
        return false;
    }
    ASSERT(mStaticCount > 0);
    // we will need to:
    // 1) Swap the current element with the last of the static ones.
    // 2) Swap the last static with the last dynamic
    // 3) Pop the last of the array.
    // 4) Sort again the static elements
    //
    --mStaticCount;
    mObjects[index] = mObjects[mStaticCount];   // 1
    mObjects[mStaticCount] = mObjects.back();   // 2
    mObjects.pop_back();                        // 3
    std::sort(mObjects.begin(),
              mObjects.begin() + mStaticCount,
              cmpCollObj);                      // 4
    return true;
}

inline bool
CollCell::containsStatic(CollObject* sco) const
{
    return getStaticIndex(sco) >= 0;
}


inline void
CollCell::addDynamic(CollObject* dco)
{
    ASSERT(dco);
    ASSERT(!containsDynamic(dco));
    // we will just put it at the end, this will not modify anything of the
    // static elements
    mObjects.push_back(dco);
}
inline bool
CollCell::removeDynamic(CollObject* dco)
{
    ASSERT(dco);
    const int index = getDynamicIndex(dco);
    if (index < 0) {
        return false;
    }
    // swap with last because the order is not important
    mObjects[index] = mObjects.back();
    mObjects.pop_back();
    return true;
}

inline bool
CollCell::containsDynamic(CollObject* dco) const
{
    ASSERT(dco);
    return getDynamicIndex(dco) >= 0;
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
    if ((dt & DetectType::CQ_Statics) && mStaticCount > 0) {
        for (std::vector<CollObject* >::iterator beg = mObjects.begin(),
                end = mObjects.begin() + mStaticCount; beg != end; ++beg) {
            CollObject* other = *beg;
            if (other->isCollisionsEnabled() &&   // check if is enable for collisions
                other != co &&                    // it is not the same object
                !(bitset[other->id]) &&           // the object was not set already
                (other->mask() & mask) &&         // matches with the mask
                 (other->boundingBox().collide(co->boundingBox())) // collide the bb's
                 ) {
                result.push_back(other); // put this into the resulting vector
                bitset.mark(other->id);  // mark it to avoid duplications
            }
        }


        /*
         *  TODO: this shit is not working properly!
        // we will do not a binary search here but we will start from the middle
        // and check until we need.
        const unsigned int middle = mStaticCount >> 1; // = mStaticCount / 2
        int index = middle;
        const float &left = co->boundingBox().tl.x;
        const float &right = co->boundingBox().br.x;

        // move left first
        CollObject* other = mObjects[index];
        for (; index >= 0 && (left <= other->boundingBox().br.x); --index) {
            other = mObjects[index];
            if (other->isCollisionsEnabled() &&   // check if is enable for collisions
                other != co &&                    // it is not the same object
                !(bitset[other->id]) &&           // the object was not set already
                (other->mask() & mask) &&         // matches with the mask
                 other->collideBB(*co) // collide the bb's
                 ) {
                result.push_back(other); // put this into the resulting vector
                bitset.mark(other->id);  // mark it to avoid duplications
            }
        }

        // move right
        index = middle+1;
        if (index < mStaticCount) {
            other = mObjects[index];
            for (; index < mStaticCount && (right <= other->boundingBox().tl.x); ++index) {
                other = mObjects[index];
                if (other->isCollisionsEnabled() &&   // check if is enable for collisions
                    other != co &&                    // it is not the same object
                    !(bitset[other->id]) &&           // the object was not set already
                    (other->mask() & mask) &&         // matches with the mask
                     other->collideBB(*co) // collide the bb's
                     ) {
                    result.push_back(other); // put this into the resulting vector
                    bitset.mark(other->id);  // mark it to avoid duplications
                }
            }
        }*/
    }

    // check for dynamics now
    if ((dt & DetectType::CQ_Dynamics) && (mObjects.size() > mStaticCount)) {
        for (std::vector<CollObject* >::iterator beg = mObjects.begin() + mStaticCount,
                end = mObjects.end(); beg != end; ++beg) {
            CollObject* other = *beg;
            if (other->isCollisionsEnabled() &&   // check if is enable for collisions
                other != co &&                    // it is not the same object
                !(bitset[other->id]) &&           // the object was not set already
                (other->mask() & mask) &&         // matches with the mask
                 (other->boundingBox().collide(co->boundingBox())) // collide the bb's
                 ) {
                result.push_back(other); // put this into the resulting vector
                bitset.mark(other->id);  // mark it to avoid duplications
            }
        }
    }
    return oldSize != result.size();
}

inline bool
CollCell::getCollidingObjects(const core::Vector2& point,
                              DetectType dt,
                              mask_t mask,
                              QueryResultVec& result,
                              core::BoolCountingMask<>& bitset)
{
    const unsigned int oldSize = result.size();

    if ((dt & DetectType::CQ_Statics) && mStaticCount > 0) {
        for (std::vector<CollObject* >::iterator beg = mObjects.begin(),
            end = mObjects.begin() + mStaticCount; beg != end; ++beg) {
            CollObject* other = *beg;
            if (other->isCollisionsEnabled() &&   // check if is enable for collisions
                !(bitset[other->id]) &&           // the object was not set already
                (other->mask() & mask) &&         // matches with the mask
                (other->boundingBox().checkPointInside(point)) // the point is inside
                 ) {
                result.push_back(other); // put this into the resulting vector
                bitset.mark(other->id);  // mark it to avoid duplications
            }
        }
    }
    // check for dynamics now
    if ((dt & DetectType::CQ_Dynamics) && (mObjects.size() > mStaticCount)) {
        for (std::vector<CollObject* >::iterator beg = mObjects.begin() + mStaticCount,
                end = mObjects.end(); beg != end; ++beg) {
            CollObject* other = *beg;
            if (other->isCollisionsEnabled() &&   // check if is enable for collisions
                !(bitset[other->id]) &&           // the object was not set already
                (other->mask() & mask) &&         // matches with the mask
                (other->boundingBox().checkPointInside(point)) // the point is inside
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
    mObjects.clear();
    mStaticCount = 0;
}

} /* namespace coll */
#endif /* COLLCELL_H_ */
