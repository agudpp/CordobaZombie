/*
 * MenuManager.h
 *
 *  Created on: 18/03/2012
 *      Author: agustin
 */

#ifndef MENUMANAGER_H_
#define MENUMANAGER_H_

#include <vector>
#include <set>

#include "MenuDefs.h"
#include "IMenu.h"
#include "CommonMath.h"
#include "DebugUtil.h"



class MenuManager
{
	typedef std::vector<IMenu *>	MenuCell;
	typedef std::vector<MenuCell>	MenuVec;
	typedef std::vector<MenuVec>	MenuMatrix;
	typedef std::set<IMenu *>		MenuSet;	// TODO: improve this with hash?


public:
	MenuManager();
	~MenuManager();

	/**
	 *  Builds the MenuMatrix
	 * @note The coordinates system of the window are tl = (0,0) and
	 * 		 br = (windowWidth, windowHeight).
	 *
	 * @param ww	The windows Width
	 * @param wh	The windows hegith
	 * @param cnX	The number of cells used in X
	 * @param cnY	The number of cells used in Y
	 */
	bool build(float ww, float wh, int cnX, int cnY);

	/**
	 * Add/Remove/Check a new IMenu to the system
	 */
	void addMenu(IMenu *menu);
	void removeMenu(IMenu *menu);
	bool hasMenu(IMenu *menu);

	/**
	 * Update the menu
	 * @return	This function will return true if the mouse is over some object
	 * 			or false if we are not over any object
	 * @note	This class use the GlobalObjects::mouse instance
	 */
	bool update(void);


private:
	/**
	 * Returns the corresponding values from a vector to the matrix
	 */
	inline int getXPosition(const float &x) const;
	inline int getYPosition(const float &y) const;

	/**
	 * Returns the cells that overlaps a AABB
	 */
	void getCellsFromAABB(const sm::AABB &aabb, std::vector<MenuCell *> &cells);

	/**
	 * Swap a value from a vector with the last value and remove the last
	 * value after
	 * @param	v	The vector where we will change the value
	 * @param	i	The index of the value to be swaped and removed
	 */
	inline void swapValue(std::vector<IMenu *> &v, int i) const;

private:
	float		mFactorX;
	float		mFactorY;
	float		mCellSizeX;
	float		mCellSizeY;
	MenuMatrix	mMatrix;
	int			mNumCellX;
	int			mNumCellY;
	sm::Point	mLastPoint;

	MenuSet		mLastInside;
};



inline int MenuManager::getXPosition(const float &x) const
{
	int r = int(x*mFactorX);
	if(r < 0) return 0;
	else if(r >= mNumCellX)	return mNumCellX-1;
	else return r;
}
inline int MenuManager::getYPosition(const float &y) const
{
	int r = int(y*mFactorY);
	if(r < 0) return 0;
	else if(r >= mNumCellY) return mNumCellY-1;
	else return r;
}


inline void MenuManager::swapValue(std::vector<IMenu *> &v, int i) const
{
	ASSERT(i < v.size());
	v[i] = v[v.size()-1];
	v.pop_back();
}


#endif /* MENUMANAGER_H_ */
