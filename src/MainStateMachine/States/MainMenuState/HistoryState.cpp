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
	const int size = mButtons.size();
	for(int i = 0; i < size; ++i){
		delete mButtons[i].getEffect();
		delete mButtons[i].getButton();
	}

	delete mSlidePlayer;
}


////////////////////////////////////////////////////////////////////////////////


void HistoryState::operator()(CbMenuButton * b, CbMenuButton::ButtonID id)
{
	if(b == mButtons[Back].getButton() && id == CbMenuButton::LEFT_BUTTON){
		mState = STATE_HIDING;
	}else if(b == mButtons[Prev].getButton() && id == CbMenuButton::LEFT_BUTTON){
		mSlidePlayer->prev();
	}else if(b == mButtons[Next].getButton() && id == CbMenuButton::LEFT_BUTTON){
		mSlidePlayer->next();
	}else{
		ASSERT(false);
	}

}


////////////////////////////////////////////////////////////////////////////////


void HistoryState::load(void){

	// we will load all the buttons
	buildButtons(mButtons, mBtnNames);
	ASSERT(mButtons.size() == mBtnNames.size());

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

}

////////////////////////////////////////////////////////////////////////////////

void HistoryState::update(void)
{
	switch(mState){
	case STATE_SHOWING:
	{
		// reproduce all the effects in all the buttons
		const int size = mButtons.size();
		debugWARNING("Descomentar la linea de abajo cuando tengamos los efectos\n");
//			for(int i = 0; i < size; ++i) {mButtons[i].getEffect()->start();}

		// TODO: reproduce the sounds (fade in) here
		// TODO: show credits
		ASSERT(mSlidePlayer);
		mSlidePlayer->show();
		++mState;
	}
	break;
	case STATE_LOOP:
	{
		//Dont do nothing here
	}
	break;
	case STATE_HIDING:
	{
		// hiding... do nothing (wait  the effect callback)
	}
	break;
	case STATE_EXITING:
	{
		// exiting
		stateFinish(Done);
	}
	break;
	default:
		ASSERT(false);

	}

	return ;
}


}
