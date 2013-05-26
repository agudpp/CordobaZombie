/*
 * LineOfSightManager.h
 *
 *  Created on: 05/03/2012
 *      Author: agustin
 */

#ifndef LINEOFSIGHTMANAGER_H_
#define LINEOFSIGHTMANAGER_H_

#include <bitset>
#include <string>
#include "DebugUtil.h"
#include "CommonMath.h"


// We have to use a template because the std::bitset use a fucking template
// to be constructed.
// The numRows and numColumns is the number of rows that we will divide the
// level and using the aabb in buildMatrix().
template<int numRows, int numColums>
class LineOfSightManager
{
	enum {
		REAL_MATRIX_DIMENSION = numRows * numColums,
	};
public:
	LineOfSightManager(){};
	~LineOfSightManager(){};

	/**
	 * Build the matrix associated to the level
	 * @param aabb		The level AABB
	 */
	void buildMatrix(const math::AABB &aabb);

	/**
	 * Function that set into the matrix that two position have LineOfSigh(LOS)
	 * @param	from	Point1
	 * @param	to		Point2
	 * @param	los		Have LOS (true) or not(false)
	 */
	void setLOS(const math::Vector2 &from, const math::Vector2 &to, bool los);

	/**
	 * Clear all the matrix
	 */
	void clear(void);

	/**
	 * Load from file
	 */
	bool loadFromFile(const std::string &fname);

	/**
	 * Save to file
	 */
	bool saveToFile(const std::string &fname);


	//-------------------------------------------------------------------------

	/**
	 * Returns the matrix sizes
	 */
	inline int getNumColumns(void) const;
	inline int getNumRows(void) const;
	inline const math::AABB &getAABB(void) const;

	/**
	 * Returns the point associated to a row and a column, this point is in
	 * the middle of the cell of the matrix
	 * @param 	r 		Row
	 * @param	c		Column
	 * @param	result	The result position
	 *
	 * @note	This function is slow, do not call in the game loop
	 */
	inline void getPositionFromCell(int r, int c, math::Vector2 &result) const;

	/**
	 * Check if we have line of sight between two points
	 * @param	from	The first position
	 * @param	to		The second position
	 * @returns	True if we have line of sight or false otherwise
	 */
	bool checkLOS(const math::Vector2 &from, const math::Vector2 &to) const;


private:
	/**
	 * Returns the corresponding values from a vector to the matrix
	 */
	inline int getXPosition(const float &x) const;
	inline int getYPosition(const float &y) const;

private:
	math::AABB 			mAABB;
	int					mRows;
	int					mColumns;
	float				mFactorX;
	float				mFactorY;
	// Note that we will create a matrix of size
	// (numRows*numColums) * (numRows*numColums)
	// TODO: podemos usar la mitad del espacio ya que es espejada la matriz
	// asique mejorar esto para ahorrar la mitad del espacio
	// because we have to map every cell to every other cell and [i,j] = [j,i]
	std::bitset<(numRows*numColums) * (numRows*numColums)>	mMatrix;
};


////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
template<int numRows, int numColums>
inline int
LineOfSightManager<numRows, numColums>::getNumColumns(void) const
{
	return numColums;
}
////////////////////////////////////////////////////////////////////////////////
template<int numRows, int numColums>
inline int
LineOfSightManager<numRows, numColums>::getNumRows(void) const
{
	return numRows;
}
template<int numRows, int numColums>
inline const
math::AABB &LineOfSightManager<numRows, numColums>::getAABB(void) const
{
	return mAABB;
}

////////////////////////////////////////////////////////////////////////////////
template<int numRows, int numColums>
inline void
LineOfSightManager<numRows, numColums>::getPositionFromCell(int r,
		                                                    int c,
		                                                    math::Vector2 &result) const
{
	ASSERT(r < numRows);
	ASSERT(c < numColums);

	const float fx = (1.0f/mFactorX) * static_cast<float>(c);
	const float sx = (1.0f/mFactorX) * static_cast<float>(c+1);
	const float fy = (1.0f/mFactorY) * static_cast<float>(r);
	const float sy = (1.0f/mFactorY) * static_cast<float>(r+1);

	result.x = (fx+sx) * 0.5f;
	result.y = (fy+sy) * 0.5f;
}

////////////////////////////////////////////////////////////////////////////////
template<int numRows, int numColums>
inline int
LineOfSightManager<numRows, numColums>::getXPosition(const float &x) const
{
	ASSERT(x >= 0);
	int r = int(x*mFactorX);
	// TODO: evitar esto usando assert? nos ahorramos un paso
	ASSERT(r < numColums);
	return (r >= numColums) ? numColums-1 : r;
}
////////////////////////////////////////////////////////////////////////////////
template<int numRows, int numColums>
inline int
LineOfSightManager<numRows, numColums>::getYPosition(const float &y) const
{
	ASSERT(y >= 0);
	int r = int(y*mFactorY);
	// TODO: evitar esto usando assert? nos ahorramos un paso
	ASSERT(r < numRows);
	return (r >= numRows) ? numRows-1 : r;
}


#endif /* LINEOFSIGHTMANAGER_H_ */
