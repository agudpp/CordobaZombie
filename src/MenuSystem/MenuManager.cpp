/*
 * MenuManager.cpp
 *
 *  Created on: 18/03/2012
 *      Author: agustin
 */

#include <algorithm>


#include "MenuManager.h"
#include "InputMouse.h"


namespace {
inline int getIndex(const std::vector<IMenu *> &vec, const IMenu *elem)
{
    for(size_t size = vec.size(), i = 0; i < size; ++i)
        if (vec[i] == elem) return i;
    return -1;
}

inline void removeElement(std::vector<IMenu *> &vec, const size_t index)
{
    ASSERT(index < vec.size());
    vec[index] = vec[vec.size()-1];
    vec.pop_back();
}
}

////////////////////////////////////////////////////////////////////////////////
void MenuManager::getCellsFromAABB(const sm::AABB &aabb,
		std::vector<MenuCell *> &cells)
{
	ASSERT(!mMatrix.empty());
	int tlx = getXPosition(aabb.tl.x),tly = getYPosition(aabb.tl.y),
			brx = getXPosition(aabb.br.x), bry = getYPosition(aabb.br.y);

	cells.clear();
	for(int i = tlx; i <= brx; ++i){
		for(int j = tly; j <= bry; ++j){
			cells.push_back(&(mMatrix[i][j]));
		}
	}

}



MenuManager::MenuManager()
{
	debugRED("TODO: No estamos soportando poder mover los botones una vez "
			"que son agregados al manager. Deberiamos poder moverlos\n");
	debugRED("TODO2: Capaz que al pedo estamos intentando manejar mas de 1 boton "
			"al mismo tiempo cuando con 1 solo alcanza (osea que el mouse"
			" pueda estar sobre 1 boton al mismo tiempo capaz que alcanza)\n");
}

MenuManager::~MenuManager()
{

}


////////////////////////////////////////////////////////////////////////////////
bool MenuManager::build(float ww, float wh, int cnX, int cnY)
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

	mCellSizeX = ww / static_cast<float>(cnX);
	mCellSizeY = wh / static_cast<float>(cnY);

	// create the factors
	mFactorX = static_cast<float>(cnX) / ww;
	mFactorY = static_cast<float>(cnY) / wh;

	debug("WindowWidth: %f\tWindowHeight:%f\n", ww, wh);
	debug("MenuManager Build: cellSizeX: %f\t cellSizeY: %f\n",
			mCellSizeX, mCellSizeY);
}

////////////////////////////////////////////////////////////////////////////////
void MenuManager::addMenu(IMenu *menu)
{
	ASSERT(menu);

	// get all the cells that we have to put the menu
	std::vector<MenuCell *> cells;
	getCellsFromAABB(menu->getAABB(), cells);

	for(int i = cells.size()-1; i >= 0; --i){
		// check that the menu doesnt exists
#ifdef DEBUG
		std::vector<IMenu *>::iterator it = std::find(cells[i]->begin(), cells[i]->end(), menu);
		ASSERT(it == cells[i]->end());
#endif
		cells[i]->push_back(menu);
	}
	debug("Adding menu in %lu cells\n", cells.size());
}
////////////////////////////////////////////////////////////////////////////////
void MenuManager::removeMenu(IMenu *menu)
{
	ASSERT(menu);

	// get all the cells that we have to put the menu
	std::vector<MenuCell *> cells;
	getCellsFromAABB(menu->getAABB(), cells);

	for(int i = cells.size()-1; i >= 0; --i){
		// find the menu and remove it
		MenuCell *v = cells[i];
		for(int j = v->size()-1; j >= 0; --j ){
			if((*v)[j] == menu){
				// remove and continue to the next cell
				swapValue(*v, j);
				break;
			}
		}
	}

	// remove it from the the vec
	int index = getIndex(mLastInside, menu);
	if (index > 0) {
	    removeElement(mLastInside, index);
	}
}
////////////////////////////////////////////////////////////////////////////////
bool MenuManager::hasMenu(IMenu *menu)
{
	ASSERT(menu);
	// get all the cells that we have to put the menu
	std::vector<MenuCell *> cells;
	getCellsFromAABB(menu->getAABB(), cells);

	for(int i = cells.size()-1; i >= 0; --i){
		// find the menu
		MenuCell *v = cells[i];
		for(int j = v->size()-1; j >= 0; --j ){
			if((*v)[j] == menu){
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
bool MenuManager::update(void)
{
	ASSERT(!mMatrix.empty());

	static sm::Point p;
	static bool lastStepClicked = false;
	static bool acatualStepClicked = false;

	p.x = input::InputMouse::absX();
	p.y = input::InputMouse::absY();

	// if the mouse move then we do all the logic
	if(mLastPoint == p){
		// the mouse doesn't move, then we only update all the actual menues
		// for only check the mouse state
		for(size_t i = 0; i < mLastInside.size(); ++i){
			(mLastInside[i])->mouseMoving();
		}

		return !mLastInside.empty();
	}

	// now check for all the menus that the mouse is inside of
	int x = getXPosition(input::InputMouse::absX()),
	        y = getYPosition(input::InputMouse::absY());

	MenuCell &cell = mMatrix[x][y];
	IMenu *m;

//	debug("MousePos:%f\t%f\n", p.x, p.y);

	// check for menus that are older
	for(size_t i = 0; i < mLastInside.size(); ++i){
		if(!(mLastInside[i])->pointInside(p)){
			// is and old one
			m = mLastInside[i];
			removeElement(mLastInside, i);
			m->mouseOutside();
		}
	}

	for(size_t size = cell.size(), i = 0; i < size; ++i){
		m = cell[i];
		if(m->pointInside(p)){
			// check if is already in the lastInside
		    const int index = getIndex(mLastInside, m);
			if(index < 0){
				// is not in the last inside, so is a new one
				m->mouseInside();
				mLastInside.push_back(m);
			} else {
				// is in the last inside, so we only have to update it
				m->mouseMoving();
			}
		}
	}

	// update the last point
	mLastPoint = p;

	return !mLastInside.empty();
}




