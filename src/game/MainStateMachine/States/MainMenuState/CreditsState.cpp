/*
 * CreditsState.cpp
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#include "CreditsState.h"

#include <OgreOverlayManager.h>
#include <OgreFontManager.h>

#include "DebugUtil.h"
#include "InputKeyboard.h"
#include "GlobalObjects.h"
#include "GUIHelper.h"
#include "Text2D.h"
#include "SoundEnums.h"
#include "SoundManager.h"
#include "SoundFamilyTable.h"



namespace mm_states {


////////////////////////////////////////////////////////////////////////////////
void
CreditsState::triggerHidingState(void)
{

}

////////////////////////////////////////////////////////////////////////////////
void
CreditsState::checkInput(void)
{
    if(input::InputKeyboard::isKeyDown(input::KC_ESCAPE)){
        stateFinish(Event::Done);
    }
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CreditsState::CreditsState() :
		IState("CreditsState"),
		mOverlay(0),
		mTextArea(0),
		mSlideEffect(),
		mRestarter(&mSlideEffect)
{
}

CreditsState::~CreditsState()
{
    if (mOverlay != 0) GUIHelper::fullDestroyOverlay(mOverlay);
}

////////////////////////////////////////////////////////////////////////////////
void CreditsState::operator()(CbMenuButton *, CbMenuButton::ButtonID id)
{
    // this function should be never called
    ASSERT(false);
}

////////////////////////////////////////////////////////////////////////////////
void CreditsState::load(void)
{
    // check if we had already loaded the overlay (is cached)
    if (mOverlay != 0 ) return;
	//
	debugWARNING("UGLY workaround to load all the fonts first.. Move this to the"
			" MainMenu loading lines (where we will load all the resources and "
			"every else\n");
	Ogre::ResourceManager::ResourceMapIterator iter =
			Ogre::FontManager::getSingleton().getResourceIterator();
	while (iter.hasMoreElements()) { iter.getNext()->load(); }

	// Load all the overlays
	if(!mOverlay){
		Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();
		mOverlay = om.getByName("MainMenu/Credits");
		ASSERT(mOverlay);
		mTextArea = reinterpret_cast<Ogre::TextAreaOverlayElement *>(
				mOverlay->getChild("Credits/Text"));
		mTextArea->setCaption("Esto es una prueba\nMas que una prueba es una copadasa\npepe\npap\nperas\n:)");
		mTextArea->setHorizontalAlignment(Ogre::GHA_CENTER);
		mTextArea->setVerticalAlignment(Ogre::GVA_CENTER);
		ASSERT(mTextArea);
		mOverlay->hide();

		// configure the SlideEffect
		mSlideEffect.setElement(mTextArea);
		mBeginPos.x = mTextArea->getLeft();
		mBeginPos.y = mTextArea->getTop();

		// Finally load the sounds for this state from the config.xml file.
		getSoundsFromXML();
	}

	// configure the text of the text area
	// load the text from the xml
	const TiXmlElement *root = getXmlElement();
	if (root == 0) {
		debugERROR("We cannot get the root element to this State\n");
		return;
	}
	// get the string
	const TiXmlElement *credits = root->FirstChildElement("Credits");
	if (credits == 0){
	    debugERROR("Invalid XML, it haven't the section Credits\n");
	    return;
	}
	const char *creditsStr = credits->Attribute("text");
	if (creditsStr == 0) {
	    debugERROR("No credits information \"Text\" could be obtained..?\n");
	    return;
	}
	Ogre::String str = creditsStr;
	debugERROR("Uncomment the following lines\n");
//	gui_utils::Text2D textConfigurator(mTextArea);
//	textConfigurator.configure(str,
//	        gui_utils::Text2D::ADJUST_TEXT_TO_CONTAINER_H);

	// configure the translation position
	Ogre::Vector2 endPos;
	endPos.x = mBeginPos.x;
	endPos.y = -mBeginPos.y - mTextArea->getHeight();
	mSlideEffect.setTranslationPositions(mBeginPos, endPos);

	// set the time
	float timeDuration;
	if (credits->Attribute("timeDuration") == 0){
	    debugWARNING("No time duration was set for the credits, we will set 25"
	            " as default!!\n");
	    timeDuration = 25.0f;
	} else {
	    timeDuration = Ogre::StringConverter::parseReal(
	            credits->Attribute("timeDuration"));
	}
	mSlideEffect.setDuration(timeDuration);

	// configure the velocity here, is linear
	mSlideEffect.setFunction(OvEff::Slide::LINEAL_FUNCTION);
}

////////////////////////////////////////////////////////////////////////////////
void CreditsState::beforeUpdate(void)
{
	// configure the overlay to be shown the top of the text
    mTextArea->setPosition(mBeginPos.x, mBeginPos.y);

    // start slide effect
    mSlideEffect.start();

    ASSERT(mOverlay);
    mOverlay->show();

    mRestarter.link();

	// Start the background music in looping mode.
	mm::SSerror err = mm::SoundManager::getInstance().playEnvSound(
			*mSounds.getSound(SS_BACKGROUND_MUSIC),	// Music filename
			BACKGROUND_MUSIC_VOLUME,				// Playback volume
			true);									// Looping activated
	ASSERT(err == mm::SSerror::SS_NO_ERROR);
}

////////////////////////////////////////////////////////////////////////////////
void CreditsState::update(void)
{
    checkInput();
	return ;
}

////////////////////////////////////////////////////////////////////////////////
void CreditsState::unload(void)
{
    mOverlay->hide();
    mRestarter.unlink();

    // Stop the background music.
    mm::SSerror err = mm::SoundManager::getInstance().stopEnvSound(
			*mSounds.getSound(SS_BACKGROUND_MUSIC));
	ASSERT(err == mm::SSerror::SS_NO_ERROR);
}


}
