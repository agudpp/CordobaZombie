/*
 * BitMatrix.h
 *
 *  Created on: Aug 20, 2013
 *      Author: agustin
 */

#ifndef BITMATRIX_H_
#define BITMATRIX_H_

#include <bitset>

#include <debug/DebugUtil.h>

namespace core {

// If you will use this class with a size bigger than you need take into account
// that the operations and memory used it will be proportional to the MAX_SIZE
// and not the real size of the matrix.
//

template <unsigned int MAX_SIZE = 4098>
class BitMatrix
{
public:

    inline BitMatrix(unsigned int numCols = 0, unsigned int numRows = 0);
    inline ~BitMatrix(){};

    // @brief Build the matrix into the stack (if this object is built into the
    //        stack of course).
    // @param numCols   The number of columns to be used
    // @param numRows   The number of rows
    // @note numCols * numRows < MAX_SIZE
    //
    inline void
    build(unsigned int numCols, unsigned int numRows);

    // @brief Reset all the values to 0
    //
    inline void
    reset(void);

    // @brief Set the value of an specific bit in the matrix
    // @param col     The column
    // @param row     The row
    // @param val     The value
    //
    inline void
    set(unsigned int col, unsigned int row, bool val);

    // @brief Return the value of an specific value
    // @param col     The column
    // @param row     The row
    // @return the value associated to matrix[col][row]
    //
    inline bool
    get(unsigned int col, unsigned int row) const;

private:
    // @brief transform column and row to index
    //
    inline unsigned int
    toIndex(unsigned int col, unsigned int row) const;

private:
    std::bitset<MAX_SIZE> mMatrix;
    unsigned int mNumCols;
    unsigned int mNumRows;
};




////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

template <unsigned int MAX_SIZE>
inline BitMatrix<MAX_SIZE>::BitMatrix(unsigned int numCols, unsigned int numRows) :
    mNumCols(numCols)
,   mNumRows(numRows)
{
    build(numCols, numRows);
}

template <unsigned int MAX_SIZE>
inline void
BitMatrix<MAX_SIZE>::build(unsigned int numCols, unsigned int numRows)
{
    mNumCols = numCols;
    mNumRows = numRows;
    ASSERT((mNumCols * mNumRows) < MAX_SIZE);
    mMatrix.reset();
}

template <unsigned int MAX_SIZE>
inline void
BitMatrix<MAX_SIZE>::reset(void)
{
    mMatrix.reset();
}

template <unsigned int MAX_SIZE>
inline void
BitMatrix<MAX_SIZE>::set(unsigned int col, unsigned int row, bool val)
{
    unsigned int i = toIndex(col,row);
    ASSERT(i < (mNumCols * mNumRows));
    mMatrix[i] = val;
}

template <unsigned int MAX_SIZE>
inline bool
BitMatrix<MAX_SIZE>::get(unsigned int col, unsigned int row) const
{
    unsigned int i = toIndex(col,row);
    ASSERT(i < (mNumCols * mNumRows));
    return mMatrix[i];
}

template <unsigned int MAX_SIZE>
inline unsigned int
BitMatrix<MAX_SIZE>::toIndex(unsigned int col, unsigned int row) const
{
    return mNumCols*row + col;
}

} /* namespace core */
#endif /* BITMATRIX_H_ */
