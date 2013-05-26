/*
 * CellphoneMapInfo.h
 *
 *  Created on: 26/06/2012
 *      Author: agustin
 */

#ifndef CELLPHONEMAPINFO_H_
#define CELLPHONEMAPINFO_H_

#include <OgreString.h>

#include "DebugUtil.h"

struct CellphoneMapInfo {
	// The name of the material to be used as map
	Ogre::String		materialName;

	/**
	 * Set the Dimensions of the map associated to the texture. This is the size
	 * of the level in ogre units
	 * width		The width of the level (x)
	 * height		The height of the level (z)
	 */
	Ogre::Real			width;
	Ogre::Real			height;


	inline Ogre::Real getXFactor(void) const
	{
		ASSERT(width > 0);
		return 1.0f/width;

	}
	inline Ogre::Real getYFactor(void) const
	{
		ASSERT(height > 0);
		return 1.0f/height;
	}

};

#endif /* CELLPHONEMAPINFO_H_ */
