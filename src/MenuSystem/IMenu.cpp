/*
 * IMenu.cpp
 *
 *  Created on: 04/06/2012
 *      Author: agustin
 */

#include "MenuManager.h"
#include "IMenu.h"

MenuManager	*IMenu::mMenuManager = 0;


/**
 * Set the MenuManager
 */
void IMenu::setMenuManager(MenuManager *mm)
{
	ASSERT(mm);
#ifdef DEBUG
    if (mMenuManager != 0) {
        debugWARNING("We are setting another Mngr to the IMenu\n");
    }
#endif
	mMenuManager = mm;
}

/**
 * Activate this menu in the MenuManager or not
 */
void IMenu::setEnable(bool enable)
{
	ASSERT(mMenuManager);
	if(!enable){
		mMenuManager->removeMenu(this);
	} else {
		if(!mMenuManager->hasMenu(this)){
			mMenuManager->addMenu(this);
		}
	}
}


