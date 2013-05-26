/*
 * HistoryState.cpp
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#include <sstream>
#include <boost/bind.hpp>

#include "HistoryState.h"
#include "DebugUtil.h"
#include "SoundEnums.h"
#include "SoundManager.h"
#include "SoundFamilyTable.h"



namespace mm_states {


const char *HistoryState::BUTTONS_NAMES[NUMBER_BUTTONS] =  {
        "ExitButton",
        "PrevButton",
        "NextButton",
};


HistoryState::HistoryState():
		IState("HistoryState"),
		mState(0),
		mSlidePlayer(0)
{
}

////////////////////////////////////////////////////////////////////////////////

HistoryState::~HistoryState()
{
	this->unload();

	// remove buttons
	const int size = mButtons.size();
	for(int i = 0; i < size; ++i){
		ASSERT(mButtons[i].getEffect());
		delete mButtons[i].getEffect();
		delete mButtons[i].getButton();
	}
	mButtons.clear();

	// hide slide player
	mSlidePlayer->hide();
}


////////////////////////////////////////////////////////////////////////////////
/*
 * En load entre otras cosas cargamos las slides. La secuencia de slides debe
 * ser definida en un archivo '.materials' y debe estar cargado en Ogre.
 * Los nombres de cada slide deben ser de la forma
 * "HistorySlidePlayerMaterials/#" con # un entero que arranca en 1 para la
 * primera slide y se va incrementando en uno para cada slide que sigue
 * marcando asi el numero de slide en la secuencia.
 */


void HistoryState::load(void)
{

	// we will load all the buttons
	if(mButtons.empty()){

		std::vector< Ogre::String > btnNames;
		btnNames.reserve(NUMBER_BUTTONS);
		for(int i = 0; i < NUMBER_BUTTONS; i++){
			btnNames.push_back(Ogre::String(BUTTONS_NAMES[i]));
		}
		this->buildButtons(mButtons, btnNames);

		ASSERT(mButtons.size() == btnNames.size());

		// set callbacks
		for(size_t i = 0; i < NUMBER_BUTTONS; ++i){
			mButtons[i].getEffect()->addCallback(
			        boost::bind(&HistoryState::operator(), this, _1));
		}

		// Finally load the sounds for this state from the config.xml file.
		getSoundsFromXML();
	}

	if(!mSlidePlayer){

		mSlidePlayer = new f_e::SlidePlayer(
							Ogre::String("HistorySlidePlayerOverlay"),
							Ogre::String("MainStates/MainMenuState/"
							"HistoryState/SlidePlayerOverlayEffects.xml"));

		ASSERT(mSlidePlayer);

		// Load the slides
		Ogre::MaterialManager& materialman =
				Ogre::MaterialManager::getSingleton();
		const std::string MatName = "HistorySlidePlayerMaterials/";
		int i = 1;
		while(1){
			std::stringstream strm;
			strm << MatName << i;
			if(!materialman.resourceExists(strm.str().c_str())){
				break;
			}
			debugGREEN("loading slide %s\n",strm.str().c_str());
			mSlidePlayer->queueSlide(Ogre::String(strm.str().c_str()));
			i++;
		}
	}

	// Enable buttons
	for(size_t i = 0, size = mButtons.size(); i < size; ++i){
		mButtons[i].getButton()->setEnable(true);
	}

}


////////////////////////////////////////////////////////////////////////////////

void HistoryState::beforeUpdate(void)
{

	mState = STATE_SHOWING;

    // show the overlay
    Ogre::Overlay *overlay = Ogre::OverlayManager::getSingleton().getByName(
            "MainMenu/HistoryState");
    ASSERT(overlay);
    overlay->show();

	// reproduce all the effects in all the buttons
	for(size_t i = 0, size = mButtons.size(); i < size; ++i) {
		ASSERT(mButtons[i].getEffect());
		mButtons[i].getEffect()->start();
		mButtons[i].getButton()->getContainer()->show();
		mButtons[i].getEffect()->getElement()->show();
	}

	ASSERT(mSlidePlayer);
	mSlidePlayer->show();

	// Start the background music in looping mode.
	mm::SSerror err = mm::SoundManager::getInstance().playEnvSound(
			*mSounds.getSound(SS_BACKGROUND_MUSIC),	// Music filename
			BACKGROUND_MUSIC_VOLUME,				// Playback volume
			true);									// Looping activated
	ASSERT(err == mm::SSerror::SS_NO_ERROR);
}

////////////////////////////////////////////////////////////////////////////////

void HistoryState::update(void)
{

	this->checkInput();

	mSlidePlayer->update();

	if(mState != STATE_EXITING){
		this->checkInput();
	}else{
		for(size_t i = 0, size = mButtons.size(); i < size; i++){
			if (mButtons[i].getEffect()->isActive()) {
				return;  // Can't quit yet
			} else {
				mButtons[i].getButton()->setEnable(false);
			}
		}
		mSlidePlayer->hide();
		stateFinish(mm_states::Event::Done);
	}

	return;
}


////////////////////////////////////////////////////////////////////////////////
void HistoryState::unload(void)
{
	//Hide SlidePlayer
	mSlidePlayer->hide();

	const int size = mButtons.size();
	for(size_t i = 0, size = mButtons.size(); i < size; ++i){
		// Restore effects
		mButtons[i].getEffect()->complement();
		mButtons[i].getButton()->resetAtlas();
	}

    // Stop the background music.
	mm::SSerror err = mm::SoundManager::getInstance().stopEnvSound(
			*mSounds.getSound(SS_BACKGROUND_MUSIC));
	ASSERT(err == mm::SSerror::SS_NO_ERROR);
}



////////////////////////////////////////////////////////////////////////////////

void HistoryState::operator()(CbMenuButton * b, CbMenuButton::ButtonID id)
{
    mm::SSerror err(mm::SSerror::SS_NO_ERROR);
    mm::SoundManager& sMgr(mm::SoundManager::getInstance());

    if (mState == STATE_HIDING || mState == STATE_EXITING) {
    	// Someone already pressed an escape button, ignore following commands.
    	return;
    }

	if(b == mButtons[mm_states::HistoryState::exitButton].getButton()
			&& id == CbMenuButton::LEFT_BUTTON)
	{
		sMgr.stopEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));
		err = sMgr.playEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));
		ASSERT(err == mm::SSerror::SS_NO_ERROR);
		mState = STATE_HIDING;
		this->hideToExit();
	}
	else if(b == mButtons[mm_states::HistoryState::prevButton].getButton()
			&& id == CbMenuButton::LEFT_BUTTON)
	{
		sMgr.stopEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));
		err = sMgr.playEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));
		ASSERT(err == mm::SSerror::SS_NO_ERROR);
		mSlidePlayer->prev();
	}
	else if(b == mButtons[mm_states::HistoryState::nextButton].getButton()
			&& id == CbMenuButton::LEFT_BUTTON)
	{
		sMgr.stopEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));
		err = sMgr.playEnvSound(*mSounds.getSound(SS_MOUSE_CLICK));
		ASSERT(err == mm::SSerror::SS_NO_ERROR);
		mSlidePlayer->next();
	}
	else
	{
		debugERROR("Invalid button has been pressed :S\n");
		ASSERT(false);
	}
}



////////////////////////////////////////////////////////////////////////////////

void HistoryState::operator()(f_e::OverlayEffect::EventID id)
{
	// Buttons have finished hiding, send finish event to the MainMenuState
	if(id == f_e::OverlayEffect::ENDING && mState == STATE_HIDING){
		mState = STATE_EXITING;
	}
}


////////////////////////////////////////////////////////////////////////////////

void HistoryState::hideToExit(void)
{
	const size_t size = mButtons.size();

	// complement effects and start playing them
	for(size_t i = 0; i < size; i++){
		ASSERT(mButtons[i].getEffect());
		mButtons[i].getEffect()->complement();
		mButtons[i].getEffect()->start();
	}

}


////////////////////////////////////////////////////////////////////////////////

void HistoryState::checkInput(void)
{
    if(input::InputKeyboard::isKeyDown(input::KC_ESCAPE)){
    	if(mState == STATE_SHOWING){
    		mState = STATE_HIDING;
    		this->hideToExit();
    	}
    }
}




} //namespace mm_state



