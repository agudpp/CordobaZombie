/*
 * GUIManager.h
 *
 *  Created on: 18/05/2012
 *      Author: agustin
 *
 */

#ifndef GUIMANAGER_H_
#define GUIMANAGER_H_

#include <OgreOverlay.h>

#include <vector>

#include "GUIObject.h"
#include "MenuManager.h"
#include "tinyxml.h"
#include "CommonMath.h"


class TiXmlElement;

class GUIManager {
public:
	GUIManager(MenuManager *mm);
	~GUIManager();

	/**
	 * Add a GUIObject to the overlay (and to the MenuManager if and only if
	 * isStatic = false).
	 * @param	obj			The object to be added
	 * @param	isStatic	Flag used to know if the GUIObject will be added to
	 * 						the MenuManager (isStatic = false) or not
	 * 						(isStatic = true)
	 */
	void addObject(GUIObject *obj, bool isStatic = false);

	/**
	 * Hide/Show all the elements
	 */
	void setVisible(bool visible);

	/**
	 * Delete all the GUIObjects
	 */
	void freeAll(void);

	/**
	 * Activate / Deactivate some GUIObject from the ManuManager.
	 * @param	go			GUIObject to activate/deactivate
	 * @param	activate	Activate/Decativate flag
	 */
	void setMenuAtivation(GUIObject *go, bool activate);



private:
	/**
	 * Create the main overlay
	 */
	void createOverlay(void);



private:
	typedef std::vector<Ogre::OverlayContainer *>	GUIObjectsVec;


	MenuManager			*mMenuManager;
	Ogre::Overlay		*mOverlay;
	GUIObjectsVec		mGUIObjects;

};

#endif /* GUIMANAGER_H_ */
