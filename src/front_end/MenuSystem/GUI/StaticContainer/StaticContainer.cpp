/*
 * StaticContainer.cpp
 *
 *  Created on: 18/05/2012
 *      Author: agustin
 *
 */

#include <OgreOverlayManager.h>

#include "DebugUtil.h"
#include "GUIHelper.h"

#include "StaticContainer.h"

StaticContainer::StaticContainer()
{

}

StaticContainer::~StaticContainer()
{
	if(mCont){
		Ogre::OverlayManager::getSingleton().destroyOverlayElement(mCont);
	}
}

/**
 * Set the material name to be used
 */
void StaticContainer::setMaterial(const Ogre::String &mat)
{
	int aux;
	mCont = GUIHelper::createPanel(mat, aux);
	ASSERT(mCont);
}


void StaticContainer::mouseInside()
{
	debugRED("ERROR: some container was added to the menu manager... or "
			"someone is calling this function...\n");
	ASSERT(false);
}
void StaticContainer::mouseOutside()
{
	debugRED("ERROR: some container was added to the menu manager... or "
			"someone is calling this function...\n");
	ASSERT(false);
}
void StaticContainer::mouseMoving()
{
	debugRED("ERROR: some container was added to the menu manager... or "
			"someone is calling this function...\n");
	ASSERT(false);
}


