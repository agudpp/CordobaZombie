/*
 * LineOfSightManager.cpp
 *
 *  Created on: 05/03/2012
 *      Author: agustin
 */

#include "LineOfSightManager.h"




////////////////////////////////////////////////////////////////////////////////
template<int numRows, int numColums>
void LineOfSightManager<numRows, numColums>::buildMatrix(const sm::AABB &aabb)
{
	const float xCellSize = aabb.getWidth() / numColums;
	const float yCellSize = aabb.getHeight() / numRows;
	mFactorX = 1.0f/xCellSize;
	mFactorY = 1.0f/yCellSize;

}

////////////////////////////////////////////////////////////////////////////////
template<int numRows, int numColums>
void LineOfSightManager<numRows, numColums>::setLOS(const sm::Vector2 &from, const sm::Vector2 &to,
		bool los)
{
	int xf = getXPosition(from), yf = getYPosition(from),
			xt = getXPosition(to), yt = getXPosition(to);

	// we get the id of the cell associated to "from" and the cell associated
	// to "to"
	const int fid = numColums * yf + xf;
	const int tid = numColums * yt + xt;

	// now check the correct place that is [fid, tid] and [tid, fid]
	mMatrix.set(REAL_MATRIX_DIMENSION * fid + tid, los);
	mMatrix.set(REAL_MATRIX_DIMENSION * tid + fid, los);
}

////////////////////////////////////////////////////////////////////////////////
template<int numRows, int numColums>
void LineOfSightManager<numRows, numColums>::clear(void)
{
	mMatrix.reset();
}

////////////////////////////////////////////////////////////////////////////////
template<int numRows, int numColums>
bool LineOfSightManager<numRows, numColums>::loadFromFile(const std::string &fname)
{
	// TODO:
	ASSERT(false);
}

////////////////////////////////////////////////////////////////////////////////
template<int numRows, int numColums>
bool LineOfSightManager<numRows, numColums>::saveToFile(const std::string &fname)
{
	// tODO:
	ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
template<int numRows, int numColums>
bool LineOfSightManager<numRows, numColums>::checkLOS(const sm::Vector2 &from, const sm::Vector2 &to) const
{
	int xf = getXPosition(from), yf = getYPosition(from),
			xt = getXPosition(to), yt = getXPosition(to);

	// we get the id of the cell associated to "from" and the cell associated
	// to "to"
	const int fid = numColums * yf + xf;
	const int tid = numColums * yt + xt;

	// now check the correct place that is [fid, tid] and [tid, fid]
	ASSERT(mMatrix.test(REAL_MATRIX_DIMENSION * fid + tid) ==
			mMatrix.test(REAL_MATRIX_DIMENSION * tid + fid));

	return mMatrix.test(REAL_MATRIX_DIMENSION * fid + tid);
}
