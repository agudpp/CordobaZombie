/*
 * MenuButton.cpp
 *
 *  Created on: 16/05/2012
 *      Author: agustin
 *
 */

#include <OgreOverlayManager.h>
#include <OgreTextureManager.h>
#include <OgreTextureUnitState.h>

#include "DebugUtil.h"
#include "GUIHelper.h"
#include "InputMouse.h"

#include "MenuButton.h"



////////////////////////////////////////////////////////////////////////////////
void MenuButton::configureAtlas(int pos)
{
	ASSERT(mCont);
	ASSERT(mTextureWidth > 0);
	ASSERT(mAtlasSize * pos <= mTextureWidth);

	// get the %
	Ogre::Real x1,x2;
	const Ogre::Real size = static_cast<float>(mAtlasSize) /
	    static_cast<float>(mTextureWidth);
	x1 = size * static_cast<float>(pos);
	x2 = x1 + size;

	debug("Configuring atlas: AtlasSize: %d, textureW: %d, x1: %f, x2: %f, pos: %d\n",
			mAtlasSize, mTextureWidth, x1, x2, pos);
	static_cast<Ogre::PanelOverlayElement *>(mCont)->setUV(x1, 0.0f, x2, 1.0f);

	// update the state
	mCurrentState = pos;
}



////////////////////////////////////////////////////////////////////////////////
MenuButton::MenuButton() :
    mFlags(FLAG_NONE),
    mActive(true),
    mAtlasSize(-1),
    mActionType(ACTION_NORMAL),
    mCurrentState(S_ACTIVE)
{

}

////////////////////////////////////////////////////////////////////////////////
MenuButton::~MenuButton()
{

}


/**
 * Set active/inactive the button
 */
void MenuButton::setActive(bool active)
{
	mActive = active;
	if(!active){
		configureAtlas(S_INACTIVE);
	} else {
		configureAtlas(S_ACTIVE);
	}
}

void MenuButton::createFromMaterial(const Ogre::String &mat)
{
	ASSERT(!mCont);

	// first we create the panel
	mCont = f_e::GUIHelper::createPanel(mat, mTextureWidth);
	ASSERT(mCont);

	ASSERT(mAtlasSize > 0)
	// configure the texture
	configureAtlas(S_ACTIVE);
}

/**
 * Configure the button with the
 * @param	atlasSize	The size of the texture atlas (it should be group
 * 						in width, not height)
 */
bool MenuButton::configure(int atlasSize)
{
	mAtlasSize = atlasSize;

	// get texture size
	ASSERT(mCont);
	Ogre::String text = mCont->getMaterial()->getTechnique(0)->getPass(0)->
			getTextureUnitState(0)->getTextureName();
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().getByName(text);
	ASSERT(!texture.isNull());
	mTextureWidth = texture->getWidth();

	configureAtlas(S_ACTIVE);

	return true;
}


/**
 * This function is called when the material has change and we need to
 * recalculate the TextureWidth and the atlas size.
 */
void MenuButton::updateMaterial(const Ogre::String &matName)
{
	mCont->setMaterialName(matName);
	// get the texture size
	mTextureWidth = f_e::GUIHelper::getTextureWidth(matName);
	ASSERT(mTextureWidth > 0);
	configure(mTextureWidth/3);
}

/**
 * Configure from an Overlay container.
 * This function will set the position of the IMenu using the position of
 * the container and will configure the atlasSize to using the material
 * and the texture size / 3.
 */
void MenuButton::configureAll(Ogre::OverlayContainer *cont)
{
	ASSERT(cont);

	setContainer(cont);

	// first of all remove from the MenuManager
	core::Vector2 tl, br;
	f_e::GUIHelper::getAbsoluteGeometry(cont, tl.y,tl.x,br.x, br.y);
	setRelativeAABB(core::AABB(tl,br));
	Ogre::MaterialPtr mat = cont->getMaterial();
	if(mat.isNull() || !mat->getTechnique(0) || !mat->getTechnique(0)->getPass(0)){
		debugWARNING("Container %s has no material to set the atlas size\n",
				cont->getName().c_str());
	} else {
		updateMaterial(mat->getName());
	}
}

/**
 * Function called when the mouse is inside of this menu
 */
void MenuButton::mouseInside()
{
	if(!mActive){
		return;
	}

	// change the image to S_MOUSE_OVER
	configureAtlas(S_MOUSE_OVER);

	if(input::InputMouse::isMouseDown(input::MB_Left)){
		mFlags |= FLAG_LEFT_PRESSED;
	} else {
		mFlags &= ~FLAG_LEFT_PRESSED;
	}
}

/**
 * Function called when the mouse goes outside from the menu
 */
void MenuButton::mouseOutside()
{
	// configure again the img
	if(mActive){
		// check the actionType
		if(mActionType == ACTION_NORMAL){
			configureAtlas(S_ACTIVE);
		} else if(mActionType == ACTION_SELECTED){
			configureAtlas(S_PRESSED);
		}
	} else {
		configureAtlas(S_INACTIVE);
	}
}

/**
 * Function called when the mouse is moving inside of this menu
 */
void MenuButton::mouseMoving()
{
	if(!mActive){
		return;
	}

	// if is active... check if is clicked
	if(mFlags & FLAG_LEFT_PRESSED){
		if(!input::InputMouse::isMouseDown(input::MB_Left)){
			mFlags &= ~FLAG_LEFT_PRESSED;
			configureAtlas(S_MOUSE_OVER);
		}
	} else {
		if(input::InputMouse::isMouseDown(input::MB_Left)){
			mFlags |= FLAG_LEFT_PRESSED;
			// callback
			leftPressed();
			configureAtlas(S_PRESSED);
		}
	}

	if(mFlags & FLAG_RIGHT_PRESSED){
		if(!input::InputMouse::isMouseDown(input::MB_Right)){
			mFlags &= ~FLAG_RIGHT_PRESSED;
		}
	} else {
		if(input::InputMouse::isMouseDown(input::MB_Right)){
			mFlags |= FLAG_RIGHT_PRESSED;
			// callback
			rightPressed();
		}
	}
}


void MenuButton::rightPressed(void)
{
	debugRED("Warning: This button is not implemented\n");
}

/**
 * When the button is left Pressed
 */
void MenuButton::leftPressed(void)
{
	debugRED("Warning: This button is not implemented\n");
}


