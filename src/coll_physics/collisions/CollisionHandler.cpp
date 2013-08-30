/*
 * CollisionHandler.cpp
 *
 *  Created on: Aug 27, 2013
 *      Author: agustin
 */

#include "CollisionHandler.h"

#include <bitset>

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
        beg->flags.beingTracked = false;
        beg->flags.enabled = true;
        mAvailableObjs.push_back(beg);
        ++beg;
    }
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CollisionHandler::CollisionHandler()
{
    // set that all the CollObject will use this instance of the CollisionHandler
    //
    CollObject::setCollisionHandler(this);

    // allocate the memory needed
    mCollObjects.size = MAX_NUM_COLLOBJECTS;
    mCollObjects.data = new CollObject[MAX_NUM_COLLOBJECTS];
    letAllObjectsFreeAvailable();
    mMask.setSize(MAX_NUM_COLLOBJECTS);
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
                                   PreciseInfo* pi)
{
    if (mAvailableObjs.empty()) {
        debugWARNING("You are asking for more collision objects that we expect: "
            "%d\n", mAvailableObjs.max_size());
        return 0;
    }

    // configure the object
    CollObject* obj = mAvailableObjs.front();
    mAvailableObjs.pop_front();

    obj->flags.beingTracked = false;
    obj->setMask(mask);
    obj->setUserDef(userDef);
    obj->setBoundingBox(aabb);
    obj->setUserDef(pi);

    return obj;
}

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::deleteCollObject(CollObject* co)
{
    ASSERT(co);
    ASSERT(exists(co));

    // we are not tracking this object anymore
    co->flags.beingTracked = false;

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
    ASSERT(!exists(co));
}


////////////////////////////////////////////////////////////////////////////
//                          CollObject functions                          //
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::coUpdateBB(CollObject* co, const core::AABB& nbb)
{
    ASSERT(co);
    ASSERT(exists(co));

    if (!(co->flags.beingTracked)) {
        // we only need to update the bounding box and nothing else
        co->mAABB = nbb;
        nbb.getCenter(co->mCenter);
        return;
    }

    core::StackVector<IndexFlagPair, MAX_MATRIX_INDICES> indices;
    checkForIntersections(co->boundingBox(),
                          nbb,
                          mMatrix,
                          indices);
    // now remove the element from the current indices and add it where it is
    // needed
    IndexFlagPair* beg = indices.begin(), *end = indices.end();
    while (beg != end) {
        CollCell& cell = mMatrix.getCell(beg->index);
        switch (beg->state) {
        case IndexFlagPair::State::REMOVE:
            // we need to remove it
            cell.removeDynamic(co);
            break;
        case IndexFlagPair::State::MAINTAIN:
            // do nothing
            break;
        case IndexFlagPair::State::ADD:
            // we need to add it
            cell.addDynamic(co);
            break;
        }
        ++beg;
    }

    // set the new bounding box and center position
    co->mAABB = nbb;
    nbb.getCenter(co->mCenter);
}

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::coUpdatePreciseInfo(CollObject* co, PreciseInfo* pi)
{
    // take into account the co->flags.beingTracked flag.
    ASSERT(false && "TODO: we need to implement this..."); // TODO
}

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::coTranslate(CollObject* co, const core::Vector2& tvec)
{
    ASSERT(co);
    ASSERT(exists(co));

    // we will just call the updateBB
    core::AABB nbb = co->mAABB;
    nbb.translate(tvec);
    coUpdateBB(co, nbb);
}

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::coSetPosition(CollObject* co, const core::Vector2& npos)
{
    ASSERT(co);
    ASSERT(exists(co));

    coTranslate(co, npos - co->center());
}

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::coEnableCollisions(CollObject* co, bool enable)
{
    ASSERT(co);
    ASSERT(exists(co));

    // we can remove the elements here, but we will use a flag instead..
    // Note that this will let the co inside of the cells and will decrease
    // the performance when checking in other rounds
    //
    co->flags.enabled = enable;
}

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::coAddStatic(CollObject* sco)
{
    ASSERT(sco);
    ASSERT(exists(sco));

    // if the object is already being tracked, we have to do nothing
    if (sco->flags.beingTracked) {
        return;
    }

    // check if the element already exists in the cell
    core::StackVector<CollCell*, MAX_MATRIX_INDICES> cells;
    mMatrix.getCells(sco->boundingBox(), cells);
    for (CollCell** beg = cells.begin(), **end = cells.end(); beg != end; ++beg) {
        if (!(*beg)->containsStatic(sco)) {
            (*beg)->addStatic(sco);
        }
    }

    // this object is tracked now
    sco->flags.beingTracked = true;
}

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::coRemoveStatic(CollObject* sco)
{
    ASSERT(sco);
    ASSERT(exists(sco));

    // check if the object is being tracked
    if (!(sco->flags.beingTracked)) {
        return;
    }

    // check if the element already exists in the cell
    core::StackVector<CollCell*, MAX_MATRIX_INDICES> cells;
    mMatrix.getCells(sco->boundingBox(), cells);
    for (CollCell** beg = cells.begin(), **end = cells.end(); beg != end; ++beg) {
        (*beg)->removeStatic(sco);
    }

    // the object is not tracked anymore
    sco->flags.beingTracked = false;
}

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::coAddDynamic(CollObject* dco)
{
    ASSERT(dco);
    ASSERT(exists(dco));

    // check if the object is being tracked
    if (dco->flags.beingTracked) {
        return; // nothing to do
    }

    // check if the element already exists in the cell
    core::StackVector<CollCell*, MAX_MATRIX_INDICES> cells;
    mMatrix.getCells(dco->boundingBox(), cells);
    for (CollCell** beg = cells.begin(), **end = cells.end(); beg != end; ++beg) {
        if (!(*beg)->containsDynamic(dco)) {
            (*beg)->addDynamic(dco);
        }
    }

    // the object now is being tracked
    dco->flags.beingTracked = true;
}

////////////////////////////////////////////////////////////////////////////////
void
CollisionHandler::coRemoveDynamic(CollObject* dco)
{
    ASSERT(dco);
    ASSERT(exists(dco));

    // check if the object is being tracked
    if (!(dco->flags.beingTracked)) {
        return; // nothing to do
    }

    // check if the element already exists in the cell
    core::StackVector<CollCell*, MAX_MATRIX_INDICES> cells;
    mMatrix.getCells(dco->boundingBox(), cells);
    for (CollCell** beg = cells.begin(), **end = cells.end(); beg != end; ++beg) {
        (*beg)->removeDynamic(dco);
    }

    // this object now is being tracked
    dco->flags.beingTracked = false;
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
    if (!(co->isCollisionsEnabled()) || !(co->flags.beingTracked)) {
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
    if (args.ptype & PrecisionType::CQ_CollPoints) {
        // yes we should
        ASSERT(false && "We need to implement this!!!!"); // TODO
    }

    return !result.objects.empty();
}

// TODO: add the other querys here like Bounding box query / Line query /
//       position query / Sphere query? ... etc.
//


} /* namespace coll */
