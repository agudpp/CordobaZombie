/*
 * CollisionHandler.cpp
 *
 *  Created on: Aug 27, 2013
 *      Author: agustin
 */

#include "CollisionHandler.h"

#include <bitset>

#include <Box2D/Collision/b2Collision.h>
#include <Box2D/Collision/Shapes/b2Shape.h>

#include "CollObject.h"


// Helper stuff
//
namespace {

struct IndexFlagPair {
    enum State {
        REMOVE = 0,
        MAINTAIN,
        ADD
    };

    core::size_t index;
    State state;
};

// @brief This method will return a list of pairs (index, bool) indicating
//        if the element should be removed from the cells[index] (if the
//        boolean flag is true, or the element should be maintained if the
//        boolean flag is false).
// @param obb       The old bouding box
// @param nbb       The new bunding box
// @param matrix    The matrix used
// @param result    The resulting list
// @note The complexity of this algorithm is O(M+N) where M is the
//       number of cells that obb intersect and N is the number of cells
//       that nbb intersects. So it is fast.
//
template <unsigned int C, unsigned int R>
inline void
checkForIntersections(const core::AABB& obb,
                      const core::AABB& nbb,
                      core::StaticMatrixPartition<coll::CollCell, C, R>& matrix,
                      core::StackVector<IndexFlagPair, C*R>& result)
{
    // if the size C*R is small (for example 15*15 = 225, this will only take
    // 8 bytes approx = 2 words (int in a 32bit machine) to initialize so
    // is really fast...
    //
    std::bitset<C*R> newIDS;
    unsigned int indexRef[C*R];
    newIDS.reset();
    core::StackVector<core::size_t, C*R> indices;

    result.clear();

    // First we will iterate over all the new indices that we should need to add
    // (at least for now). And we will save the index of the IndexFlagPair vector
    // in indexRef. This indexRef is a map for Cell_index -> IndexFlagPair_index.
    // This way we will be able to know the position of the element to update it
    // easily later
    //
    matrix.getCellsIndices(nbb, indices);
    core::size_t* beg = indices.begin(), *end = indices.end();
    while (beg != end) {
        ASSERT(*beg < C*R);
        newIDS[*beg] = 1;
        indexRef[*beg] = result.size();
        result.push_back({*beg, IndexFlagPair::State::ADD});
        ++beg;
    }

    // now we will get all the old indices (that intersect the old bb).
    // All these indices should be marked as "REMOVE" iff is not in the bitset.
    // If the element is in the bitset these means that the new bounding box
    // will maintain its position in the same cell (so we need to mark it as
    // MAINTAIN)
    //
    matrix.getCellsIndices(obb, indices);
    beg = indices.begin(); end = indices.end();
    while (beg != end) {
        ASSERT(*beg < C*R);
        if ((newIDS[*beg])) {
            // we need to maintain this position, so we update the status
            ASSERT(indexRef[*beg] < C*R);
            result[indexRef[*beg]].state = IndexFlagPair::State::MAINTAIN;
        } else {
            // we should remove this position
            result.push_back({*beg, IndexFlagPair::State::REMOVE});
        }
        ++beg;
    }
}


////////////////////////////////////////////////////////////////////////////////
// Box2D Functions
//


}


namespace coll {


////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::letAllObjectsFreeAvailable(void)
{
    mAvailableObjs.clear();
    CollObject* beg = mCollObjects.begin(), *end = mCollObjects.end();
    unsigned int i = 0;
    while (beg != end) {
        beg->reset();
        beg->id = i++;
        beg->flags.enabled = true;
        mAvailableObjs.push_back(beg);
        ++beg;
    }
    mCurrentObjects.clear();
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CollisionHandler::CollisionHandler()
{
    // allocate the memory needed
    mCollObjects.size = MAX_NUM_COLLOBJECTS;
    mCollObjects.data = new CollObject[MAX_NUM_COLLOBJECTS];
    letAllObjectsFreeAvailable();
    mMask.setSize(MAX_NUM_COLLOBJECTS);
    mOldAABB.resize(MAX_NUM_COLLOBJECTS);
}

////////////////////////////////////////////////////////////////////////////////
CollisionHandler::~CollisionHandler()
{
    delete []mCollObjects.data;
}

////////////////////////////////////////////////////////////////////////////
//                          LifeTime functions                            //
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
CollObject*
CollisionHandler::getNewCollObject(mask_t mask,
                                   const core::AABB& aabb,
                                   void* userDef,
                                   CollPreciseInfo* pi)
{
    if (mAvailableObjs.empty()) {
        debugWARNING("You are asking for more collision objects that we expect: "
            "%d\n", mAvailableObjs.max_size());
        return 0;
    }

    // configure the object
    CollObject* obj = mAvailableObjs.front();
    mAvailableObjs.pop_front();

    obj->setMask(mask);
    obj->setUserDef(userDef);
    obj->setBoundingBox(aabb);
    if (pi) {
        obj->setPreciseInfo(pi);
    }

    return obj;
}

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::deleteCollObject(CollObject* co)
{
    ASSERT(co);
    ASSERT(exists(co));

    // we will remove the element from the current cells
    core::StackVector<CollCell*, MAX_MATRIX_INDICES> cells;
    mMatrix.getCells(co->boundingBox(), cells);
    if (!cells.empty()) {
        // we need to remove from the cells first
        CollCell** beg = cells.begin(), **end = cells.end();
        while (beg != end) {
            ASSERT(beg);
            const bool dummy = (*beg)->removeDynamic(co) || (*beg)->removeStatic(co);
            ASSERT(dummy); // if not we are trying to remove an inexistent element?
            ++beg;
        }
    }

    // now we are sure that the pointer is not anymore in the cells. Remove the
    // Precise info if needed and put the poniter into the available queue
    co->reset();
    mAvailableObjs.push_back(co);

    // remove it from the current objects
    const int index = findObject(co);
    if (index >= 0) {
        mCurrentObjects.disorder_remove(index);
    }

    ASSERT(!exists(co));
}


////////////////////////////////////////////////////////////////////////////
//                          CollObject functions                          //
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::coAddStatic(CollObject* sco)
{
    ASSERT(sco);
    ASSERT(exists(sco));

    // check if the element already exists in the cell
    core::StackVector<CollCell*, MAX_MATRIX_INDICES> cells;
    mMatrix.getCells(sco->boundingBox(), cells);
    for (CollCell** beg = cells.begin(), **end = cells.end(); beg != end; ++beg) {
        if (!(*beg)->containsStatic(sco)) {
            (*beg)->addStatic(sco);
        }
    }


    sco->flags.enabled = true;
    sco->flags.onWorld = true;
    sco->flags.isStatic = true;

    // note that we don't need to analyze any static object because it is impossible
    // that it can change (if not, is not an static object).
    //
}

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::coRemoveStatic(CollObject* sco)
{
    ASSERT(sco);
    ASSERT(exists(sco));

    // check if the element already exists in the cell
    core::StackVector<CollCell*, MAX_MATRIX_INDICES> cells;
    mMatrix.getCells(sco->boundingBox(), cells);
    for (CollCell** beg = cells.begin(), **end = cells.end(); beg != end; ++beg) {
        (*beg)->removeStatic(sco);
    }

    sco->flags.onWorld = false;
    sco->flags.enabled = false;
}

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::coAddDynamic(CollObject* dco)
{
    ASSERT(dco);
    ASSERT(exists(dco));

    // check if the element already exists in the cell
    core::StackVector<CollCell*, MAX_MATRIX_INDICES> cells;
    mMatrix.getCells(dco->boundingBox(), cells);
    for (CollCell** beg = cells.begin(), **end = cells.end(); beg != end; ++beg) {
        if (!(*beg)->containsDynamic(dco)) {
            (*beg)->addDynamic(dco);
        }
    }

    // add the object to analyze it
    ASSERT(findObject(dco) < 0);
    mCurrentObjects.push_back(dco);

    // enable collisions
    dco->flags.enabled = true;
    dco->flags.onWorld = true;
    dco->flags.isStatic = false;

    // we will also add the last bounding box of this object used when we add
    // it in the cells
    //
    mOldAABB[dco->id] = dco->boundingBox();
}

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::coRemoveDynamic(CollObject* dco)
{
    ASSERT(dco);
    ASSERT(exists(dco));

    // check if the element already exists in the cell
    core::StackVector<CollCell*, MAX_MATRIX_INDICES> cells;
    mMatrix.getCells(mOldAABB[dco->id], cells);
    for (CollCell** beg = cells.begin(), **end = cells.end(); beg != end; ++beg) {
        (*beg)->removeDynamic(dco);
    }

    // disable collisions
    dco->flags.enabled = false;
    dco->flags.onWorld = false;

    // remove it from the current objects to analyze
    const int index = findObject(dco);
    ASSERT(index >= 0);
    if (index >= 0) {
        mCurrentObjects.disorder_remove(index);
    }
}

////////////////////////////////////////////////////////////////////////////
//                  CollisionHandler General functions                    //
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::setWorldBoundingBox(const core::AABB& wbb)
{
    // we will configure the StatickMatrix and remove all the information inside
    // of the matrix
    //
    destroy();
    mMatrix.construct(wbb);
}

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::destroy(void)
{
    core::size_t i = 0;
    while (mMatrix.isIndexValid(i)) {
        CollCell& cell = mMatrix.getCell(i);
        cell.clearAll();
        ++i;
    }
    letAllObjectsFreeAvailable();
}

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::update(void)
{
    // we will check each of the objects we need to update and perform the
    // needed operations.
    //
    CollObject** objBeg = mCurrentObjects.begin(), **objEnd = mCurrentObjects.end();
    core::StackVector<IndexFlagPair, MAX_MATRIX_INDICES> indices;
    while (objBeg != objEnd) {
        ASSERT(objBeg);
        CollObject* current = *objBeg;
        if (current->flags.dirty && current->flags.enabled) {
            // we need to update this one
            indices.clear();
            checkForIntersections(mOldAABB[current->id],
                                  current->boundingBox(),
                                  mMatrix,
                                  indices);
            // now remove the element from the current indices and add it where it is
            // needed
            IndexFlagPair* item = indices.begin(), *itemEnd = indices.end();
            while (item != itemEnd) {
                CollCell& cell = mMatrix.getCell(item->index);
                switch (item->state) {
                case IndexFlagPair::State::REMOVE:
                    // we need to remove it
                    cell.removeDynamic(current);
                    break;
                case IndexFlagPair::State::MAINTAIN:
                    // do nothing
                    break;
                case IndexFlagPair::State::ADD:
                    // we need to add it
                    cell.addDynamic(current);
                    break;
                }
                ++item;
            }

            current->flags.dirty = false;
            mOldAABB[current->id] = current->boundingBox();
        }
        ++objBeg;
    }
}

////////////////////////////////////////////////////////////////////////////
//                  CollisionHandler Queries functions                    //
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
bool
CollisionHandler::performQuery(CollObject* co,
                               const QueryArgs& args,
                               QueryResult& result)
{
    ASSERT(co);
    ASSERT(exists(co));

    // clear the result
    result.objects.clear();
    result.points.clear();

    // check if this object is ready to perform a query
    if (!(co->isCollisionsEnabled())) {
        debugWARNING("You are trying to get the collision objects that collide with"
            " a disabled one or that is not being tracked by the Handler!\n");
        return false;
    }

    // Init the new query and get all the cells that intersect the object
    //
    mMask.newRound();
    core::StackVector<CollCell*, MAX_MATRIX_INDICES> cells;
    mMatrix.getCells(co->boundingBox(), cells);
    for (CollCell** beg = cells.begin(), **end = cells.end(); beg != end; ++beg) {
        (*beg)->getCollidingObjects(co, args.dtype, args.mask, result.objects, mMask);
    }

    // now we should check if we need to do a more precise checking
    if (args.ptype & PrecisionType::CQ_PreciseCheck) {
        if (args.ptype & PrecisionType::CQ_CollPoints) {
            // yes we should
            ASSERT(false && "We need to implement this!!!!"); // TODO
        } else {
            // we only need to check for overlappings, it is a little more easy
            QueryResultVec& objs = result.objects;
            for (unsigned int i = 0; i < objs.size(); ++i) {
                if (!(objs[i]->collidePrecise(*co))) {
                    // remove this one
                    objs.disorder_remove(i);
                    --i;
                }
            }
        }
    }

    return !result.objects.empty();
}

// TODO: add the other querys here like Bounding box query / Line query /
//       position query / Sphere query? ... etc.
//


} /* namespace coll */
