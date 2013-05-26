/*
 * GUIHelper.h
 *
 *  Created on: 18/05/2012
 *      Author: agustin
 *
 */

#ifndef GUIHELPER_H_
#define GUIHELPER_H_


#include <OgrePanelOverlayElement.h>
#include <OgreTexture.h>

namespace f_e {

class GUIHelper {
public:
	/**
	 * Create a new PanelOverlayElement
	 * @param	matName		The name of the material
	 * @param	rSize		The resulting size of the texture (width)
	 * @param	name		The name of the panel
	 */
	static Ogre::PanelOverlayElement *createPanel(const Ogre::String &matName,
			int &rSize, const Ogre::String &name = "");

	/**
	 * Bugfix for the nested overlay containers position. This function will
	 * get a overlay and will iterate over all the (child) containers and will
	 * set the new "relative" position
	 * @param	overlay		The overlay to fix the childs positions
	 */
	static void fixOverlayPosition(Ogre::Overlay *overlay);

	/**
	 * Ugly but fast recursively function used to reposition the overlaycontainer
	 */
	static void reposContainer(Ogre::OverlayContainer *o,
			Ogre::Real parentX, Ogre::Real parentY, Ogre::Real parentHeight,
			Ogre::Real parentWidth);
	static void reposContainer(Ogre::OverlayContainer *o);

	/**
	 * Get the absolute position of the container
	 * @param	cont	The container
	 * @param	top		The absolute top
	 * @param	left	The absolute left
	 * @param	right	The absolute right
	 * @param	bottom	The absolute bottom
	 */
	static void getAbsoluteGeometry(Ogre::OverlayContainer *cont, Ogre::Real &top,
			Ogre::Real &left, Ogre::Real &right, Ogre::Real &bottom);

	/**
	 * Destroy and remove (deallocate memory) of an overlay and its childrens.
	 * Automatically destroy the memory of the materials (textures)
	 * @param 	overlay		The overlay
	 */
	static void fullDestroyOverlay(Ogre::Overlay *overlay);
	static void fullDestroyOverlayElement(Ogre::OverlayContainer *elem);
	static void fullDestroyOverlaySingleE(Ogre::OverlayElement *elem);

	/**
	 * Finds a container inside a overlay (childrens) and return's it or returns
	 * 0 if have an overlay with that name (or part of that name).
	 * @param	overlay		Where we will search the container
	 * @param	subStrName	The part of the name of the container to find
	 * @return	cont		The container with subString name (subStrName)
	 * 						0 if the container is not found
	 */
	static Ogre::OverlayContainer *findContainer(Ogre::Overlay *overlay,
			const Ogre::String subStrName);
	static Ogre::OverlayContainer *findContainer(Ogre::OverlayContainer *overlay,
			const Ogre::String subStrName);

	/**
	 * Destroy / unload the memory of a material
	 * @param	mat		MaterialName
	 */
	static void destroyMaterial(const Ogre::String &mat);

	/**
	 * Get all the childrens of an overlay
	 * @param	ov		The overlay where we will retreive the childrens
	 * @param	ch		Childrens
	 */
	static void getChildrens(Ogre::Overlay *ov,
			std::vector<Ogre::OverlayContainer *> &ch);
	static void getChildrens(Ogre::OverlayContainer *ov,
			std::vector<Ogre::OverlayContainer *> &ch);



	////////////////////////////////////////////////////////////////////////////
	//							TEXTURES FUNCTIONS							  //
	////////////////////////////////////////////////////////////////////////////

	/**
	 * Returns the texture width of a material or -1 if have not texture
	 */
	static int getTextureWidth(const Ogre::String &materialName);
	static int getTextureHeight(const Ogre::String &materialName);
	static void getTexture(const Ogre::String &matName, Ogre::TexturePtr &result);
};

}

#endif /* GUIHELPER_H_ */
