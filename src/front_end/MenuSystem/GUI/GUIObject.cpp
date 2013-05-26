/*
 * GUIObject.cpp
 *
 *  Created on: 21/05/2012
 *      Author: agustin
 *
 */
#include <OgreOverlayManager.h>

#include "GlobalObjects.h"

#include "GUIObject.h"
#include "GUIHelper.h"

GUIObject::GUIObject() :
mCont(0)
{

}

GUIObject::~GUIObject() {
	// remove the panel
	if(mCont){
	    f_e::GUIHelper::fullDestroyOverlayElement(mCont);
	}
}


/**
 * Load from an overlay name
 */
void GUIObject::loadFromOverlay(const Ogre::String &overlayName, bool isTemaplate)
{
	ASSERT(!mCont);

	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
	mCont = static_cast<Ogre::OverlayContainer *>(
			om.getOverlayElement(overlayName, isTemaplate));
	if(!mCont){
		debugRED("Error loading the overlay: %s\n", overlayName.c_str());
		ASSERT(false);
	}
}

/**
 * Set the material name to be used
 */
void GUIObject::setMaterial(const Ogre::String &mat)
{
	ASSERT(mCont);
	mCont->setMaterialName(mat);
}

/**
 * Configure the overlay
 * @param pos	The position (relative to the windows size [0.0,1.0])
 * @param size	The Size (relative to the windows size [0.0,1.0])
 */
void GUIObject::configureGeometry(const Ogre::Vector2 &pos,
		const Ogre::Vector2 &size)
{
	ASSERT(mCont);
	ASSERT(pos.x >= 0.0f && pos.x <= 1.0f);
	ASSERT(pos.y >= 0.0f && pos.y <= 1.0f);
	ASSERT(size.x >= 0.0f && size.x <= 1.0f);
	ASSERT(size.y >= 0.0f && size.y <= 1.0f);

	mCont->setMetricsMode(Ogre::GMM_RELATIVE);

	mCont->setPosition(pos.x, pos.y);
	mCont->setDimensions(size.x, size.y);

	// configure the AABB for the menu manager.
	mAABB.tl.x = GLOBAL_WINDOW->getWidth() * pos.x;
	mAABB.tl.y = GLOBAL_WINDOW->getHeight() * pos.y;
	mAABB.br.x = mAABB.tl.x + GLOBAL_WINDOW->getWidth() * size.x;
	mAABB.br.y = mAABB.tl.y + GLOBAL_WINDOW->getHeight() * size.y;

}
