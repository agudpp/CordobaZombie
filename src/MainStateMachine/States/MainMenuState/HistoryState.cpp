/*
 * HistoryState.cpp
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#include "HistoryState.h"
#include "DebugUtil.h"
#include <sstream>


namespace mm_states {



HistoryState::HistoryState():
		IState("HistoryState"),
		mState(0),
		mSlidePlayer(0)
{
	mSlidePlayer = new SlidePlayer(
						Ogre::String("HistorySlidePlayerOverlay"),
						Ogre::String("MainStates/MainMenuState/"
						"HistoryState/SlidePlayerOverlayEffects.xml"));

	ASSERT(mSlidePlayer);

	mBtnNames.push_back(Ogre::String("Back"));
	mBtnNames.push_back(Ogre::String("Prev"));
	mBtnNames.push_back(Ogre::String("Next"));

}

////////////////////////////////////////////////////////////////////////////////


HistoryState::~HistoryState() {
}


////////////////////////////////////////////////////////////////////////////////


void HistoryState::operator()(CbMenuButton * b, CbMenuButton::ButtonID id)
{
	if(b == mButtons[Back].getButton() && id == CbMenuButton::LEFT_BUTTON)
	{
		int size = mButtons.size();
		for(int i = 0; i < size; i++){
			mButtons[i].getEffect()->start();
		}
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

	// we will load all the buttons
	buildButtons(mButtons, mBtnNames);
	ASSERT(mButtons.size() == mBtnNames.size());

	int size = mButtons.size();
	for(int i = 0; i < size; i++){
		mButtons[i].getEffect()->addCallback(this);
	}

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

	mState = STATE_SHOWING;
}



////////////////////////////////////////////////////////////////////////////////

void HistoryState::beforeUpdate(void)
{
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
	//Hide SlidePlayer
	//mSlidePlayer->hide();
	const int size = mButtons.size();

	for(int i = 0; i < size; ++i){
		delete mButtons[i].getEffect();
		delete mButtons[i].getButton();
	}

	delete mSlidePlayer;

}

void HistoryState::operator()(EventID id){
	// Buttons have finished hiding, send finish event to the MainMenuState
	if(id == ENDING && mState != STATE_EXITING){
		mState = STATE_EXITING;
		stateFinish(Event::Done);
	}
}

} //namespace mm_state



