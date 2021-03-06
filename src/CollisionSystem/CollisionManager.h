/*
 * CollisionManager.h
 *
 *  Created on: 18/02/2012
 *      Author: agustin
 *
 */

#ifndef COLLISIONMANAGER_H_
#define COLLISIONMANAGER_H_

#include "CollisionCell.h"
#include "CollisionObject.h"
#include "CollisionTypedefs.h"

#include "DebugUtil.h"


// Typedefs
typedef std::vector<const CollisionObject *> CollisionResult;

struct CollRslt{
	const CollisionObject 		*collObj;
	sm::Vector2					point;
//	std::vector<sm::Vector2>	points;

};
typedef std::vector<CollRslt>	CollRsltPoints;

class CollisionManager {

	typedef std::set<CollisionCell *>	CellPtrSet;
	typedef std::vector<CollisionCell>	CellVec;
	typedef std::vector<CellVec>		CellMatrix;
public:
	CollisionManager();
	~CollisionManager();

	/** Builds the CollisionSpace
	 * @note We will use the normal coordinates system, so the x grows
	 * 		from left to right, and the y grows from down to up.
	 * 		We also assume that the level start at the (0,0) position and
	 * 		grows to the positive axis values
	 *
	 * @param ssX	The space size X of the "world"
	 * @param ssY	The space size Y of the "world"
	 * @param cnX	The number of cells used in X
	 * @param cnY	The number of cells used in Y
	 */
	bool build(cScalar ssX, cScalar ssY, int cnX, int cnY);

	/**
	 * Returns the level size and cells info
	 */
	inline cScalar getWorldXSize(void) const;
	inline cScalar getWorldYSize(void) const;
	inline cScalar getCellYSize(void) const;
	inline cScalar getCellXSize(void) const;
	inline int getNumCellsX(void) const;
	inline int getNumCellsy(void) const;

	/**
	 * Adds a new static object.
	 * @param so	StaticObject to be added
	 * @note	By now there are NO difference between MovableObjects and
	 * 			StaticOBjects, probably is slower separate them.
	 */
	void addStaticObject(const CollisionObject *so);

	/**
	 * Remove a static object.
	 * @param	so	Static object to be removed.
	 */
	void removeStaticObject(const CollisionObject *so);

	/**
	 * Add a movable object (the object wasn't added before)
	 */
	void addMovableObject(const CollisionObject *mo);

	/**
	 * Removes a movable object to stop the detection.
	 */
	void removeMovableObject(const CollisionObject *mo);

	/**
	 * Remove all the objects (static and not static)
	 */
	void removeAllObjects(void);

	/**
	 * Translate a object from the actual position using the vector t
	 */
	void translateObject(CollisionObject *mo, const sm::Vector2 &t);

	/**
	 * Get all the collision of an object
	 */
	void getCollision(const CollisionObject *mo, CollisionResult &objs);

	/**
	 * Get all the collision using the masks associated
	 * TODO: implementar esta
	 */
	void getCollisionUsingMask(const CollisionObject *mo, CollisionResult &objs)
	{ /* TODO: implementar*/}

	/**
	 * Get all the objects that are AABB region.
	 * @param	aabb	The aabb region to be used
	 * @param	mask	MaskGroup used to filter all the objects
	 * @param	result	The Objects that are in the AABB
	 */
	void getCollisionObjects(const sm::AABB &aabb, mask_t mask,
			CollisionResult &result);

	/**
	 * Get all the objects that intersect a certain point with a certain mask
	 * @param point		The point to check
	 * @param mask		The mask to be used
	 * @param result	The result of the check
	 */
	void getCollisionObjects(const sm::Vector2 &point, mask_t mask,
			CollisionResult &result);

	/**
	 * Get all the objects that intersect a certain line segment with a certain
	 * mask
	 * @param p1		The first point of the line segment
	 * @param p2		The second point of the line segment
	 * @param mask		The mask to be used
	 * @param result	The result of the check
	 */
	void getCollisionObjects(const sm::Vector2 &p1, const sm::Vector2 &p2,
			mask_t mask, CollisionResult &result);

	/**
	 * Function used to check if a point is inside the level
	 * @param	p		The point to be checked
	 * @return	True	if p is inside the level
	 * 			False	otherwise
	 */
	inline bool isPointInside(const sm::Vector2 &p);


private:

	/**
	 * Returns the corresponding values from a vector to the matrix
	 */
	inline int getXPosition(const cScalar &x) const
	{
		ASSERT(x >= 0);
		int r = int(x*mFactorX);
		// TODO: evitar esto usando assert? nos ahorramos un paso
		ASSERT(r < mNumCellX);
		return (r >= mNumCellX) ? mNumCellX-1 : r;
	}
	inline int getYPosition(const cScalar &y) const
	{
		ASSERT(y >= 0);
		int r = int(y*mFactorY);
		// TODO: evitar esto usando assert? nos ahorramos un paso
		ASSERT(r < mNumCellY);
		return (r >= mNumCellY) ? mNumCellY-1 : r;
	}

	/**
	 * Add a new object into the matrix to all the places that corresponds
	 */
	inline void addObjectToMatrix(const CollisionObject *mo,
			int tlx, int tly, int brx, int bry)
	{
//		debug("tl(%d,%d), br(%d,%d)\n", tlx, tly, brx, bry);
		ASSERT(tlx == brx || tlx+1 == brx);
		ASSERT(tly == bry || tly == bry+1);


		// add to all the cells the object
		(mMatrix[tlx])[tly].addObject(mo, 0);
		if(brx != tlx){
			// add to the tly cell to
			(mMatrix[brx])[tly].addObject(mo,1);
		}
		// check if we have to other rows
		if(bry != tly){
			(mMatrix[tlx])[bry].addObject(mo,2);
			// check again if we have to add to the other column
			if(brx != tlx){
				// add to the tly cell to
				(mMatrix[brx])[bry].addObject(mo,3);
			}
		}
	}

	/**
	 * Remove object from matrix
	 */
	inline void removeObjectFromMatrix(const CollisionObject *mo,
			int tlx, int tly, int brx, int bry)
	{
		ASSERT(tlx == brx || tlx+1 == brx);
		ASSERT(tly == bry || tly == bry+1);

		// add to all the cells the object
		(mMatrix[tlx])[tly].removeObject(mo, 0);
		if(brx != tlx){
			// add to the tly cell to
			(mMatrix[brx])[tly].removeObject(mo,1);
		}
		// check if we have to other rows
		if(bry != tly){
			(mMatrix[tlx])[bry].removeObject(mo,2);
			// check again if we have to add to the other column
			if(brx != tlx){
				// add to the tly cell to
				(mMatrix[brx])[bry].removeObject(mo,3);
			}
		}
	}

	/**
	 * Get the AABB associated to a cell
	 */
	inline void getAABBFromCell(sm::AABB &result, int x, int y)
	{
		result.tl.x = x * mCellSizeX;
		result.tl.y = (y+1) * mCellSizeY;
		result.br.x = (x+1) * mCellSizeX;
		result.br.y = y * mCellSizeY;

	}




private:
	float		mFactorX;
	float		mFactorY;
	float		mCellSizeX;
	float		mCellSizeY;
	float		mXBounds;
	float		mYBounds;
	CellMatrix	mMatrix;
	int			mNumCellX;
	int			mNumCellY;
	std::set<const CollisionObject*>	mAuxCont;
	CellPtrSet	mCells;
};







////////////////////////////////////////////////////////////////////////////////
inline cScalar CollisionManager::getWorldXSize(void) const {return mCellSizeX*mNumCellX;}
inline cScalar CollisionManager::getWorldYSize(void) const {return mCellSizeY*mNumCellY;}
inline cScalar CollisionManager::getCellYSize(void) const {return mCellSizeX;}
inline cScalar CollisionManager::getCellXSize(void) const {return mCellSizeY;}
inline int CollisionManager::getNumCellsX(void) const {return mNumCellX;}
inline int CollisionManager::getNumCellsy(void) const {return mNumCellY;}

////////////////////////////////////////////////////////////////////////////////
inline bool CollisionManager::isPointInside(const sm::Vector2 &p)
{
	if((p.x < 0.0f || p.x > mXBounds) ||
			(p.y < 0.0f || p.y > mYBounds)) return false;

	return true;
}




#endif /* COLLISIONMANAGER_H_ */
