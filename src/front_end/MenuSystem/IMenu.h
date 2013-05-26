/*
 * IMenu.h
 *
 *  Created on: 19/03/2012
 *      Author: agustin
 */

#ifndef IMENU_H_
#define IMENU_H_


#include "MenuDefs.h"
#include "GlobalObjects.h"
#include "CommonMath.h"
#include "DebugUtil.h"

class MenuManager;

class IMenu
{
public:
	IMenu(){ASSERT(mMenuManager);};
	virtual ~IMenu(){setEnable(false);};

	/**
	 * Returns the AABB associated to this menu
	 */
	inline const core::AABB &getAABB(void) const;

	/**
	 * Set the AABB to this menu
	 */
	inline void setAABB(const core::AABB &aabb);
	inline void setRelativeAABB(const core::AABB &aabb);

	/**
	 * Check if a point is inside of the menu
	 */
	inline bool pointInside(const core::Point &p) const;

	/**
	 * Activate this menu in the MenuManager or not
	 */
	void setEnable(bool enable);

	/**
	 * Set the MenuManager
	 */
	static void setMenuManager(MenuManager *mm);


protected:
	friend class MenuManager;
	/**
	 * Function called when the mouse is inside of this menu
	 */
	virtual void mouseInside(void) = 0;

	/**
	 * Function called when the mouse goes outside from the menu
	 */
	virtual void mouseOutside(void) = 0;

	/**
	 * Function called when the mouse is moving inside of this menu
	 */
	virtual void mouseMoving(void) = 0;


protected:
	core::AABB			mAABB;


	static MenuManager	*mMenuManager;

};


/**
 * Returns the AABB associated to this menu
 */
inline const core::AABB &IMenu::getAABB(void) const
{
	return mAABB;
}

/**
 * Set the AABB to this menu
 */
inline void IMenu::setAABB(const core::AABB &aabb)
{
	mAABB = aabb;
}
inline void IMenu::setRelativeAABB(const core::AABB &aabb)
{
	ASSERT(aabb.tl.x >= 0.0f && aabb.tl.x <= 1.0f);
	ASSERT(aabb.tl.y >= 0.0f && aabb.tl.y <= 1.0f);
	ASSERT(aabb.br.y >= 0.0f && aabb.br.y <= 1.0f);
	ASSERT(aabb.br.x >= 0.0f && aabb.br.x <= 1.0f);

	mAABB.tl.x = aabb.tl.x * GLOBAL_WINDOW->getWidth();
	mAABB.tl.y = aabb.tl.y * GLOBAL_WINDOW->getHeight();
	mAABB.br.y = aabb.br.y * GLOBAL_WINDOW->getHeight();
	mAABB.br.x = aabb.br.x * GLOBAL_WINDOW->getWidth();
}


/**
 * Check if a point is inside of the menu
 */
inline bool IMenu::pointInside(const core::Point &p) const
{
	return mAABB.tl.x <= p.x && mAABB.tl.y <= p.y &&
			mAABB.br.x >= p.x && mAABB.br.y >= p.y;
}



#endif /* IMENU_H_ */
