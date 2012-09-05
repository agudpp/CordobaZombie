/*
 * Backpack.cpp
 *
 *  Created on: 14/05/2012
 *      Author: agustin
 */

#include <OgreOverlayManager.h>
#include <OgreStringConverter.h>

#include <auto_ptr.h>

#include "GUIHelper.h"
#include "GlobalObjects.h"
#include "DebugUtil.h"
#include "Backpack.h"
#include "Util.h"

Ogre::OverlayContainer	*Backpack::mBackpack = 0;
int	Backpack::mCountReference = 0;



////////////////////////////////////////////////////////////////////////////////
void Backpack::loadBackpackOverlayElement(void)
{
	// load the background
	if(!mBackpack){
		Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
		Ogre::Overlay *overlay = om.getByName(BACKPACK_OVERLAY_NAME);
		ASSERT(overlay);
		overlay->show();

		// now get the container
		mBackpack = overlay->getChild(BACKPACK_OVERLAY_CONTAINTER_NAME);
		if(!mBackpack){
			debugRED("Error loading the %s backpack overlay container\n",
					BACKPACK_OVERLAY_CONTAINTER_NAME);
			ASSERT(false);
		}
		mBackpack->show();

		debugBLUE("BackpackCreated\n");
		showAndHideObjects();
	}
}

////////////////////////////////////////////////////////////////////////////////
void Backpack::showAndHideObjects(void)
{
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	std::vector<Ogre::OverlayContainer *> items;
	Ogre::Overlay *itemOverlay = om.getByName(BACKPACK_ITEMS_OVERLAY);

	ASSERT(itemOverlay);
	GUIHelper::getChildrens(itemOverlay, items);
	// show overlay
	itemOverlay->show();
	// hide items
	for(int i = items.size()-1; i >= 0; --i){
		items[i]->hide();
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Backpack::Backpack()
{
	loadBackpackOverlayElement();
	mCountReference++;
}

////////////////////////////////////////////////////////////////////////////////
Backpack::~Backpack()
{
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	Ogre::Overlay *overlay = om.getByName(BACKPACK_OVERLAY_NAME);
	if(overlay){
		GUIHelper::fullDestroyOverlay(overlay);
	}
	mCountReference--;
	if(mCountReference == 0){
		// destroy the backpack
		GUIHelper::fullDestroyOverlayElement(mBackpack);
		mBackpack = 0;
	}
	for(int i = mItems.size() - 1; i >= 0; --i){
		delete mItems[i];
	}
}

////////////////////////////////////////////////////////////////////////////////
void Backpack::addBackpackItem(BackpackItem *bi)
{
	ASSERT(bi);
	ASSERT(!hasBackpackItem(bi));

	bi->show();
	bi->updated();
	mItems.push_back(bi);
}

////////////////////////////////////////////////////////////////////////////////
void Backpack::removeBackpackItem(BackpackItem *bi)
{
	ASSERT(bi);
	ASSERT(hasBackpackItem(bi));

	// remove from the overlay from the backpack
	int i = mItems.size()-1;
	for(; i >= 0; --i){
		if(mItems[i] == bi){
			break;
		}
	}
	removeItem(i);
}

////////////////////////////////////////////////////////////////////////////////
void Backpack::removeBackpackItemUserDef(void *bi)
{
	ASSERT(bi);
	int i = mItems.size()-1;
	for(; i >= 0; --i){
		if(mItems[i]->getUserDef() == bi){
			break;
		}
	}
	removeItem(i);
}

////////////////////////////////////////////////////////////////////////////////
bool Backpack::hasBackpackItem(BackpackItem *bi) const
{
	ASSERT(bi);
	for(int i = mItems.size()-1; i >= 0; --i){
		if(mItems[i] == bi){
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
BackpackItem *Backpack::hasBackpackItemUserDef(void *ud)
{
	for(int i = mItems.size()-1; i >= 0; --i){
		if(mItems[i]->getUserDef() == ud){
			return mItems[i];
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
void Backpack::show(void)
{
	for(int i = mItems.size() -1; i >= 0; --i){
		mItems[i]->show();
	}

}
////////////////////////////////////////////////////////////////////////////////
void Backpack::hide(void)
{
	for(int i = mItems.size() -1; i >= 0; --i){
		mItems[i]->hide();
	}
}

////////////////////////////////////////////////////////////////////////////////
void Backpack::showBackpack(void)
{
	mBackpack->show();
}
////////////////////////////////////////////////////////////////////////////////
void Backpack::hideBackpack(void)
{
	mBackpack->hide();
}

////////////////////////////////////////////////////////////////////////////////
void Backpack::update(void)
{
	debugRED("TODO: por ahora estamos actualizando todos los objetos, "
			"probablemente no sea necesario...\n");
	for(int i = mItems.size()-1 ; i >= 0; --i) mItems[i]->updated();
}


