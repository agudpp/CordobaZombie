/*
 * StaticMatrixPartition.h
 *
 *  Created on: Jun 22, 2013
 *      Author: agustin
 */

#ifndef STATICMATRIXPARTITION_H_
#define STATICMATRIXPARTITION_H_

#include <types/basics.h>
#include <types/StackVector.h>
#include <math/AABB.h>
#include <math/Vec2.h>
#include <debug/DebugUtil.h>



namespace core {

template <typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
class StaticMatrixPartition
{
public:
    StaticMatrixPartition() {};
    ~StaticMatrixPartition() {};

    // @brief Set the size of the matrix. If we previously created the matrix
    //        we will remove all the data and initialize the new one
    // @param aabb          The space of the bounding box we want to map
    //
    inline void
    construct(const AABB& aabb);

    // @brief Destroy the matrix contents
    //
    inline void
    destroy(void);

    // @brief Return the number of rows and columns
    //
    inline core::size_t
    numColumns(void) const;
    inline core::size_t
    numRows(void) const;

    // @brief Return the bounding box that this matrix is mapping ("world")
    //
    inline const AABB&
    boundingBox(void) const;

    // @brief Get the index for an specific row and column
    // @param row   The row
    // @param col   The column
    //
    inline core::size_t
    getIndex(core::size_t row, core::size_t col) const;

    // @brief Get the cell of from an specific row and column or from an index
    // @param row   The row
    // @param col   The column
    //
    inline CellType&
    getCell(core::size_t row, core::size_t col);
    inline const CellType&
    getCell(core::size_t row, core::size_t col) const;
    inline CellType&
    getCell(core::size_t index);
    inline const CellType&
    getCell(core::size_t index) const;

    // @brief Check if a position is inside the matrix or not
    // @param position  The position to be checked
    //
    inline bool
    isPositionInside(const Vector2& position) const;

    // @brief Get a cell from a position (the position must be inside the boundingBox
    //        of this matrix). Check that before calling this method. If not
    //        the position will be clamped to the bounding
    // @param position  The position that will be mapped into a cell
    // @return the associated cell
    //
    inline const CellType&
    getCell(const Vector2& position) const;
    inline CellType&
    getCell(const Vector2& position);

    // @brief Get the associated cells pointers that intersects a given AABB
    // @param aabb      The AABB of the query
    // @param result    The list of CellType* that intersect with AABB
    //
    inline void
    getCells(const AABB& aabb, StackVector<CellType *, NUM_COLS*NUM_ROWS>& result);
    inline void
    getCellsIndices(const AABB& aabb, StackVector<core::size_t, NUM_COLS*NUM_ROWS>& result) const;

    // @brief Check if a cell id is valid
    // @param index   The index of the cell to be checked
    //
    inline bool
    isIndexValid(core::size_t index) const;

private:
    // @brief Helper method to get clamped X and Y position from a given x/y value
    //
    inline core::size_t
    getClampedX(float x) const;
    inline core::size_t
    getClampedY(float y) const;

private:
    AABB mBoundingBox;
    float mInvXFactor;
    float mInvYFactor;
    StackVector<CellType, NUM_ROWS*NUM_COLS> mMatrix;
};


////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline core::size_t
StaticMatrixPartition<CellType, NUM_COLS, NUM_ROWS>::getClampedX(float x) const
{
    return (x <= mBoundingBox.tl.x ? 0 :
            x >= mBoundingBox.br.x ? NUM_ROWS - 1 :
            static_cast<core::size_t>((x - mBoundingBox.tl.x) * mInvXFactor));
}
template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline core::size_t
StaticMatrixPartition<CellType,NUM_COLS,NUM_ROWS>::getClampedY(float y) const
{
    return (y >= mBoundingBox.tl.y ? NUM_ROWS -1 :
            y <= mBoundingBox.br.y ? 0 :
            static_cast<core::size_t>((y - mBoundingBox.br.y) * mInvYFactor));
}

////////////////////////////////////////////////////////////////////////////////
template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline void
StaticMatrixPartition<CellType,NUM_COLS,NUM_ROWS>::destroy(void)
{
    mMatrix.clear();
}

template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline void
StaticMatrixPartition<CellType,NUM_COLS,NUM_ROWS>::construct(const AABB& aabb)
{
    mMatrix.clear();
    mMatrix.resize(NUM_COLS * NUM_ROWS);

    mBoundingBox = aabb;

    // calculate inv factors
    const float worldWidth = aabb.getWidth();
    const float worldHeight = aabb.getHeight();
    mInvYFactor = static_cast<float>(NUM_ROWS) / worldHeight; // = 1 / YCellSize
    mInvXFactor = static_cast<float>(NUM_COLS) / worldWidth; // 1 / XCellSize

}

template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline core::size_t
StaticMatrixPartition<CellType,NUM_COLS,NUM_ROWS>::numColumns(void) const
{
    return NUM_COLS;
}
template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline core::size_t
StaticMatrixPartition<CellType,NUM_COLS,NUM_ROWS>::numRows(void) const
{
    return NUM_ROWS;
}

template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline const AABB&
StaticMatrixPartition<CellType,NUM_COLS,NUM_ROWS>::boundingBox(void) const
{
    return mBoundingBox;
}

template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline core::size_t
StaticMatrixPartition<CellType,NUM_COLS,NUM_ROWS>::getIndex(core::size_t row,
                                                            core::size_t col) const
{
    return NUM_COLS * row + col;
}

template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline CellType&
StaticMatrixPartition<CellType,NUM_COLS,NUM_ROWS>::getCell(core::size_t index)
{
    ASSERT(index < mMatrix.size());
    return mMatrix[index];
}
template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline const CellType&
StaticMatrixPartition<CellType,NUM_COLS,NUM_ROWS>::getCell(core::size_t index) const
{
    ASSERT(index < mMatrix.size());
    return mMatrix[index];
}

template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline CellType&
StaticMatrixPartition<CellType,NUM_COLS,NUM_ROWS>::getCell(core::size_t row,
                                                           core::size_t col)
{
    return getCell(getIndex(row, col));
}
template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline const CellType&
StaticMatrixPartition<CellType,NUM_COLS,NUM_ROWS>::getCell(core::size_t row,
                                                           core::size_t col) const
{
    return getCell(getIndex(row, col));
}

template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline bool
StaticMatrixPartition<CellType,NUM_COLS,NUM_ROWS>::isPositionInside(const Vector2& position) const
{
    return mBoundingBox.checkPointInside(position);
}

template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline const CellType&
StaticMatrixPartition<CellType,NUM_COLS,NUM_ROWS>::getCell(const Vector2& position) const
{
    // translate positions inside of our coordinate system and multiply by the
    // factor to get the index directly
    const core::size_t row = getClampedX(position.x);
    const core::size_t col = getClampedY(position.y);

    ASSERT(row < NUM_ROWS);
    ASSERT(col < NUM_COLS);

    return getCell(row, col);
}

template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline CellType&
StaticMatrixPartition<CellType,NUM_COLS,NUM_ROWS>::getCell(const Vector2& position)
{
    // translate positions inside of our coordinate system and multiply by the
    // factor to get the index directly
    const core::size_t row = getClampedX(position.x);
    const core::size_t col = getClampedY(position.y);

    ASSERT(row < NUM_ROWS);
    ASSERT(col < NUM_COLS);

    return getCell(row, col);
}

template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline void
StaticMatrixPartition<CellType,NUM_COLS, NUM_ROWS>::getCells(const AABB& aabb,
                                                            StackVector<CellType *, (NUM_COLS*NUM_ROWS)>& result)
{
    result.clear();
    // do fast check first
    if (!aabb.collide(mBoundingBox)) {
        return;
    }

    // we can ensure that we have a intersection, get the x ranges and y ranges
    core::size_t rowBegin = getClampedX(aabb.tl.x),
                 rowEnd = getClampedX(aabb.br.x),
                 colBegin = getClampedY(aabb.br.y),
                 colEnd = getClampedY(aabb.tl.y);

    // we will not reserve space for the result since we assume that the vector
    // was used before and already contains sufficient space to allocate this

    // TODO: optimize this
    for (; rowBegin <= rowEnd; ++rowBegin) {
        for (core::size_t col = colBegin; col <= colEnd; ++col) {
            result.push_back(&getCell(rowBegin, col));
        }
    }
}

template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline void
StaticMatrixPartition<CellType,NUM_COLS, NUM_ROWS>::getCellsIndices(const AABB& aabb,
                                                                    StackVector<core::size_t, NUM_COLS*NUM_ROWS>& result) const
{
    result.clear();
    // do fast check first
    if (!aabb.collide(mBoundingBox)) {
        return;
    }

    // we can ensure that we have a intersection, get the x ranges and y ranges
    core::size_t rowBegin = getClampedX(aabb.tl.x),
                 rowEnd = getClampedX(aabb.br.x),
                 colBegin = getClampedY(aabb.br.y),
                 colEnd = getClampedY(aabb.tl.y);

    // we will not reserve space for the result since we assume that the vector
    // was used before and already contains sufficient space to allocate this

    // TODO: optimize this
    for (; rowBegin <= rowEnd; ++rowBegin) {
        for (core::size_t col = colBegin; col <= colEnd; ++col) {
            result.push_back(getIndex(rowBegin, col));
        }
    }
}

template<typename CellType, unsigned int NUM_COLS, unsigned int NUM_ROWS>
inline bool
StaticMatrixPartition<CellType,NUM_COLS,NUM_ROWS>::isIndexValid(core::size_t index) const
{
    return index < mMatrix.size();
}

} /* namespace core */
#endif /* MATRIXPARTITION_H_ */