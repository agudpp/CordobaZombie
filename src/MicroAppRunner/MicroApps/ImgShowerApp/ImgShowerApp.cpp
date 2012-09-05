/*
 * ImgShowerApp.cpp
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

#include <OgreTextAreaOverlayElement.h>
#include <OgreColourValue.h>
#include <OgreMaterialManager.h>

#include "GlobalObjects.h"
#include "ImgShowerApp.h"

ImgShowerApp::ImgShowerApp() :
mContainer(0),
mOverlay(0)
{
	// TODO Auto-generated constructor stub

}

ImgShowerApp::~ImgShowerApp()
{
	// TODO Auto-generated destructor stub
}



/* Loads the app */
void ImgShowerApp::load(void)
{
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

	if(!mContainer){
		// Create a panel
		mContainer = static_cast<Ogre::OverlayContainer*>(
			overlayManager.createOverlayElement("Panel", "ImgShowerAppPanel"));
		mContainer->setMetricsMode(Ogre::GMM_RELATIVE);
		mContainer->setPosition(0, 0);
		mContainer->setDimensions(1, 1);
		mContainer->setMaterialName(mOverlayName); // Optional background material
		// Ensures that the material exists
#ifdef DEBUG
		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(
				mOverlayName,Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
		ASSERT(!mat.isNull());
#endif

	}

	if(!mOverlay){
		// Create an overlay, and add the panel
		mOverlay = overlayManager.create("ImgShowerAppOverlay");
		mOverlay->add2D(mContainer);
		mOverlay->show();
	}

	// extracted from http://www.ogre3d.org/tikiwiki/Creating+Overlays+via+Code

}

/* The function used to update the MicroApp in every frame */
MicroApp::microAppCode ImgShowerApp::update(Ogre::Real frameTime)
{

	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_ESCAPE)){
		return MicroApp::DONE;
	}

	return MicroApp::CONTINUE;
}

/* Unloads the app */
void ImgShowerApp::unload(void)
{

	if(mOverlay){
		Ogre::OverlayManager::getSingleton().destroy(mOverlay);
		mOverlay = 0;
	}
	if(mContainer){
		Ogre::MaterialManager::getSingleton().unload(mOverlayName);
		Ogre::OverlayManager::getSingleton().destroyOverlayElement(mContainer);
		mContainer = 0;
	}


}


