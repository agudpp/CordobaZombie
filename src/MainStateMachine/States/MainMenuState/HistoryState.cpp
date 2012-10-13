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



HistoryState::HistoryState():
		IState("HistoryState"),
		mState(0),
		mSlidePlayer(0)
{
}

////////////////////////////////////////////////////////////////////////////////

HistoryState::~HistoryState() {
	const int size = mButtons.size();

	for(int i = 0; i < size; ++i){
		delete mButtons[i].getEffect();
		delete mButtons[i].getButton();
	}

	delete mSlidePlayer;
}


////////////////////////////////////////////////////////////////////////////////

void HistoryState::hideToExit(void){
	int size = mButtons.size();
	if(size == 0){
		mState = STATE_EXITING;
		stateFinish(Event::Done);
	}else{
		for(int i = 0; i < size; i++){
			mButtons[i].getEffect()->complement();
			mButtons[i].getEffect()->start();
		}
	}
}


////////////////////////////////////////////////////////////////////////////////

void HistoryState::checkInput(void)
{
    if(input::InputKeyboard::isKeyDown(input::KC_ESCAPE)){
        this->hideToExit();
    }
}


////////////////////////////////////////////////////////////////////////////////

void HistoryState::operator()(CbMenuButton * b, CbMenuButton::ButtonID id)
{
	if(b == mButtons[Back].getButton() && id == CbMenuButton::LEFT_BUTTON)
	{
		this->hideToExit();
	}
	else if(b == mButtons[Prev].getButton() && id == CbMenuButton::LEFT_BUTTON)
	{
		mSlidePlayer->prev();
	}
	else if(b == mButtons[Next].getButton() && id == CbMenuButton::LEFT_BUTTON)
	{
		mSlidePlayer->next();
	}
	else
	{
		ASSERT(false);
	}

}


////////////////////////////////////////////////////////////////////////////////
/*
 * En load entre otras cosas cargamos las slides. La secuencia de slides debe
 * ser definida en un archivo '.materials' y debe estar cargado en Ogre.
 * Los nombres de cada slide deben ser de la forma
 * "HistorySlidePlayerMaterials/#" con # un entero que arranca en 1 para la
 * primera slide y se va incrementando en uno para cada slide que sigue
 * marcando el asi el numero de slide en la secuencia.
 */

void HistoryState::load(void){

	debugRAUL("LOADING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n");

	// we will load all the buttons
	if(mBtnNames.size() == 0){

		mBtnNames.push_back(Ogre::String("backButton"));
		mBtnNames.push_back(Ogre::String("prevButton"));
		mBtnNames.push_back(Ogre::String("nextButton"));

		buildButtons(mButtons, mBtnNames);

		ASSERT(mButtons.size() == mBtnNames.size());

		int size = mButtons.size();
		for(int i = 0; i < size; i++){
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
		Ogre::MaterialManager& materialman = Ogre::MaterialManager::getSingleton();
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

	mState = STATE_SHOWING;
}



////////////////////////////////////////////////////////////////////////////////

void HistoryState::beforeUpdate(void)
{

	debugRAUL("BEFORE UPDATE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n");

	// reproduce all the effects in all the buttons
	const int size = mButtons.size();
	for(int i = 0; i < size; ++i) {mButtons[i].getEffect()->start();}

	ASSERT(mSlidePlayer);
	mSlidePlayer->show();
}

////////////////////////////////////////////////////////////////////////////////

void HistoryState::update(void)
{
	mSlidePlayer->update();
	return;
}


////////////////////////////////////////////////////////////////////////////////
void HistoryState::unload(void)
{
	debugRAUL("UNLOAD !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n");

	//Hide SlidePlayer
	mSlidePlayer->hide();

	const int size = mButtons.size();
	for(int i = 0; i < size; ++i){
		// Restore effects
		mButtons[i].getEffect()->complement();
	}

}

void HistoryState::operator()(OvEff::OverlayEffect::EventID id)
{
	// Buttons have finished hiding, send finish event to the MainMenuState
	if(id == OvEff::OverlayEffect::ENDING && mState != STATE_EXITING){
		mState = STATE_EXITING;
		stateFinish(mm_states::Done);
	}
}

} //namespace mm_state



