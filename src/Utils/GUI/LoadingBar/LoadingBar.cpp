/*
 * LoadingBar.cpp
 *
 *  Created on: 23/05/2012
 *      Author: agustin
 */

#include <OgreOverlayManager.h>

#include "LoadingBar.h"

#include "GUIHelper.h"

LoadingBar::LoadingBar() :
		mProgress(0),
		mActualProgress(0),
		mMaxValue(-1),
		mContainer(0),
		mOverlay(0),
		mSliceImage(false)
{
}

LoadingBar::~LoadingBar()
{
	clear();
}



/**
 * Set the Materiales
 * @param	background	The background material
 * @param	bar			The material bar
 */
void LoadingBar::setOverlayName(const Ogre::String &overlay)
{
	ASSERT(!mOverlay);
	ASSERT(!mProgress);
	ASSERT(!mContainer);

	debugWARNING("REMOVER al chori esta funcionalidad, asi no dependemos de "
			"un Overlay, ademas podemos eliminar el OverlayPOinter al chori\n");

	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	mOverlay = overlayManager.getByName(overlay);
	ASSERT(mOverlay);

	// get the progress bar and background
	mContainer = static_cast<Ogre::OverlayContainer*>(
				GUIHelper::findContainer(mOverlay, "Background"));
	ASSERT(mContainer);

	mProgress = static_cast<Ogre::PanelOverlayElement*>(
			GUIHelper::findContainer(mOverlay, "ProgressBar"));
	ASSERT(mProgress);

//	GUIHelper::fixOverlayPosition(mOverlay);
	mOverlay->show();

	// get the size of the progress bar
	mMaxSize = mProgress->getWidth();

	setState(mActualProgress);
	mMaxValue = -1;
}

/**
 * Load from a container overlay (the overlay container should be the
 * "background" and this container must have a child with a substr
 * "ProgressBar" to be used as the bar.
 * @param	ovc		The OverlayContainer to be used
 */
void LoadingBar::setOverlayContainer(Ogre::OverlayContainer *ovc)
{
	ASSERT(!mOverlay);
	ASSERT(!mProgress);
	ASSERT(!mContainer);
	ASSERT(ovc);

	// get the progress bar and background
	mContainer = ovc;
	mProgress = static_cast<Ogre::PanelOverlayElement*>(
			GUIHelper::findContainer(ovc, "ProgressBar"));
	ASSERT(mProgress);

	// get the size of the progress bar
	mMaxSize = mProgress->getWidth();

	setState(mActualProgress);
	mMaxValue = -1;
}

/**
 * Set % of the bar [0.0, 100.0] %
 */
void LoadingBar::setState(float progress)
{
	ASSERT(mProgress);
	ASSERT(progress >= 0.0f && progress <= 100.0f);

	mActualProgress = progress;

	// change the size of the progress bar

	const float newWidth = (mActualProgress / 100.0f)  * mMaxSize;
	mProgress->setWidth(newWidth);
	if(mSliceImage){
		// use slice..
		mProgress->setUV(0.0f,0.0f,newWidth, 1.0f);
	}
}

/**
 * Clear all the data
 */
void LoadingBar::clear(void)
{
	if(mOverlay) {
		GUIHelper::fullDestroyOverlay(mOverlay);
	} else {
		if(mContainer){
			GUIHelper::fullDestroyOverlayElement(mContainer);
		}
	}

	mOverlay = 0;
	mProgress = 0;
	mContainer = 0;
}
