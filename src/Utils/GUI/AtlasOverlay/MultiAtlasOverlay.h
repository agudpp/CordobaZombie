/* Wrapper to handle the atlas of a material (multi atlas).
 * MultiAtlasOverlay.h
 *
 *  Created on: 15/08/2012
 *      Author: agustin
 *
 */

#ifndef MULTIATLASOVERLAY_H_
#define MULTIATLASOVERLAY_H_


#include <OgrePanelOverlayElement.h>

#include "DebugUtil.h"
#include "GUIHelper.h"

class MultiAtlasOverlay {
public:
	MultiAtlasOverlay(Ogre::PanelOverlayElement *p = 0) :
		mCont(p)
	{}
	~MultiAtlasOverlay()
	{}

	/**
	 * Set/Get the container to be wrapped
	 */
	inline Ogre::PanelOverlayElement *getPanel(void);
	inline void setPanel(Ogre::PanelOverlayElement *p);

	/**
	 * Set the number of images we have in the material associated (this is
	 * the number of atlas images we have)
	 * @param	iprow		Number of images per row
	 * @param	ipcolumn	Number of images per column
	 */
	inline void setNumberImages(int iprow, int ipcolumn);

	/**
	 * Change the atlas position
	 * @param	row		The row to be selected
	 * @param	column	The column to be selected
	 */
	inline void changeAtlas(int row, int column);


private:
	Ogre::PanelOverlayElement		*mCont;
	float							mAtlasFactorX;
	float							mAtlasFactorY;
};


////////////////////////////////////////////////////////////////////////////////
inline Ogre::PanelOverlayElement *MultiAtlasOverlay::getPanel(void)
{
	return mCont;
}

////////////////////////////////////////////////////////////////////////////////
inline void MultiAtlasOverlay::setPanel(Ogre::PanelOverlayElement *p)
{
	ASSERT(p);
	mCont = p;
}

////////////////////////////////////////////////////////////////////////////////
inline void MultiAtlasOverlay::setNumberImages(int iprow, int ipcolumn)
{
	ASSERT(mCont);
	ASSERT(iprow > 0);
	ASSERT(ipcolumn > 0);

	int textSizex = GUIHelper::getTextureWidth(mCont->getMaterialName());
	mAtlasFactorX = static_cast<float>(textSizex) / static_cast<float>(ipcolumn);
	mAtlasFactorX = mAtlasFactorX / static_cast<float>(textSizex);

	int textSizey = GUIHelper::getTextureHeight(mCont->getMaterialName());
	mAtlasFactorY = static_cast<float>(textSizey) / static_cast<float>(iprow);
	mAtlasFactorY = mAtlasFactorY / static_cast<float>(textSizey);
}

////////////////////////////////////////////////////////////////////////////////
inline void MultiAtlasOverlay::changeAtlas(int row, int column)
{
	ASSERT(mCont);
	const Ogre::Real x1 = (mAtlasFactorX * column);
	const Ogre::Real y1 = (mAtlasFactorY * row);
	ASSERT(x1 + mAtlasFactorX <= 1.0f);
	ASSERT(y1 + mAtlasFactorY <= 1.0f);
	mCont->setUV(x1, y1, x1+mAtlasFactorX, y1+mAtlasFactorY);
}

#endif /* MULTIATLASOVERLAY_H_ */
