#ifndef __MOUSECURSOR_H__
#define __MOUSECURSOR_H__

#include <OgrePanelOverlayElement.h>
#include <OgreMaterialManager.h>
#include <OgreString.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>

#include "DebugUtil.h"


#define MOUSE_CURSOR_MATERIAL_NAME		"MouseCursors"
#define MOUSE_CURSOR_IMAGE_NAME			"mouseCursors.png"
#define MOUSE_RELATIVE_WIDTH			0.05f
#define MOUSE_RELATIVE_HEIGHT			0.05f


class MouseCursor
{
private:
	Ogre::Overlay* mGuiOverlay;
	Ogre::OverlayContainer* mCursorContainer;
	Ogre::TexturePtr mTexture;
	Ogre::MaterialPtr mMaterial;

	Ogre::Real mWindowWidth;
	Ogre::Real mWindowHeight;
	Ogre::Real mXRelPos;
	Ogre::Real mYRelPos;

	Ogre::Real	mAtlasSize;
	int			mTextureSize;

public:
	enum Cursor {
		NORMAL_CURSOR = 0,
		PICK_OBJECT_CURSOR,
		ATTACK_CURSOR,
		MOVE_CRUSOR,

		NUM_CURSORS,
	};

public:
    inline MouseCursor();

    inline virtual ~MouseCursor(void);

    inline void setCursor(Cursor c);

    inline void setWindowDimensions(unsigned int width, unsigned int height);

    inline void setVisible(bool visible);

    inline void updatePosition(int x, int y);

    inline Ogre::Real getXRelativePos(void) const {return mXRelPos;}

    inline Ogre::Real getYRelativePos(void) const {return mYRelPos;}

    inline Ogre::Real clamp(Ogre::Real a, Ogre::Real min, Ogre::Real max);

private:
   inline void
   setImage(const Ogre::String& filename, const Ogre::String &group = "General");
};



/******************************************************************************/
/****************************     INLINES     *********************************/


////////////////////////////////////////////////////////////////////////////////
inline
MouseCursor::MouseCursor() :
	mGuiOverlay(0),
	mCursorContainer(0)
{
    mMaterial = Ogre::MaterialManager::getSingleton().create(
    		MOUSE_CURSOR_MATERIAL_NAME,	"Popular");
    ASSERT(!mMaterial.isNull());
    mCursorContainer = (Ogre::OverlayContainer*)
    		Ogre::OverlayManager::getSingletonPtr()->createOverlayElement(
    				"Panel", "MouseCursor");
    mCursorContainer->setMaterialName(mMaterial->getName());
    mCursorContainer->setPosition(0, 0);
    mGuiOverlay = Ogre::OverlayManager::getSingletonPtr()->create(
    		"MouseCursor");
    mGuiOverlay->setZOrder(649);
    mGuiOverlay->add2D(mCursorContainer);
    mGuiOverlay->show();
    mCursorContainer->setMetricsMode(Ogre::GMM_RELATIVE);
    mCursorContainer->setDimensions(MOUSE_RELATIVE_WIDTH,
		MOUSE_RELATIVE_HEIGHT);

    // get the factor to multiply
    Ogre::TexturePtr text = Ogre::TextureManager::getSingleton().getByName(
    		MOUSE_CURSOR_IMAGE_NAME);
	ASSERT(!text.isNull());
	mTextureSize = text->getWidth();
	mAtlasSize = mTextureSize / NUM_CURSORS;

	setImage(MOUSE_CURSOR_IMAGE_NAME);

	// start using normal cursor
	setCursor(NORMAL_CURSOR);
}


////////////////////////////////////////////////////////////////////////////////
inline
MouseCursor::~MouseCursor(void)
{
	Ogre::MaterialManager::getSingleton().unload(mMaterial->getName());
	if(mCursorContainer){
		Ogre::OverlayManager::getSingleton().destroyOverlayElement(
				mCursorContainer);
	}
	if(mGuiOverlay){
		Ogre::OverlayManager::getSingleton().destroy(mGuiOverlay);
	}
	if(mTexture->isLoaded()){
		Ogre::TextureManager::getSingleton().unload(mTexture->getName());
	}
}


////////////////////////////////////////////////////////////////////////////////
inline void
MouseCursor::setCursor(Cursor c)
{
	ASSERT(c < NUM_CURSORS);
	// get the %
	Ogre::Real x1,x2;
	x1 = (mAtlasSize * c) / static_cast<float>(mTextureSize);
	x2 = static_cast<float>(mAtlasSize * (c+1)) / static_cast<float>(mTextureSize);
	static_cast<Ogre::PanelOverlayElement *>(mCursorContainer)->setUV(
			x1, 0.0f, x2, 1.0f);
}


////////////////////////////////////////////////////////////////////////////////
inline void
MouseCursor::setWindowDimensions(unsigned int width, unsigned int height)
{
    mWindowWidth = (width > 0) ? width : 1;
    mWindowHeight = (height > 0) ? height : 1;

    mWindowHeight = 1.0f/mWindowHeight;
    mWindowWidth = 1.0f/mWindowWidth;

//        mCursorContainer->setWidth(mTexture->getWidth() * mWindowWidth);
//        mCursorContainer->setHeight(mTexture->getHeight() * mWindowHeight);
}


////////////////////////////////////////////////////////////////////////////////
inline void
MouseCursor::setVisible(bool visible)
{
    if(visible) {
        mCursorContainer->show();
    } else {
        mCursorContainer->hide();
    }
}


////////////////////////////////////////////////////////////////////////////////
inline void
MouseCursor::updatePosition(int x, int y)
{
    mXRelPos = x * mWindowWidth;
    mYRelPos = y * mWindowHeight;
    mCursorContainer->setPosition(clamp(mXRelPos, 0.0f, 1.0f),
    		clamp(mYRelPos, 0.0f, 1.0f));
}


////////////////////////////////////////////////////////////////////////////////
inline Ogre::Real
MouseCursor::clamp(Ogre::Real a, Ogre::Real min, Ogre::Real max)
{
    if (a < min) {
        return min;
    }
    if (a > max) {
        return max;
    }
    return a;
}


////////////////////////////////////////////////////////////////////////////////
inline void
MouseCursor::setImage(const Ogre::String& filename, const Ogre::String& group)
{
	   mTexture = Ogre::TextureManager::getSingleton().load(filename, group);
	   ASSERT(!mTexture.isNull());

	   Ogre::TextureUnitState *pTexState;
	   if(mMaterial->getTechnique(0)->getPass(0)->getNumTextureUnitStates())
	   {
		   pTexState = mMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	   }
	   else
	   {
		   pTexState = mMaterial->getTechnique(0)->getPass(0)->createTextureUnitState( mTexture->getName() );
	   }
	   pTexState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	   mMaterial->getTechnique(0)->getPass(0)->setSceneBlending(
			Ogre::SBT_TRANSPARENT_ALPHA);
}


#endif  // __MOUSECURSOR_H__
