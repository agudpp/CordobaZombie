/*	We assume that this class will use a vector of points that represent the path
 *
 * UnitPath.h
 *
 *  Created on: 29/02/2012
 *      Author: agustin
 *
 */

#ifndef UNITPATH_H_
#define UNITPATH_H_

#include <vector>

#include "CommonMath.h"
#include "DebugUtil.h"

// we will define the path again here (this is the "PathfinderManager::Path"
// but redefined again to avoid class coupling
typedef std::vector<core::Vector2>	PointPath;

class UnitPath {
	static const int	MAX_INTERMEDIATE_POINTS		=	7;
	static const int	MAX_INTERMEDIATE_POINTS_M1	=	MAX_INTERMEDIATE_POINTS-1;
	static const float	T_FACTOR;
public:
	UnitPath();
	~UnitPath();

	/**
	 * Set the radius of the unit that use this path
	 */
	inline void setRadius(float r);
	inline float getRadius(void) const;

	/**
	 * Returns the path directly to be modified outside of this class.
	 * If this happen then you have to call the "update()" method
	 */
	inline PointPath &getPointPath(void);
	inline const PointPath &getPointPath(void) const;

	/**
	 * Set an external path
	 */
	inline void setPath(const PointPath &other);

	/**
	 * Function called when you modified the path from outside
	 */
	inline void update(void);

	/**
	 * Get the path size
	 */
	inline int getPathSize(void) const;

	/**
	 * Get the actual point/index
	 */
	inline int getActualIndex(void) const;
	inline const core::Vector2 &getActualPoint(void) const;

	/**
	 * Returns the length from a point to the actual point in the path
	 */
	inline float getSqrDistanceToActualPoint(const core::Vector2 &p) const;

	/**
	 * Update the path index to the next point.
	 * Returns true if we have more points, false if this was the last one
	 * Will also calculate the spline curve to be used
	 */
	inline bool nextPathIndex(void);

	/**
	 * Check if there are more pathpoints to travel or not
	 */
	inline bool hasMorePathPoints(void) const;

	/**
	 * Clears the path
	 */
	inline void clearPath(void);

	/**
	 * Check if the path is clear
	 */
	inline bool isEmpty(void) const;

	/**
	 * Set the path index
	 */
	inline void setPathIndex(int i);

	/**
	 * Restart the path index
	 */
	inline void restartPathIndex(void);

	/**
	 * This function returns the next position that the unit will have to be
	 * on the path. To do this we need the "possible" next position of the unit
	 * @param	np		The possible next position of the unit (the approximation
	 * 					position of the unit in the next frame)
	 * @param	result	The position that have to be the unit in the path
	 *
	 * This function automatically updates the path
	 */
	bool getNextPosition(const core::Vector2 &ap, const core::Vector2 &np, core::Vector2 &result);

private:
	/**
	 * Perform all the operations used to calculate the spline of the actual
	 * "pathPoint".
	 */
	void calculateActualPathPoint(void);

	/**
	 * Calculates the position int the spline using all the calculated values
	 * and the t parametter
	 */
	inline void getSplinePos(core::Vector2 &pos, const float t);

private:
	PointPath		mPoints;
	int				mIndex;
	float			mRadius;
	float			mSqrRadius;

	// flag used to determine if we have to move directly to the point
	// or using spline
	bool			mUseSpline;

	// We will calculate the spline using Bézier_curve (quadratic)
	core::Vector2		mP0;
	core::Vector2		mP1;
	core::Vector2		mP2;
	core::Vector2		mDir;
	float 			mSqrLenght;
	core::Vector2		mSplined[MAX_INTERMEDIATE_POINTS];
	int				mActualIntermediate;
};





////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
inline void UnitPath::setRadius(float r)
{
	mRadius = r;
	mSqrRadius = r*r;
}

inline float UnitPath::getRadius(void) const
{
	return mRadius;
}

////////////////////////////////////////////////////////////////////////////////
inline PointPath &UnitPath::getPointPath(void)
{
	return mPoints;
}
inline const PointPath &UnitPath::getPointPath(void) const
{
	return mPoints;
}

////////////////////////////////////////////////////////////////////////////////
inline void UnitPath::setPath(const PointPath &other)
{
	mPoints = other;
	update();
}

////////////////////////////////////////////////////////////////////////////////
inline void UnitPath::update(void)
{
	restartPathIndex();
}

////////////////////////////////////////////////////////////////////////////////
inline int UnitPath::getPathSize(void) const
{
	return mPoints.size();
}

////////////////////////////////////////////////////////////////////////////////
inline int UnitPath::getActualIndex(void) const
{
	return mIndex;
}
inline const core::Vector2 &UnitPath::getActualPoint(void) const
{
	ASSERT(mIndex < mPoints.size());
	return mPoints[mIndex];
}

////////////////////////////////////////////////////////////////////////////////
inline float UnitPath::getSqrDistanceToActualPoint(const core::Vector2 &p) const
{
	return p.squaredDistance(getActualPoint());
}

////////////////////////////////////////////////////////////////////////////////
inline bool UnitPath::nextPathIndex(void)
{
	++mIndex;
	if(mIndex >= mPoints.size()){
		return false;
	}
	// else we have to update the spline
	calculateActualPathPoint();
	return true;
}

////////////////////////////////////////////////////////////////////////////////
inline bool UnitPath::hasMorePathPoints(void) const
{
	return mIndex < mPoints.size();
}

////////////////////////////////////////////////////////////////////////////////
inline void UnitPath::clearPath(void)
{
	mIndex = 0;
	mPoints.clear();
}

////////////////////////////////////////////////////////////////////////////////
inline bool UnitPath::isEmpty(void) const
{
	return mPoints.empty() || (!hasMorePathPoints());
}

////////////////////////////////////////////////////////////////////////////////
inline void UnitPath::setPathIndex(int i)
{
	ASSERT(i < mPoints.size());
	if(mIndex != i){
		mIndex = i;
		calculateActualPathPoint();
	}
}

////////////////////////////////////////////////////////////////////////////////
inline void UnitPath::restartPathIndex(void)
{
	mIndex = 1;
	calculateActualPathPoint();
}

////////////////////////////////////////////////////////////////////////////////
inline void UnitPath::getSplinePos(core::Vector2 &pos, const float t)
{
	const float a1 = (1.0f-t);
	const float a2 = a1*a1;

	// use approximation?
	pos = mP0*a2 + mP1*(2.0f*a1*t)+mP2*(t*t);
}

#endif /* UNITPATH_H_ */
