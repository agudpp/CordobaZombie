/*
 * ExtraActionPanel.cpp
 *
 *  Created on: 16/06/2012
 *      Author: agustin
 */

#include <OgreOverlayManager.h>

#include "DebugUtil.h"
#include "GUIHelper.h"
#include "ExtraActionPanel.h"


namespace hud {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ExtraActionPanel::ExtraActionPanel() :
mOverlay(0)
{

}

////////////////////////////////////////////////////////////////////////////////
ExtraActionPanel::~ExtraActionPanel()
{

}


////////////////////////////////////////////////////////////////////////////////
void ExtraActionPanel::load(void)
{
	if(mOverlay) unload();
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	mOverlay = om.getByName(EXTRA_ACTION_PANEL_OVERLAY);
	ASSERT(mOverlay);

	// now iterate over all the childrens
	std::vector<Ogre::OverlayContainer *> childs;
	Ogre::OverlayContainer *parent = mOverlay->getChild("ActionPanel/Main");
	ASSERT(parent);
	f_e::GUIHelper::getChildrens(parent, childs);
	ASSERT(!childs.empty());

	for(int i = 0; i < childs.size(); ++i){
		debugGREEN("Creating new ExtreActionPanl %s\n", childs[i]->getName().c_str());
		ExtraAction	*ea = new ExtraAction;
		ea->loadFromOverlay(childs[i]);
		mElements.push_back(ea);
	}
	mActualEmpty = 0;
	mOverlay->show();
}

////////////////////////////////////////////////////////////////////////////////
void ExtraActionPanel::unload(void)
{
	if(mOverlay){
	    f_e::GUIHelper::fullDestroyOverlay(mOverlay);
		mOverlay = 0;
	}
	for(int i = 0; i < mElements.size(); ++i){
		delete mElements[i];
	}
	mElements.clear();
}

////////////////////////////////////////////////////////////////////////////////
void ExtraActionPanel::show(void)
{
	if(mOverlay) mOverlay->show();
}
void ExtraActionPanel::hide(void)
{
	if(mOverlay) mOverlay->hide();
}

////////////////////////////////////////////////////////////////////////////////
int ExtraActionPanel::getNumActualActions(void) const
{
	return mActualEmpty;
}

////////////////////////////////////////////////////////////////////////////////
void ExtraActionPanel::removeAction(int pos)
{
	ASSERT(pos >= 0 && pos < mActualEmpty);

	// We have to sort the array if we remove some action that wasn't the last
	// disable the first
	ExtraAction *toRemove = mElements[pos];
	ASSERT(toRemove);
	toRemove->disableAction();

	// TODO: dejamos todo los otros como estaban no los movemos de lugar.
	mActualEmpty--;
}

////////////////////////////////////////////////////////////////////////////////
int ExtraActionPanel::addAction(ExtraAction::Callback *callback,
		const Ogre::String &material)
{
	ASSERT(mActualEmpty >= 0);
	if(mActualEmpty >= mElements.size()) return -1; // no more size available
	// else we can add a new one
	ASSERT(callback);

	// get the first available ea
	ExtraAction *ea = 0;
	for(int i = 0; i < mElements.size(); ++i){
		if(!mElements[i]->isActionEnable()){
			ea = mElements[i];
			break;
		}
	}
	ASSERT(ea);
	ea->setCallback(callback);
	ea->changeMaterial(material);
	ea->enableAction();

	return mActualEmpty++;
}

}
