/*
 * UnitPath.cpp
 *
 *  Created on: 29/02/2012
 *      Author: agustin
 *
 */

#include "UnitPath.h"
#ifdef DEBUG
#include "PrimitiveDrawer.h"
#endif


const float  UnitPath::T_FACTOR = (1.0f/MAX_INTERMEDIATE_POINTS);

/**
 * Perform all the operations used to calculate the spline of the actual
 * "pathPoint".
 */
void UnitPath::calculateActualPathPoint(void)
{
	// check that we can actually can calculate the actual point
	// or if is the last one... if it is, we have not to calculate the splie
	if(mPoints.empty() || mIndex >= (mPoints.size()-1)){
		mUseSpline = false;
		return;
	}

	// calculate the vectors used to calculate the bezier curves
	ASSERT((mIndex > 0) && (mIndex < mPoints.size()-1));
	sm::Vector2 v1,v2;
	v1 = mPoints[mIndex];
	v1 -= mPoints[mIndex-1];
	v2 = mPoints[mIndex];
	v2 -= mPoints[mIndex+1];
	v1.normalize();
	v2.normalize();

	// we will try to use other spline... we will calculate the P1 in base
	// of the sum of the 2 vectors
	v1 *= mRadius;
	v2 *= mRadius;
	mP1 = v1 + v2;
	mP1 += mPoints[mIndex];
	// Calculate now the 2 points P0 and P2
	v1.rotate180();
	v1 *= 2.0f;
	mP0 = mPoints[mIndex] + v1;
	v2.rotate180();
	v2 *= 2.0f;
	mP2 = mPoints[mIndex] + v2;

//	// This is the older way... ------------------------------------------------
	// normal to the point is saved in P1
//	mP1 = v1 + v2;
//	mP1.normalize();
//	mP1 *= mRadius;
//	mP1 += mPoints[mIndex];

	// Calculate now the 2 points P0 and P2
//	v1.rotate180();
//	mP0 = mPoints[mIndex] + v1 * mRadius;
//	v2.rotate180();
//	mP2 = mPoints[mIndex] + v2 * mRadius;
// -----------------------------------------------------------------------------

	mDir = mP2 - mP0;
	mSqrLenght = mDir.squaredLength();
	mDir.normalize();
//	DRAWER.createNewV
	// now we have the 3 points used to calculate the spline
	mUseSpline = true;
	mActualIntermediate = 0;
	for(int i = 0; i < MAX_INTERMEDIATE_POINTS; ++i){
		getSplinePos(mSplined[i], T_FACTOR*i);
//#ifdef DEBUG
//		DRAWER.createNewVPoint(Ogre::Vector3(mSplined[i].x, 0,mSplined[i].y));
//#endif
	}

}





UnitPath::UnitPath() :
		mIndex(0),
		mUseSpline(false)
{
	restartPathIndex();
}

UnitPath::~UnitPath()
{

}


/**
 * This function returns the next position that the unit will have to be
 * on the path. To do this we need the "possible" next position of the unit
 * @param	np		The possible next position of the unit (the approximation
 * 					position of the unit in the next frame)
 * @param	result	The position that have to be the unit in the path
 */
bool UnitPath::getNextPosition(const sm::Vector2 &ap, const sm::Vector2 &np, sm::Vector2 &result)
{

	if(!mUseSpline){
		// go directly to the target
		if(getSqrDistanceToActualPoint(ap) < mSqrRadius){
			if(!nextPathIndex()){
				return false;
			} else {
				if(!mUseSpline){
					// returns the actual point
					result = getActualPoint();
				} // else... analyze above
			}
		} else {
			result = getActualPoint();
			return true;
		}
	}

	ASSERT(mActualIntermediate < MAX_INTERMEDIATE_POINTS);

	// now check the which is the next spline to move
	const sm::Vector2 *spline = &(mSplined[mActualIntermediate]);
	float sqrActDist = spline->squaredDistance(ap);
	const float nxtsqrDist = (mActualIntermediate == MAX_INTERMEDIATE_POINTS) ?
			-1.0f : mSplined[mActualIntermediate+1].squaredDistance(ap);

	// check which is the closest point
	if(nxtsqrDist < sqrActDist) {
		++mActualIntermediate;
		spline = &(mSplined[mActualIntermediate]);
		sqrActDist = nxtsqrDist;
	}

	// work with the closest point
	if(sqrActDist < mSqrRadius){
		// update the next intermediate
		++mActualIntermediate;

		// have more intermediates?
		if(mActualIntermediate >= MAX_INTERMEDIATE_POINTS){
			// we have no more intermediates... try to get a new one
			if(!nextPathIndex()){
				// no more intermediates
				return false;
			} else {
				// else we have a new intermediate of the next waypoint
				// return this one
//				result = mSplined[mActualIntermediate];
//				return true;
			}
		}

		if(!mUseSpline){
			result = getActualPoint();
			return true;
		}
		// else.. we can safety return the new intermediate
		result = mSplined[mActualIntermediate];
		return true;
	} else {
		// we just have to continue moving to the actual intermediate
		result = *spline;
		return true;
	}


	ASSERT(false);


	return false;
}

