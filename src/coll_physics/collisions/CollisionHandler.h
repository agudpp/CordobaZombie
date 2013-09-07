/*
 * CollisionHandler.h
 *
 *  Created on: Aug 27, 2013
 *      Author: agustin
 */

#ifndef COLLISIONHANDLER_H_
#define COLLISIONHANDLER_H_

#include <vector>

#include <math/AABB.h>
#include <debug/DebugUtil.h>
#include <space_partition/StaticMatrixPartition.h>
#include <types/ContiguousCont.h>
#include <types/StackQueue.h>
#include <types/StackVector.h>
#include <types/BoolCountingMask.h>

#include "CollDefines.h"
#include "CollCell.h"
#include "CollObject.h"
#include "CollPreciseInfo.h"

namespace coll {

// @brief This class will handle the collisions in all the world, it will
//        be implemented with the CStyle instead of C++ for some methods (like
//        an API).
//        We will hardcode the divisions of the world to create a more transparent
//        API for the user (this way we can change the implementation of the
//        space partition algorithm without any modification).
//

struct CollPreciseInfo;


class CollisionHandler
{
    // Define the size of the matrix to be used
    //
    static const unsigned int NUM_COLS = 15;
    static const unsigned int NUM_ROWS = 15;
    static const unsigned int MAX_MATRIX_INDICES = NUM_COLS*NUM_ROWS;

    // TODO: BIG LIMITATION WITH THIS! BIG!!! and very memory expensive but
    //       for this particular project will work extremely perfect :p.
    //       We will put an upper bound to the number of collision objects
    //       that we can handle at the same time in the world, and we will
    //       preallocate all of them at the begining to have a contiguous memory
    //       block
    //
    static const unsigned int MAX_NUM_COLLOBJECTS = 150;

public:
    CollisionHandler();
    ~CollisionHandler();

    ////////////////////////////////////////////////////////////////////////////
    //                          LifeTime functions                            //
    ////////////////////////////////////////////////////////////////////////////

    // @brief Get a new CollisionObject to be used.
    //        YOU SHOULD NOT HAVE TO FREE THE ALLOCATED MEMORY!
    // @brief mask      The collision mask to be used
    // @brief aabb      The current bounding box
    // @brief userDef   The user defined information
    // @brief pi        The precise info associated to the object.
    // @return the new CollObject.
    CollObject*
    getNewCollObject(mask_t mask = ~0,
                     const core::AABB& aabb = core::AABB(),
                     void* userDef = 0,
                     CollPreciseInfo* pi = 0);

    // @brief Delete an existing collision object. After you call this method
    //        the object is destroyed and removed from the collision system.
    //        Be careful with referencing the pointer after calling this method.
    // @param co        The collision object to be removed
    //
    void
    deleteCollObject(CollObject* co);

    // @brief Check if a collision object exists or not.
    // @param co        The collision object to check.
    // @return true if still exists | false otherwise
    //
    inline bool
    exists(const CollObject* co) const;

    // @brief Return the maximum number of objects that we can track at the same
    //        time (statics + dynamic).
    //
    inline unsigned int
    maxNumberObjects(void) const;

    ////////////////////////////////////////////////////////////////////////////
    //                          CollObject functions                          //
    ////////////////////////////////////////////////////////////////////////////

    // @note That all these functions will be only valid for dynamic objects
    //       and not static ones
    //

    // @brief Insert an static collision object (that will not be moved along
    //        the time).
    //        If for any reason you change the static object position you will
    //        get undefined behaviors since it will be not updated internally.
    // @param sco       The static collision object
    // @note This method is slow since we are rebuilding the internal structure
    //       of the Handler. So this should be performed offline (non main loop)
    //       if possible
    //
    void
    coAddStatic(CollObject* sco);

    // @brief Remove an static collision object already added.
    // @param sco       The static coll object to be removed
    // @note This method is slow since we are rebuilding the internal structure
    //       of the Handler. So this should be performed offline (non main loop)
    //       if possible
    //
    void
    coRemoveStatic(CollObject* sco);

    // @brief Insert a dynamic collision object to be tracked.
    // @param dco        The collision object we want to add.
    //
    void
    coAddDynamic(CollObject* dco);

    // @brief Remove a dynamic collision object.
    // @param dco        The collision object we want to remove.
    //
    void
    coRemoveDynamic(CollObject* dco);

    ////////////////////////////////////////////////////////////////////////////
    //                  CollisionHandler General functions                    //
    ////////////////////////////////////////////////////////////////////////////

    // @brief Set the world size where we will check for collisions and move the
    //        object around it.
    // @param wbb       The world bounding box.
    // @note THIS method will destroy everything and remove all the elements
    //       So calling this method is the same than reconstructing the CollHandler.
    //       Will call automaticall destroy() first.
    //
    void
    setWorldBoundingBox(const core::AABB& wbb);
    inline const core::AABB&
    worldBoundingBox(void) const;

    // @brief Destroy all the world and all the collision objects.
    // @note  THIS WILL INVALIDATE ALL THE POINTERS TO THE COLLOBJECTS!!!!
    //
    void
    destroy(void);

    // @brief Update the collision world. This method should be called each frame
    //        you will perform any query (before performing any query and after
    //        updating all the collision objects). If you don't update the
    //        collision world you will get incorrect results when perform queries
    //
    void
    update(void);

    ////////////////////////////////////////////////////////////////////////////
    //                  CollisionHandler Queries functions                    //
    ////////////////////////////////////////////////////////////////////////////

    // @brief Perform a query for a particular collision object.
    // @param co        The collision object we want to check.
    // @param args      The query args
    // @param result    The results of the query.
    // @return true on success and if we get some result | false otherwise
    //
    bool
    performQuery(CollObject* co, const QueryArgs& args, QueryResult& result);

    // TODO: add the other querys here like Bounding box query / Line query /
    //       position query / Sphere query? ... etc.
    //


private:
    // avoid copying
    //
    CollisionHandler(const CollisionHandler&);
    CollisionHandler& operator=(const CollisionHandler&);

    // Define some useful methods
    //

    // @brief Set all the CollisionObjects to be free to be used.
    //
    void
    letAllObjectsFreeAvailable(void);

    // @brief Get the index of a particular element in the mCurrentObjects
    //        vector.
    // @return -1 if the element wasn't found, index if we found it.
    //
    inline int
    findObject(CollObject* o);

private:
    // Define the matrix to be used here and the world bounding box
    //
    core::StaticMatrixPartition<CollCell, NUM_COLS, NUM_ROWS> mMatrix;

    // define the contiguous array of CollisionObject to be used, for now
    // we will use a ContiguousContainer. Note the limitation above
    //
    core::ContiguousContainer<CollObject> mCollObjects;
    core::StackVector<core::AABB, MAX_NUM_COLLOBJECTS> mOldAABB;
    core::StackQueue<CollObject*, MAX_NUM_COLLOBJECTS> mAvailableObjs;
    core::StackVector<CollObject*, MAX_NUM_COLLOBJECTS> mCurrentObjects;

    // We will use the BoolCountingMask to mark all the elements that were
    // already selected in each query (the result should contain unique elements).
    //
    core::BoolCountingMask<> mMask;
};







////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline int
CollisionHandler::findObject(CollObject* o)
{
    for (int i = 0, size = mCurrentObjects.size(); i < size; ++i) {
        if (mCurrentObjects[i] == o) return i;
    }
    return -1;
}

inline bool
CollisionHandler::exists(const CollObject* co) const
{
    ASSERT(co);
    return co->id < mCollObjects.size &&
        (&(mCollObjects[co->id]) == co);
}

inline unsigned int
CollisionHandler::maxNumberObjects(void) const
{
    return MAX_NUM_COLLOBJECTS;
}

inline const core::AABB&
CollisionHandler::worldBoundingBox(void) const
{
    return mMatrix.boundingBox();
}




} /* namespace coll */
#endif /* COLLISIONHANDLER_H_ */
