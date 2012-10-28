/*
 * HistoryState.cpp
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#include <boost/bind.hpp>

#include "HistoryState.h"
#include "DebugUtil.h"
#include <sstream>


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

HistoryState::~HistoryState() {

	debugRAUL("=#=#=#=#=#=#=#=#=#=#=#= Destroying History State\n")

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

void HistoryState::load(void){

	debugRAUL("=#=#=#=#=#=#=#=#=#=#=#= LOADING History State\n");

	// we will load all the buttons
	if(mButtons.empty()){

		std::vector< Ogre::String > btnNames;
		btnNames.reserve(NUMBER_BUTTONS);
		for(int i = 0; i < NUMBER_BUTTONS; i++){
			btnNames.push_back(Ogre::String(BUTTONS_NAMES[i]));
		}
		this->buildButtons(mButtons, btnNames);

		ASSERT(mButtons.size() == btnNames.size());

		debugRAUL("=#=#=#=#=#=#=#=#=#=#=#= Built %d buttons\n", (int)mButtons.size());

		// set callbacks
		for(size_t i = 0; i < NUMBER_BUTTONS; ++i){
			mButtons[i].getEffect()->addCallback(
			        boost::bind(&HistoryState::operator(), this, _1));
		}

	}

	if(!mSlidePlayer){

		mSlidePlayer = new SlidePlayer(
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
	debugRAUL("=#=#=#=#=#=#=#=#=#=#=#= BEFORE UPDATE History State\n");

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

}

////////////////////////////////////////////////////////////////////////////////

void HistoryState::update(void)
{
//	debugRAUL("=#=#=#=#=#=#=#=#=#=#=#= UPDATE History State\n");

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
		debugRAUL("=#=#=#=#=#=#=#=#=#=#=#= Going OUT\n");
		stateFinish(mm_states::Event::Done);
	}

	return;
}


////////////////////////////////////////////////////////////////////////////////
void HistoryState::unload(void)
{
	debugRAUL("=#=#=#=#=#=#=#=#=#=#=#= UNLOAD History State\n");

	//Hide SlidePlayer
	mSlidePlayer->hide();

	const int size = mButtons.size();
	for(size_t i = 0, size = mButtons.size(); i < size; ++i){
		// Restore effects
		mButtons[i].getEffect()->complement();
		mButtons[i].getButton()->resetAtlas();
	}

}



////////////////////////////////////////////////////////////////////////////////

void HistoryState::operator()(CbMenuButton * b, CbMenuButton::ButtonID id)
{
	if(b == mButtons[mm_states::HistoryState::exitButton].getButton()
			&& id == CbMenuButton::LEFT_BUTTON)
	{
		debugRAUL("=#=#=#=#=#=#=#=#=#=#=#= Pressed escape button\n");
		mState = STATE_HIDDING;
		this->hideToExit();
	}
	else if(b == mButtons[mm_states::HistoryState::prevButton].getButton()
			&& id == CbMenuButton::LEFT_BUTTON)
	{
		mSlidePlayer->prev();
	}
	else if(b == mButtons[mm_states::HistoryState::nextButton].getButton()
			&& id == CbMenuButton::LEFT_BUTTON)
	{
		mSlidePlayer->next();
	}
	else
	{
		debugERROR("Invalid button has been pressed :S\n");
		ASSERT(false);
	}

}



////////////////////////////////////////////////////////////////////////////////

void HistoryState::operator()(OvEff::OverlayEffect::EventID id)
{
	// Buttons have finished hiding, send finish event to the MainMenuState
	if(id == OvEff::OverlayEffect::ENDING && mState == STATE_HIDDING){

		mState = STATE_EXITING;

		debugRAUL("=#=#=#=#=#=#=#=#=#=#=#= Got call back from button,"
				" Changed state to exiting\n");

	}
}


////////////////////////////////////////////////////////////////////////////////

void HistoryState::hideToExit(void){

	debugRAUL("=#=#=#=#=#=#=#=#=#=#=#= Hiding to exit\n");

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
    		debugRAUL("=#=#=#=#=#=#=#=#=#=#=#= Read KC_ESCAPE from Keyboard\n");
    		mState = STATE_HIDDING;
    		this->hideToExit();
    	}
    }
}




} //namespace mm_state



