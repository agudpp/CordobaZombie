/*
 * MainMenuState.cpp
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#include "MainMenuState.h"

#include <OgreTimer.h>
#include <OgreWindowEventUtilities.h>
#include <OgreString.h>

#include "StatesFactory.h"
#include "GlobalObjects.h"
#include "InputKeyboard.h"
#include "InputMouse.h"
#include "IMenu.h"

const char *MainMenuState::CONFIG_FILENAME = "MainMenuConfiguration.xml";

////////////////////////////////////////////////////////////////////////////////
MainMenuState::CallbackReceiber::CallbackReceiber(MainMenuState &m) :
mMainMenu(m)
{
}

void MainMenuState::CallbackReceiber::operator()(
		mm_states::IState *s,
		mm_states::Event e
		)
{
	mMainMenu.newStateEvent(s,e);
}



////////////////////////////////////////////////////////////////////////////////
void MainMenuState::newStateEvent(mm_states::IState *state, mm_states::Event e)
{
	ASSERT(state);
	// check the event is from one of the actual states
	if(state != mActualState){
		debugERROR("Event from an bad state:\n"
				"State: %s\n"
				"ActualState: %s\n"
				"Event: %d\n",
				state->name().c_str(),
				(mActualState) ? mActualState->name().c_str() : "Null",
				e);
		ASSERT(false);
		return;
	}

	// is valid, we do the translation
	MainMachineEvent globalEvent = MME_ERROR;
	mm_states::IState *newState = nextState(e, globalEvent);

	// check if we have a global event
	if (newState == 0){
		if (globalEvent != MME_ERROR){
			// new global event TODO
			debugRED("TODO\n");
			ASSERT(false);
		} else {
			// WTF
			debugERROR("We are trying to go to a state that doesn't exists:\n"
					"ActualState: %s\n NewState: %s\n Event: %d\n",
					mActualState->name().c_str(), newState->name().c_str(), e);
			ASSERT(false);
		}
	}

	// we have the next state, configure it
	configureNewState(newState);
}

////////////////////////////////////////////////////////////////////////////////
void MainMenuState::configureNewState(mm_states::IState *newState)
{
	ASSERT(newState);
	if(mActualState == newState) return;

	// load the new state
	newState->load();
	mBeforeUpdateCalled = false;

	// the old state
	mLastState = mActualState;
	if(mLastState) mLastState->unload();

	mActualState = newState;

	// get the video ranges
	mActualState->getVideoRanges(mEnteringRanges);

	ASSERT(mEnteringRanges.size() == 3);

	// TODO Rulo:
	debugRED("TODO Rulo: Configurar aca el video player o lo que sea que tengas"
			" que hacer, el video a reproducir seria mEnteringRanges[0]\n");
}


////////////////////////////////////////////////////////////////////////////////
MainMenuState::VideoState MainMenuState::getVideoState(const VideoRangeVec &range)
{
	// TODO: Rulo, pone aca lo correspondiente
	ASSERT(false);
	return Entering;
}

////////////////////////////////////////////////////////////////////////////////
void MainMenuState::updateStateMachine(void)
{
	// check that we have state to update
	if(!mActualState) return;

	// check video position
	VideoState actualVS = getVideoState(mEnteringRanges);

	if (actualVS == Entering) {
		// we don't have to do nothing, only update videoplayer
		// TODO rulo, actualizar el video player aca
		return;
	}

	// we are in the updating stage
	if (mBeforeUpdateCalled == false) {
		mBeforeUpdateCalled = true;
		mActualState->beforeUpdate();
		return;
	}

	// now update the state
	mActualState->update();

}

////////////////////////////////////////////////////////////////////////////////
void MainMenuState::configureMenuManager(void)
{
	// configure the IMenu
	mMenuManager.build(GLOBAL_WINDOW->getWidth(),GLOBAL_WINDOW->getHeight(),
			5,5);
	IMenu::setMenuManager(&mMenuManager);
}
void MainMenuState::configureOvEffectManager(void)
{
	ASSERT(false);
}
void MainMenuState::configureSoundManager(void)
{
	ASSERT(false);
}

////////////////////////////////////////////////////////////////////////////////
mm_states::IState *MainMenuState::nextState(mm_states::Event e,
		MainMachineEvent &ge)
{
	ge = MainMachineEvent(MME_ERROR);
	if(mActualState == 0) return 0;

	const Ogre::String &stateName = mActualState->name();
	if(stateName == "MainState"){
		switch(e){
		case mm_states::Credits:
			return getStateByName("CreditsState");
		case mm_states::History:
			return getStateByName("HistoryState");
		case mm_states::Config:
			return getStateByName("ConfigState");
		case mm_states::PlayGame:
			debugRED("TODO: aca deberiamos setear ge con el estado correspondiente\n");
			return 0;
		case mm_states::Exit:
			// we are exiting
			debugBLUE("We are exiting the game\n");
			debugRED("TODO: aca deberiamos setear ge con el estado correspondiente\n");
			return 0;
		default:
			debugERROR("Event %d is not supported?\n", e);
			ASSERT(false);
		}

	} else if (stateName == "CreditsState"){
		switch(e) {
		case mm_states::Done:
			return getStateByName("MainState");
		default:
			debugERROR("Event %d is not supported?\n", e);
			return 0;
		}

	} else if (stateName == "ConfigState") {
		switch(e) {
		case mm_states::Done:
			return getStateByName("MainState");
		default:
			debugERROR("Event %d is not supported?\n", e);
			return 0;
		}

	} else if (stateName == "HistoryState") {
		switch(e) {
		case mm_states::Done:
			return getStateByName("MainState");
		default:
			debugERROR("Event %d is not supported?\n", e);
			return 0;
		}
	} else {
		debugERROR("We are in a state that is not actually supported? %s\n",
				mActualState->name().c_str());
		ASSERT(false);
	}
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MainMenuState::MainMenuState() :
IMainState("MainMenuState"),
mActualState(0),
mLastState(0),
mLastEvent(mm_states::Done),
mBeforeUpdateCalled(false)
{

}

////////////////////////////////////////////////////////////////////////////////
MainMenuState::~MainMenuState()
{
	// TODO: Remove all the memory and resources used by this state
	// calling exit()
}

////////////////////////////////////////////////////////////////////////////////
void MainMenuState::enter(const MainMachineInfo &info)
{
	// load all the states
	ASSERT(mActualState == 0);
	ASSERT(mStates.empty());

	if (!mm_states::StatesFactory::buildStates(mStates)){
		debugERROR("Error loading the States\n");
		ASSERT(false);
	}
	ASSERT(!mStates.empty());

	// Load all the other stuff
	try {
		configureMenuManager();
		configureOvEffectManager();
		configureSoundManager();
	} catch (ExceptionInfo &e) {
		debugERROR("Error occurred [%d]: %s\n", e.errCode, e.info.c_str());
		ASSERT(false);
	}

	// configure all the states with the information to be used
	const int size = mStates.size();
	for(int i = 0; i < size; ++i){
		mStates[i]->setFilename(CONFIG_FILENAME);
	}
}

////////////////////////////////////////////////////////////////////////////////
MainMachineEvent MainMenuState::update(MainMachineInfo &info)
{
	// here is the main loop
	// TODO: fix the FrameElapsedTime and check how to get the ogres one.

	float timeStamp = 0;
	Ogre::Timer timer;

	while(true) {
		timeStamp = timer.getMilliseconds();

		// capture input
		input::InputKeyboard::capture();
		input::InputMouse::capture();

		// render the frame
		if(!GLOBAL_ROOT->renderOneFrame()){
			break;
		}

		// update the state machine
		updateStateMachine();

		// This must be called when we use the renderOneFrame approach
		Ogre::WindowEventUtilities::messagePump();

		Common::GlobalObjects::lastTimeFrame = (timer.getMilliseconds() - timeStamp) * 0.001;
	}
}

////////////////////////////////////////////////////////////////////////////////
void MainMenuState::exit(void)
{
	ASSERT(false);
}
