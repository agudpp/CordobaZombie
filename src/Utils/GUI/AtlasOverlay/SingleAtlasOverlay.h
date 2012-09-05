/* Wrapper to handle the atlas of a material.
 *
 * SingleAtlasOverlay.h
 *
 *  Created on: 15/08/2012
 *      Author: agustin
 *
 */

#ifndef SINGLEATLASOVERLAY_H_
#define SINGLEATLASOVERLAY_H_

#include <OgrePanelOverlayElement.h>

#include "DebugUtil.h"
#include "GUIHelper.h"

class SingleAtlasOverlay {
public:
	/**
	 * Set the panel to be wrapped
	 */
	SingleAtlasOverlay(Ogre::PanelOverlayElement *p = 0):
		mCont(p)
	{}

	~SingleAtlasOverlay()
	{}

	/**
	 * Set/Get the container to be wrapped
	 */
	inline Ogre::PanelOverlayElement *getPanel(void);
	inline void setPanel(Ogre::PanelOverlayElement *p);

	/**
	 * Set the number of images we have in the material associated (this is
	 * the number of atlas images we have)
	 */
	inline void setNumberImages(int n);

	/**
	 * Change the atlas [position
	 */
	inline void changeAtlas(int p);


protected:
	Ogre::PanelOverlayElement		*mCont;
	float							mAtlasFactor;
};


////////////////////////////////////////////////////////////////////////////////
inline Ogre::PanelOverlayElement *SingleAtlasOverlay::getPanel(void)
{
	return mCont;
}

////////////////////////////////////////////////////////////////////////////////
inline void SingleAtlasOverlay::setPanel(Ogre::PanelOverlayElement *p)
{
	ASSERT(p);
	mCont = p;
}

////////////////////////////////////////////////////////////////////////////////
inline void SingleAtlasOverlay::setNumberImages(int n)
{
	ASSERT(mCont);
	ASSERT(n > 0);

	int textSize = GUIHelper::getTextureWidth(mCont->getMaterialName());
	mAtlasFactor = static_cast<float>(textSize) / static_cast<float>(n);
	mAtlasFactor = mAtlasFactor / static_cast<float>(textSize);
}

////////////////////////////////////////////////////////////////////////////////
inline void SingleAtlasOverlay::changeAtlas(int n)
{
	ASSERT(mCont);
	const Ogre::Real x1 = (mAtlasFactor * n);
	ASSERT(x1 + mAtlasFactor <= 1.0f);
	mCont->setUV(x1, 0.0f, x1+mAtlasFactor, 1.0f);
}

#endif /* SINGLEATLASOVERLAY_H_ */
