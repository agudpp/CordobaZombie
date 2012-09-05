/*
 * CollisionManager.cpp
 *
 *  Created on: 18/02/2012
 *      Author: agustin
 *
 */

#ifdef DEBUG
#include "PrimitiveDrawer.h"
#endif
#include "IntersectDetect.h"
#include "CollisionManager.h"







CollisionManager::CollisionManager() :
mFactorX(0.0f),
mFactorY(0.0f),
mNumCellX(0),
mNumCellY(0)
{
	// TODO Auto-generated constructor stub

}

CollisionManager::~CollisionManager() {
	// TODO Auto-generated destructor stub
}


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
bool CollisionManager::build(cScalar ssX, cScalar ssY, int cnX, int cnY)
{
	// create the number of rows
	mMatrix.resize(cnX);
	// resize the number of columns
	for(int i = mMatrix.size()-1; i >= 0; --i){
		mMatrix[i].resize(cnY);
	}

	// num of cells
	mNumCellX = cnX;
	mNumCellY = cnY;

	mCellSizeX = ssX / static_cast<float>(cnX);
	mCellSizeY = ssY / static_cast<float>(cnY);

	mXBounds = mNumCellX * mCellSizeX;
	mYBounds = mNumCellY * mCellSizeY;

	// create the factors
	mFactorX = static_cast<cScalar>(cnX) / ssX;
	mFactorY = static_cast<cScalar>(cnY) / ssY;

	debug("CollisionManager Build: cellSizeX: %f\t cellSizeY: %f\n",
			mCellSizeX, mCellSizeY);
}

/**
 * Adds a new static object. By the now we are not implementing this function
 * TODO: notar que estamos agregando como si fuese movableObject
 */
void CollisionManager::addStaticObject(const CollisionObject *mo)
{
	// TODO: no se si vale la pena diferenciar, estamos agregando un vector
	// mas por celda y haciendo 2 ciclos al pedo, perdemos cache... por eso
	// todo es lo mismo (movable y static)
	addMovableObject(mo);
}

/**
 * Remove a static object.
 * @param	so	Static object to be removed.
 */
void CollisionManager::removeStaticObject(const CollisionObject *mo)
{
	// TODO: no se si vale la pena diferenciar, estamos agregando un vector
	// mas por celda y haciendo 2 ciclos al pedo, perdemos cache... por eso
	// todo es lo mismo (movable y static)
	removeMovableObject(mo);
}

/**
 * Add a movable object (the object wasn't added before)
 */
void CollisionManager::addMovableObject(const CollisionObject *mo)
{
	ASSERT(mo);
	ASSERT(mo->bb.getHeight() <= mCellSizeY);
	ASSERT(mo->bb.getWidth() <= mCellSizeX);

	// calculate the cells that this object intersects
	// TODO: tamos asumiendo que no son llamadas a funcion si no tendriamos
	// que hacer una unica funcion que nos devuelva los 4 puntos de una
	int tlx = getXPosition(mo->bb.tl.x), tly = getYPosition(mo->bb.tl.y),
			brx = getXPosition(mo->bb.br.x), bry = getYPosition(mo->bb.br.y);


	addObjectToMatrix(mo, tlx, tly, brx, bry);
}

/**
 * Removes a movable object to stop the detection.
 */
void CollisionManager::removeMovableObject(const CollisionObject *mo)
{
	ASSERT(mo);
	ASSERT(mo->bb.getHeight() <= 2.0f*mCellSizeY);
	ASSERT(mo->bb.getWidth() <= 2.0f*mCellSizeX);

	// calculate the cells that this object intersects
	// TODO: tamos asumiendo que no son llamadas a funcion si no tendriamos
	// que hacer una unica funcion que nos devuelva los 4 puntos de una
	int tlx = getXPosition(mo->bb.tl.x), tly = getYPosition(mo->bb.tl.y),
			brx = getXPosition(mo->bb.br.x), bry = getYPosition(mo->bb.br.y);
	removeObjectFromMatrix(mo, tlx, tly, brx, bry);
}

/**
 * Remove all the objects
 */
void CollisionManager::removeAllObjects(void)
{
	for(int i = mMatrix.size()-1; i >= 0; --i){
		for(int j = mMatrix[i].size()-1; j >= 0; --j){
			(mMatrix[i])[j].removeAllObjects();
		}
	}
}

/**
 * Translate a object from the actual position using the vector t
 */
void CollisionManager::translateObject(CollisionObject *mo, const sm::Vector2 &t)
{
	ASSERT(mo);

	if(t.x == 0.0f && t.y == 0.0f) return;

	// calculate the cells that this object intersects
	// TODO: tamos asumiendo que no son llamadas a funcion si no tendriamos
	// que hacer una unica funcion que nos devuelva los 4 puntos de una
	int tlx = getXPosition(mo->bb.tl.x), tly = getYPosition(mo->bb.tl.y),
			brx = getXPosition(mo->bb.br.x), bry = getYPosition(mo->bb.br.y);

	// translate
	mo->bb.translate(t);

	// get the new position now
	int tlx2 = getXPosition(mo->bb.tl.x), tly2 = getYPosition(mo->bb.tl.y),
			brx2 = getXPosition(mo->bb.br.x), bry2 = getYPosition(mo->bb.br.y);


	removeObjectFromMatrix(mo, tlx,tly,brx,bry);
	addObjectToMatrix(mo, tlx2,tly2,brx2,bry2);

	// TODO: hacer mas rapido esto, no podemos agregar algo antes de haber removido
	// antes todo porque puede ser que estemos intentando agregar algo en una
	// celda que vamos a borrar despues..

}

/**
 * Get all the collision of an object
 */
void CollisionManager::getCollision(const CollisionObject *mo, CollisionResult &objs)
{
	ASSERT(mo);

	objs.clear();
	int tlx = getXPosition(mo->bb.tl.x), tly = getYPosition(mo->bb.tl.y),
			brx = getXPosition(mo->bb.br.x), bry = getYPosition(mo->bb.br.y);
	(mMatrix[tlx])[tly].getAllCollisions(mo, objs);
	if(tlx != brx){
		(mMatrix[brx])[tly].getAllCollisions(mo, objs);
	}
	if(tly != bry){
		(mMatrix[tlx])[bry].getAllCollisions(mo,objs);
		if(tlx != brx) {
			(mMatrix[brx])[bry].getAllCollisions(mo,objs);
		}
	}

}



////////////////////////////////////////////////////////////////////////////////
void CollisionManager::getCollisionObjects(const sm::AABB &aabb, mask_t mask,
		CollisionResult &objs)
{
	if((aabb.tl.x < 0.0f) || (aabb.br.x < 0.0f) ||
			(aabb.tl.y < 0.0f) || (aabb.br.y < 0.0f)){
		return;
	}
	int tlx = getXPosition(aabb.tl.x), tly = getYPosition(aabb.tl.y),
			brx = getXPosition(aabb.br.x), bry = getYPosition(aabb.br.y);

	mAuxCont.clear();
	// we have to perform a lot of querys
	for(int i = tlx; i <= brx; ++i){
		for(int j = bry; j <= tly; ++j){
			(mMatrix[i])[j].getAllCollisionsMask(mAuxCont, mask, aabb);
		}
	}
	objs.clear();
	objs.insert(objs.end(), mAuxCont.begin(), mAuxCont.end());

}

////////////////////////////////////////////////////////////////////////////////
void CollisionManager::getCollisionObjects(const sm::Vector2 &point, mask_t mask,
		CollisionResult &result)
{
	if((point.x < 0.0f) || (point.y < 0.0f)){
		return;
	}
	int x = getXPosition(point.x), y = getYPosition(point.y);

	result.clear();
	mMatrix[x][y].getAllCollisionMask(result, mask, point);
}

////////////////////////////////////////////////////////////////////////////////
void CollisionManager::getCollisionObjects(const sm::Vector2 &p1, const sm::Vector2 &p2,
		mask_t mask, CollisionResult &result)
{
	if((p1.x < 0.0f) || (p2.x < 0.0f) ||
			(p1.y < 0.0f) || (p2.y < 0.0f)){
		return;
	}
	int x1 = getXPosition(p1.x), y1 = getYPosition(p1.y);
	int x2 = getXPosition(p2.x), y2 = getYPosition(p2.y);

	int lx,rx,ty,by;
	if(x1 > x2){
		rx = x1;
		lx = x2;
	} else {
		rx = x2;
		lx = x1;
	}
	if(y1 > y2){
		ty = y1;
		by = y2;
	} else {
		ty = y2;
		by = y1;
	}

//
//	debug("ty:%d, by:%d, lx:%d, rx: %d,x1: %d, x2: %d, y1: %d, y2: %d\n",
//			ty, by, lx, rx, x1,x2,y1,y2);

	mAuxCont.clear();
	result.clear();
	mCells.clear();

	// now check the trivial cases
	if(x1 == x2){
		if(y1 == y2){
			// only one cell
			mCells.insert(&(mMatrix[x1][y1]));
		} else {
			// only columns
			for(int i = by; i <= ty; ++i){mCells.insert(&(mMatrix[x1][i]));}
		}
	} else if(y1 == y2){
		if(x1 == x2){
			// only one cell
			mCells.insert(&(mMatrix[x1][y1]));
		} else {
			// only columns
			for(int i = lx; i <= rx; ++i){mCells.insert(&(mMatrix[i][y1]));}
		}
	} else {
		// we have to select the base and how we will be moving
		bool goingUp = false;
		if((x1 == lx && y1 == by) || (lx == x2 && y2 == by)) goingUp = true;
		sm::AABB aabb;
		int lastX = 0;

		if(goingUp){
			// we have to get all the intersections manually
			// insert the actual cell
			mCells.insert(&(mMatrix[lx][by]));

			lastX = lx;
			for(int y = by; y <= ty; ++y){
				for(int x = lastX; x <= rx; ++x){
					// check if this cell intersects the line
					getAABBFromCell(aabb, x, y);
					if(IntersectDetect::checkLineAABB(p1, p2, aabb)){
						// add it to the queue
						mCells.insert(&(mMatrix[x][y]));
						lastX = x;
					} else {
						// this not intersects; go up row
						break;
					}
				}
			}

		} else {
			mCells.insert(&(mMatrix[lx][ty]));

			lastX = lx;
			for(int y = ty; y >= by; --y){
				for(int x = lastX; x <= rx; ++x){
					// check if this cell intersects the line
					getAABBFromCell(aabb, x, y);
					if(IntersectDetect::checkLineAABB(p1, p2, aabb)){
						// add it to the queue
						mCells.insert(&(mMatrix[x][y]));
						lastX = x;
					} else {
						// this not intersects; go down row
						break;
					}
				}
			}


		}
	}

	// now for all the cells that we get we will get the elements that interseect
	// the line
	CellPtrSet::iterator it = mCells.begin(),
			itEnd = mCells.end();
	for(; it != itEnd; ++it){
		(*it)->getAllCollisionMask(mAuxCont, mask, p1, p2);
	}

	result.insert(result.end(), mAuxCont.begin(), mAuxCont.end());
}

