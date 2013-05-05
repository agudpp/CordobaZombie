/*
 * MenuButton.h
 *
 *  Created on: 16/05/2012
 *      Author: agustin
 *
 */

#ifndef MENUBUTTON_H_
#define MENUBUTTON_H_

#include <boost/shared_ptr.hpp>

#include <OgreString.h>
#include <OgrePanelOverlayElement.h>

#include "GUIObject.h"

class MenuButton : public GUIObject {
private:
	/**
	 * This is the order that we have to put the image in the texture
	 */
	enum {
		S_ACTIVE,
		S_MOUSE_OVER,
		S_PRESSED,
		S_INACTIVE
	};

	enum {
		FLAG_NONE				= 0,
		FLAG_LEFT_PRESSED 		= 1,
		FLAG_RIGHT_PRESSED 		= 2,
		FLAG_MIDDLE_PRESSED 	= 4,
	};

public:
	enum ActionType {
		ACTION_NORMAL,
		ACTION_SELECTED,
	};

public:
	MenuButton();
	virtual ~MenuButton();

	/**
	 * Set active/inactive the button
	 */
	void setActive(bool active);
	inline bool isActive(void) const;

	/**
	 * Set the material name to be used
	 */
	void createFromMaterial(const Ogre::String &mat);

	/**
	 * Configure the button with the info
	 * @param	atlasSize	The size of the texture atlas (it should be group
	 * 						in width, not height). This is the size of one of
	 * 						the rectangles in the texture file.
	 */
	bool configure(int atlasSize);

	/**
	 * Reset the atlas to the main state
	 */
	inline void resetAtlas(void);

	/**
	 * This function is called when the material has change and we need to
	 * recalculate the TextureWidth and the atlas size.
	 */
	void updateMaterial(const Ogre::String &mat);

	/**
	 * Configure from an Overlay container.
	 * This function will set the position of the IMenu using the position of
	 * the container and will configure the atlasSize to using the material
	 * and the texture size / 3.
	 * This function although set the container of the element
	 *
	 * @param cont		The container used to configure all
	 */
	void configureAll(Ogre::OverlayContainer *cont);

	/**
	 * Set/Get the action type
	 * @note	ACTION_NORMAL	Will be used in the normal button cases
	 * 			ACTION_SELECTED	Will be used when we want to set the button as
	 * 							selected.
	 */
	inline void setActionType(ActionType t);
	inline ActionType getActionType(void) const;



protected:
	/**
	 * Function called when the mouse is inside of this menu
	 */
	virtual void mouseInside(void);

	/**
	 * Function called when the mouse goes outside from the menu
	 */
	virtual void mouseOutside(void);

	/**
	 * Function called when the mouse is moving inside of this menu
	 */
	virtual void mouseMoving(void);

	////////////////////////////////////////////////////////////////////////////
	////						CALLBACKS TO MPLEMENT						////
	////////////////////////////////////////////////////////////////////////////

	/**
	 * When the button is pressed with right button (after is released)
	 */
	virtual void rightPressed(void);

	/**
	 * When the button is left Pressed
	 */
	virtual void leftPressed(void);


	////////////////////////////////////////////////////////////////////////////



private:
	/**
	 * Configure the atlas to the corresponding position
	 */
	void configureAtlas(int pos);


private:
	int							mFlags;
	bool						mActive;
	int 						mAtlasSize;
	int							mTextureWidth;
	ActionType					mActionType;
};

// typedef the shared ptr here
//
typedef boost::shared_ptr<MenuButton> MenuButtonPtr;


inline bool MenuButton::isActive(void) const
{
	return mActive;
}

inline void MenuButton::resetAtlas(void)
{
    configureAtlas(S_ACTIVE);
}

inline void MenuButton::setActionType(MenuButton::ActionType t)
{
	mActionType = t;
}
inline MenuButton::ActionType MenuButton::getActionType(void) const
{
	return mActionType;
}


#endif /* MENUBUTTON_H_ */
