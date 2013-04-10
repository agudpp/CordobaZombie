/*
 * GUIObject.h
 *
 *  Created on: 21/05/2012
 *      Author: agustin
 *
 */

#ifndef GUIOBJECT_H_
#define GUIOBJECT_H_


#include <OgreOverlayContainer.h>
#include <OgreVector2.h>

#include "DebugUtil.h"
#include "IMenu.h"

class GUIObject : public IMenu {
public:
	GUIObject();
	virtual ~GUIObject();

	/**
	 * Returns the GUIObject ID (name). This name is extracted from the
	 * container... So we first need to create the container
	 */
	inline const Ogre::String &getName(void);

	/**
	 * Returns the container
	 */
	inline Ogre::OverlayContainer	*getContainer(void);

	/**
	 * Set the container from outside
	 */
	inline void setContainer(Ogre::OverlayContainer *cont);

	/**
	 * Load from an overlay name
	 */
	void loadFromOverlay(const Ogre::String &overlayName, bool isTemaplate = false);

	/**
	 * Set the material name to be used
	 */
	void setMaterial(const Ogre::String &mat);

	/**
	 * Configure the overlay and the AABB used for the MenuManager
	 * @param pos	The position (relative to the windows size [0.0,1.0])
	 * @param size	The Size (relative to the windows size [0.0,1.0])
	 */
	virtual void configureGeometry(const Ogre::Vector2 &pos,
			const Ogre::Vector2 &size);

	/**
	 * Set visible or hide
	 */
	inline void setVisible(bool visible);


protected:
	/**
	 * Function called when the mouse is inside of this menu
	 */
	virtual void mouseInside(void) = 0;

	/**
	 * Function called when the mouse goes outside from the menu
	 */
	virtual void mouseOutside(void) = 0;

	/**
	 * Function called when the mouse is moving inside of this menu
	 */
	virtual void mouseMoving(void) = 0;




protected:
	Ogre::OverlayContainer		*mCont;
};




inline const Ogre::String &GUIObject::getName(void)
{
	ASSERT(mCont);
	return mCont->getName();
}

inline Ogre::OverlayContainer	*GUIObject::getContainer(void)
{
	return mCont;
}

inline void GUIObject::setContainer(Ogre::OverlayContainer *cont)
{
	ASSERT(cont);
	ASSERT(!mCont);
	mCont = cont;
}

inline void GUIObject::setVisible(bool visible)
{
	ASSERT(mCont);
	if(visible) mCont->show();
	else		mCont->hide();
}

#endif /* GUIOBJECT_H_ */
