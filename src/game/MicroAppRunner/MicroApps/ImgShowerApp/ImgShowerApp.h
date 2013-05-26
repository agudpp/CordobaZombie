/*
 * ImgShowerApp.h
 *
 *  Created on: 10/11/2011
 *      Author: agustin
 *
 *
 * OpenVC License
 *
 * Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
 * Copyright (C) 2009, Willow Garage Inc., all rights reserved.
 * Third party copyrights are property of their respective owners.
 *
 * This software is provided by the copyright holders and contributors "as is" and
 * any express or implied warranties, including, but not limited to, the implied
 * warranties of merchantability and fitness for a particular purpose are disclaimed.
 * In no event shall the Intel Corporation or contributors be liable for any direct,
 * indirect, incidental, special, exemplary, or consequential damages
 * (including, but not limited to, procurement of substitute goods or services;
 * loss of use, data, or profits; or business interruption) however caused
 * and on any theory of liability, whether in contract, strict liability,
 * or tort (including negligence or otherwise) arising in any way out of
 * the use of this software, even if advised of the possibility of such damage.
 */

#ifndef IMGSHOWERAPP_H_
#define IMGSHOWERAPP_H_

#include <OgreOverlay.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>


#include "DebugUtil.h"
#include "MicroApp.h"

#define IMGSA_FADE_TIME			1.0f

class ImgShowerApp : public MicroApp {


public:
	ImgShowerApp();
	virtual ~ImgShowerApp();

	/* Set the image name to be loaded as full screen */
	inline void setOverlayName(const Ogre::String &name)
	{
		mOverlayName = name;
	}

	/* Loads the app */
	virtual void load(void);

	/* The function used to update the MicroApp in every frame */
	virtual MicroApp::microAppCode update(Ogre::Real frameTime);

	/* Unloads the app */
	virtual void unload(void);


private:
	Ogre::String			mOverlayName;
	Ogre::OverlayContainer	*mContainer;
	Ogre::Overlay			*mOverlay;
};

#endif /* IMGSHOWERAPP_H_ */
