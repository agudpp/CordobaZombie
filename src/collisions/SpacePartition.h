/*
 * SpacePartition.h
 *
 *  Created on: Feb 20, 2013
 *      Author: agustin
 */

#ifndef SPACEPARTITION_H_
#define SPACEPARTITION_H_

#include <vector>
#include <limits>
#include <cinttypes>

#include <math/AABB.h>
#include <math/Vector2.h>
#include <debug/DebugUtil.h>

#include "Defines.h"
#include "TwoDimCell.h"
#include "Object.h"

namespace coll {


class SpacePartition {

    typedef std::vector<TwoDimCell> CellsVec;

    // Matrix definition
    class Matrix {
    public:
        Matrix();
        ~Matrix();

        // Create a matrix, if the matrix is already created it will be destroyed
        //
        void create(const size_t numRows, const size_t numCols);

        // destroy the actual matrix
        //
        void destroy(void);

        // Remove all the objects from the cells
        //
        void removeAllObjects(void);

        // Get the TwoDimCell associated to a row and column.
        //
        inline TwoDimCell &getCell(const size_t col, const size_t row);
        inline TwoDimCell &getCell(const size_t index);
        inline void getCell(const size_t bcol, const size_t brow,
                            const size_t ecol, const size_t erow,
                            std::vector<TwoDimCell*> &result);

        // helper function to get the index from a column and row number
        //
        inline size_t getIndex(const size_t col, const size_t row) const;

    private:
        Matrix(const Matrix&);
        Matrix &operator=(const Matrix&);

    private:
        CellsVec mCells;
        size_t mRows;
        size_t mCols;

    };

public:
    SpacePartition();
    ~SpacePartition();

    /**
     * @brief Builds the CollisionSpace
     *        We will use the normal opengl coordinates system, this means that
     *        the (0,0) is on the (top,left) and the (M,N) is on the
     *        (bottom,right).
     *
     * @param world The world rectangle to detect collisions (screen size for example)
     * @param cnX   The number of cells used in X
     * @param cnY   The number of cells used in Y
     */
    bool
    build(const math::AABB<UnitType> &world, size_t cnX, size_t cnY);

    /**
     * @brief Returns the level size and cells info
     */
    inline
    UnitType worldXSize(void) const;
    inline
    UnitType worldYSize(void) const;
    inline
    size_t numCellsX(void) const;
    inline
    size_t numCellsY(void) const;


    // Space Partition object handling functions
    //

    /**
     * @brief Add a object (the object wasn't added before)
     * @param obj   The object to be added
     */
    void
    addObject(Object *obj);

    /**
     * @brief Removes an object from the SpacePartition. This will stop the
     *        collision detection of the object.
     *        Requires the object be in the SpacePartition
     * @param obj   The object to be removed
     */
    void
    removeObject(const Object *obj);

    /**
     * @brief Remove all the objects
     */
    void
    removeAllObjects(void);

    /**
     * @brief Check if an object exists or not in the SpacePartition
     * @param obj   The object we want to check if exists or not
     */
    inline bool
    exists(const Object *obj) const;


    // Specific object handling functions
    //

    /**
     * @brief Translate a object from the actual position using the vector t
     * @param obj   The object to be translated (it should be in the Space
     *              Partition)
     * @param t     The translation vector
     */
    inline void
    translateObject(Object *obj, const math::Vector2<UnitType> &t);

    /**
     * @brief Set the position of an specific object using a math::vector2
     * @param obj   The object to be re-positionated
     * @param pos   The position vector
     */
    inline void
    setObjectPosition(Object *obj, const math::Vector2<UnitType> &pos);

    /**
     * @brief Update the AABB of an object that was added before to this manager
     * @param obj   The object that we want to update the AABB
     * @param aabb  The new AABB we want to set to the object
     */
    void
    updateObject(Object *obj, const math::AABB<UnitType> &aabb);


    // Space Partition querys
    //

    /**
     * @brief Get all the intersection for a certain object
     * @param obj       The object to be cheked against
     * @param result    The resulting vector to put the objects
     */
    void
    getIntersections(const Object *obj, ConstObjectVec &objs);

    /**
     * @brief Get all the objects that are in a AABB region (AABB query).
     * @param   aabb    The aabb region to be used
     * @param   mask    MaskGroup used to filter all the objects
     * @param   result  The Objects that are in the AABB
     */
    void
    getObjectsQuery(const math::AABB<UnitType> &aabb,
                    std::uint32_t mask,
                    ConstObjectVec &result);

    /**
     * @brief Get all the objects that intersect a certain point with a certain
     *        mask (Point query)
     * @param point     The point to check
     * @param result    The result of the check // we need to create 2 more rows and 2 more columns to get the objects that
    // will be outside of the collision world
     * @param mask      The mask to be used
     */
    void
    getObjectsQuery(const math::Vector2<UnitType> &point,
                    ConstObjectVec &result,
                    std::uint32_t mask = ~0u);

    /**
     * @brief Get all the objects that intersect a certain line segment with a
     *        certain mask (Line query)
     * @param p1        The first point of the line segment
     * @param p2        The second point of the line segment
     * @param result    The result of the check
     * @param mask      The mask to be used
     */
    void
    getObjectsQuery(const math::Vector2<UnitType> &p1,
                    const math::Vector2<UnitType> &p2,
                    ConstObjectVec &result,
                    std::uint32_t mask = ~0u);

    /**
     * @brief Function used to check if a point is inside the Space Partition
     * @param   p       The point to be checked
     * @return  True    if p is inside the Space Partition
     *          False   otherwise
     */
    inline bool
    isPointInside(const math::Vector2<UnitType> &p);


private:
    // Auxiliary functions
    //

    /**
     * Returns the corresponding values from a vector to the matrix
     */
    inline size_t getXPosition(UnitType x, bool& inside) const;
    inline size_t getYPosition(UnitType y, bool& inside) const;

    /**
     * Returns the begin index and end index from an AABB. If the AABB
     * is completely outside of the CollisionWorld, then false is returned
     * and the indices are incorrectly, if the AABB intersects with the
     * CollisionWorld then true is returned and the indices are valid
     */
    inline bool getIndices(const math::AABB<UnitType> &aabb,
                           size_t &beginIndex,
                           size_t &endIndex);

    /**
     * Get the cells associated to a Object and put the result in buff
     */
    inline void getCellsFromObject(const Object *obj,
                                   std::vector<TwoDimCell*> &buff);
    inline void getCellsFromAABB(const math::AABB<UnitType> &aabb,
                                 std::vector<TwoDimCell*> &buff);

    /**
     * Simple segment intersection test
     */
    inline bool segmentIntersection(const size_t a, const size_t b,
                                    const size_t c, const size_t d);


    /**
     * Remove an object from the actual position and add it to the new one
     * trying to be efficently
     */
    inline void updateObject(size_t beforeBIndex, const size_t beforeEIndex,
                             size_t afterBIndex, const size_t afterEBIndex,
                             Object *obj);


private:
    float mFactorX;
    float mFactorY;
    math::AABB<UnitType> mWorld;
    size_t mNumCellX;
    size_t mNumCellY;
    Matrix mMatrix;
    std::vector<TwoDimCell*> mCellAuxBuffer;
    std::vector<Object *> mObjects;
};



// Inline Implementations
//

/**
 * Returns the corresponding values from a vector to the matrix
 */
inline size_t
SpacePartition::getXPosition(UnitType x, bool& inside) const
{
    ASSERT(x >= UnitType(0));

    if (x < mWorld.tl.x) {
        x = mWorld.tl.x;
        inside = false;
    } else if (x > mWorld.br.x) {
        x = mWorld.br.x - 1;
        inside = false;
    } else {
        inside = true;
    }
    const size_t r = static_cast<size_t>(static_cast<float>(x) * mFactorX);
    ASSERT(r < mNumCellX);
    return (r >= mNumCellX) ? mNumCellX - 1 : r;
}
inline size_t
SpacePartition::getYPosition(UnitType y, bool& inside) const
{
    ASSERT(y >= UnitType(0));

    if (y < mWorld.tl.y) {
        y = mWorld.tl.y;
        inside = false;
    } else if (y > mWorld.br.y) {
        y = mWorld.br.y - 1;
        inside = false;
    } else {
        inside = true;
    }
    const size_t r = static_cast<size_t>(static_cast<float>(y) * mFactorY);
    ASSERT(r < mNumCellY);
    return (r >= mNumCellY) ? mNumCellY - 1 : r;
}

inline bool
SpacePartition::getIndices(const math::AABB<UnitType> &aabb,
                           size_t &beginIndex,
                           size_t &endIndex)
{
    bool inside1, aux, inside2;
    const size_t x1 = getXPosition(aabb.tl.x, inside1);
    const size_t y1 = getYPosition(aabb.tl.y, aux);
    inside1 = inside1 && aux;

    const size_t x2 = getXPosition(aabb.br.x, inside2);
    const size_t y2 = getYPosition(aabb.br.y, aux);
    inside2 = inside2 && aux;
    if (!inside1 && !inside2) {
        // we are completely outside
        return false;
    }
    beginIndex = mMatrix.getIndex(x1, y1);
    endIndex = mMatrix.getIndex(x2, y2);
    return true;
}

inline void
SpacePartition::getCellsFromObject(const Object *obj,
                                   std::vector<TwoDimCell*> &buff)
{
    ASSERT(obj);
    getCellsFromAABB(obj->aabb(), buff);
}
inline void
SpacePartition::getCellsFromAABB(const math::AABB<UnitType> &aabb,
                                 std::vector<TwoDimCell*> &buff)
{
    buff.clear();

    // check if the aabb is completely outside or one
    bool p1Inside, aux, p2Inside;
    const size_t posX1 = getXPosition(aabb.tl.x, p1Inside);
    const size_t posY1 = getYPosition(aabb.tl.y, aux);
    p1Inside = p1Inside && aux;

    const size_t posX2 = getXPosition(aabb.br.x, p2Inside);
    const size_t posY2 = getYPosition(aabb.br.y, aux);
    p2Inside = p2Inside && aux;

    if (!p1Inside && !p2Inside) {
        // nothing to do! the aabb is completely outside
        ASSERT(mWorld.collide(aabb) == false); // ensure there is not one inside other
        return;
    }

    // if we are here, this means we can get the cells using the better
    // approximation of the positions (that are inside the cells) safely
    mMatrix.getCell(posX1, posY1, posX2, posY2, buff);
}

inline bool
SpacePartition::segmentIntersection(const size_t a, const size_t b,
                                    const size_t c, const size_t d)
{
    return (b <= d && b >= c) || (a >= c && a <= d) || (a <= c && b >= d);
}

inline void
SpacePartition::updateObject(size_t beforeBIndex, const size_t beforeEIndex,
                             size_t afterBIndex, const size_t afterEIndex,
                             Object *obj)
{
    ASSERT(exists(obj));

    // calculate the intersection
    if (!segmentIntersection(beforeBIndex, beforeEIndex,
            afterBIndex, afterEIndex)){
        // they not intersect, remove the object in the old cells and then
        // add it to the new one
        for(; beforeBIndex < beforeEIndex; ++beforeBIndex){
            mMatrix.getCell(beforeBIndex).removeObject(obj);
        }
        for(; afterBIndex < afterEIndex; ++afterBIndex){
            mMatrix.getCell(afterBIndex).addObject(obj);
        }
        return;
    }

    // they intersect, we need to get the indices for the new cells only
    // and remove the old ones
    for(size_t i = beforeBIndex; i < afterBIndex; ++i){
        mMatrix.getCell(i).removeObject(obj);
    }
    for(size_t i = beforeEIndex; i < afterEIndex; ++i){
        mMatrix.getCell(i).removeObject(obj);
    }

    // add
    for(size_t i = afterBIndex; i < beforeBIndex; ++i){
        mMatrix.getCell(i).addObject(obj);
    }
    for(size_t i = afterEIndex; i < beforeEIndex; ++i){
        mMatrix.getCell(i).addObject(obj);
    }
}

inline bool
SpacePartition::exists(const Object *obj) const
{
    ASSERT(obj);
    return obj->mID < mObjects.size() && mObjects[obj->mID] == obj;
}


inline UnitType
SpacePartition::worldXSize(void) const
{
    return mWorld.getWidth();
}
inline UnitType
SpacePartition::worldYSize(void) const
{
    return mWorld.getHeight();
}
inline size_t
SpacePartition::numCellsX(void) const
{
    return mNumCellX;
}
inline size_t
SpacePartition::numCellsY(void) const
{
    return mNumCellY;
}
inline void
SpacePartition::translateObject(Object *obj, const math::Vector2<UnitType> &t)
{
    ASSERT(obj);
    ASSERT(exists(obj));
    math::AABB<UnitType> aabb = obj->aabb();
    aabb.translate(t);
    updateObject(obj, aabb);
}

////////////////////////////////////////////////////////////////////////////////
inline void
SpacePartition::setObjectPosition(Object *obj, const math::Vector2<UnitType> &pos)
{
    ASSERT(obj);
    ASSERT(exists(obj));

    math::AABB<UnitType> aabb = obj->mAABB;
    aabb.setPosition(pos);
    updateObject(obj, aabb);
}

////////////////////////////////////////////////////////////////////////////////
inline bool
SpacePartition::isPointInside(const math::Vector2<UnitType> &p)
{
    return mWorld.checkPointInside(p);
}

////////////////////////////////////////////////////////////////////////////////
inline TwoDimCell &
SpacePartition::Matrix::getCell(const size_t col, const size_t row)
{
    ASSERT(mRows != std::numeric_limits<size_t>::max());
    ASSERT(mCols != std::numeric_limits<size_t>::max());
    ASSERT(mCols * row + col < mCells.size());
    return mCells[mCols * row + col];
}
inline TwoDimCell &
SpacePartition::Matrix::getCell(const size_t index)
{
    ASSERT(index < mCells.size());
    return mCells[index];
}

inline void
SpacePartition::Matrix::getCell(const size_t bcol, const size_t brow,
                                const size_t ecol, const size_t erow,
                                std::vector<TwoDimCell*> &result)
{
    ASSERT(mRows != std::numeric_limits<size_t>::max());
    ASSERT(mCols != std::numeric_limits<size_t>::max());

    size_t bIndex = mCols * brow + bcol;
    const size_t eIndex = mCols * erow + ecol;

    ASSERT(bIndex <= eIndex);
    ASSERT(bIndex < mCells.size());
    ASSERT(eIndex < mCells.size());

    // we do not reserve the size for the vector becase we assume that we
    // we will use always the same vector
    for (; bIndex <= eIndex; ++bIndex) {
        result.push_back(&mCells[bIndex]);
    }
}

inline size_t
SpacePartition::Matrix::getIndex(const size_t col, const size_t row) const
{
    return mCols * row + col;
}

}

#endif /* SPACEPARTITION_H_ */

