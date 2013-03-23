/*
 * LoadingBar.h
 *
 *  Created on: 23/05/2012
 *      Author: agustin
 */

#ifndef LOADINGBAR_H_
#define LOADINGBAR_H_

#include <OgrePanelOverlayElement.h>
#include <OgreOverlay.h>

#include "CommonMath.h"
#include "DebugUtil.h"


class LoadingBar
{
public:
	LoadingBar();
	~LoadingBar();

	/**
	 * Call this function to set the "slice texture" functionality: When this
	 * feature is activated we will use the "ProgressBar" material as the whole
	 * bar and we will not scale the material, we will show only the part of
	 * the texture corresponding to the progress bar size.
	 * By default this functionality is inactive
	 */
	inline void useSliceFeature(bool useSlice);

	/**
	 * Set the overlay script name. This should contain the ProgressBar overlay
	 * called as "bar" say
	 * @param	overlay		The overlayName to load
	 */
	void setOverlayName(const Ogre::String &overlay);

	/**
	 * Load from a container overlay (the overlay container should be the
	 * "background" and this container must have a child with a substr
	 * "ProgressBar" to be used as the bar.
	 * @param	ovc		The OverlayContainer to be used
	 */
	void setOverlayContainer(Ogre::OverlayContainer *ovc);

	/**
	 * Set the maximum value to use
	 */
	inline void setMaximumValue(float max);

	/**
	 * Use this function if you already set the MaximumValue and set the actual
	 * value to calculate automatically the %
	 */
	inline void setActualValue(float v);
	inline float actualValue(void) const;

	/**
	 * Set % of the bar [0.0, 100.0] %
	 */
	void setState(float progress);

	/**
	 * Clear all the data
	 */
	void clear(void);

	/**
	 * Show/Hide
	 */
	inline void show(void);
	inline void hide(void);

private:
	Ogre::PanelOverlayElement 	*mProgress;
	float						mActualProgress;
	float						mMaxSize;
	Ogre::OverlayContainer		*mContainer;
	Ogre::Overlay				*mOverlay;
	float						mMaxValue;
	bool						mSliceImage;
};




inline void LoadingBar::useSliceFeature(bool useSlice)
{
	mSliceImage = useSlice;
}

/**
 * Set the maximum value to use
 */
inline void LoadingBar::setMaximumValue(float max)
{
	ASSERT(max > 0);
	mMaxValue = max;
}

/**
 * Use this function if you already set the MaximumValue and set the actual
 * value to calculate automatically the %
 */
inline void LoadingBar::setActualValue(float v)
{
	ASSERT(mMaxValue > 0);
	ASSERT(v >= 0);
	if(v > mMaxValue) v = mMaxValue;
	setState(v/mMaxValue*100.0f);
}
inline float LoadingBar::actualValue(void) const
{
    return mActualProgress;
}

inline void LoadingBar::show(void)
{
	mContainer->show();
}
inline void LoadingBar::hide(void)
{
	mContainer->hide();
}

#endif /* LOADINGBAR_H_ */
