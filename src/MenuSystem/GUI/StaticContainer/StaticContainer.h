/*
 * StaticContainer.h
 *
 *  Created on: 18/05/2012
 *      Author: agustin
 *
 */

#ifndef STATICCONTAINER_H_
#define STATICCONTAINER_H_

#include <OgreString.h>
#include <OgreOverlayContainer.h>

#include "GUIObject.h"

class StaticContainer  : public GUIObject {
public:
	StaticContainer();
	~StaticContainer();

	/**
	 * Set the material name to be used
	 */
	void setMaterial(const Ogre::String &mat);



protected:
	/**
	 * NOT IMPLEMENTED FUNCTIONS! this static containers shouldn't be added to
	 * the menu manager
	 */
	void mouseInside(const MenuMouse *);
	void mouseOutside(const MenuMouse *);
	void mouseMoving(const MenuMouse *);

};


#endif /* STATICCONTAINER_H_ */
