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



namespace mm_states {


////////////////////////////////////////////////////////////////////////////////
void CreditsState::triggerHidingState(void)
{

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
	Ogre::String str = credits->GetText();
	gui_utils::Text2D textConfigurator(mTextArea);

	textConfigurator.configure(str,
	        gui_utils::Text2D::ADJUST_TEXT_TO_CONTAINER_H);

	// configure the translation position
	Ogre::Vector2 endPos;
	endPos.x = mBeginPos.x;
	endPos.y = -mBeginPos.y - mTextArea->getHeight();
	mSlideEffect.setTranslationPositions(mBeginPos, endPos);

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

    // TODO: show credits
    ASSERT(mOverlay);
    mOverlay->show();
}

////////////////////////////////////////////////////////////////////////////////
void CreditsState::update(void)
{
	//TODO: I think that we have nothing to do here, the effect will be updated
    // by the OverlayManager in the MainState so...

	return ;
}

////////////////////////////////////////////////////////////////////////////////
void CreditsState::unload(void)
{
	// TODO: destroy the overlay and else
    GUIHelper::fullDestroyOverlay(mOverlay);
    mOverlay = 0;

}

////////////////////////////////////////////////////////////////////////////////
void CreditsState::keyPressed(input::KeyCode key)
{
	if(key == input::KC_ESCAPE){
	    stateFinish(Event::Done);
	}
}

}
