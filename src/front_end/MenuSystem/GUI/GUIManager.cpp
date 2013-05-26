/*
 * GUIManager.cpp
 *
 *  Created on: 18/05/2012
 *      Author: agustin
 *
 */
#include <OgreStringConverter.h>
#include <OgreOverlayManager.h>

#include "DebugUtil.h"
#include "GUIManager.h"



////////////////////////////////////////////////////////////////////////////////
void GUIManager::createOverlay(void)
{
	ASSERT(!mOverlay);
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

	// Create an overlay, and add the panel
	mOverlay = overlayManager.create("GUIOverlay");
	mOverlay->show();
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
GUIManager::GUIManager(MenuManager *mm) :
		mMenuManager(mm)
{
	ASSERT(mm);
	createOverlay();
}

////////////////////////////////////////////////////////////////////////////////
GUIManager::~GUIManager()
{
	freeAll();
}


////////////////////////////////////////////////////////////////////////////////
void GUIManager::addObject(GUIObject *obj, bool isStatic)
{
	ASSERT(obj);
	ASSERT(obj->getContainer());

	// add to the overlay
	mOverlay->add2D(obj->getContainer());

	if(!isStatic){
		// we need to add the object to be tracked by the menu manager
		mMenuManager->addMenu(obj);
	}
}

////////////////////////////////////////////////////////////////////////////////
void GUIManager::setVisible(bool visible)
{
	if(visible){
		mOverlay->show();
	} else {
		mOverlay->hide();
	}
}

////////////////////////////////////////////////////////////////////////////////
void GUIManager::freeAll(void)
{
	debugRED("TODO: tenemos que hacer esto\n");
}

////////////////////////////////////////////////////////////////////////////////
void GUIManager::setMenuAtivation(GUIObject *go, bool activate)
{
	if(activate){
		mMenuManager->addMenu(go);
	} else {
		mMenuManager->removeMenu(go);
	}
}
