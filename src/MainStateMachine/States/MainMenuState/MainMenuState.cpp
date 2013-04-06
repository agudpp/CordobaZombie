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
#include "Util.h"
#include "MouseCursor.h"
#include "SoundManager.h"
#include "BufferBuilder.h"

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
					mActualState->name().c_str(), "NULL", e);
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

	// Load entering ranges to video api and play
	mVideoPlayerAPI->load(0,mEnteringRanges[0].start, mEnteringRanges[0].end);
	mVideoPlayerAPI->setRepeat(false);
	mVideoPlayerAPI->play();
	mVideoPlayerAPI->setVisible(true);
}


////////////////////////////////////////////////////////////////////////////////
MainMenuState::VideoState MainMenuState::getVideoState(void)
{
	ASSERT(mEnteringRanges.size() > 0);

	float actualpos = mVideoPlayerAPI->getVideoTime();

	if(actualpos >= mEnteringRanges[0].start
			&& actualpos < mEnteringRanges[0].end){

		return Entering;
	}else{
		return Updating;
	}
}

////////////////////////////////////////////////////////////////////////////////
void MainMenuState::updateStateMachine(void)
{
	static SoundManager& sMgr(SoundManager::getInstance());

	// check that we have state to update
	if(!mActualState) return;

	// check video position
	VideoState actualVS = getVideoState();

	if (actualVS == Entering) {
		// All we had to do was to update the video player.
		return;
	}

	// We are in the refresh stage
	if (mBeforeUpdateCalled == false) {
		mBeforeUpdateCalled = true;
		mActualState->beforeUpdate();

		mVideoPlayerAPI->load( 0
				             , mEnteringRanges[1].start
						     , mEnteringRanges[1].end
						     );
		mVideoPlayerAPI->setRepeat(true);
		mVideoPlayerAPI->setVisible(true);
		mVideoPlayerAPI->play();
		return;
	}

	// Finally update the state ...
	mActualState->update();
	// ... and the SoundManager.
	sMgr.update();
}

////////////////////////////////////////////////////////////////////////////////
void MainMenuState::configureOvEffectManager(void)
{
	OvEff::OverlayEffect::setManager(&mOvEffManager);
}

////////////////////////////////////////////////////////////////////////////////
void MainMenuState::configureSoundManager(void)
{
	std::vector<const TiXmlElement *> states;
	SoundManager& sMgr = SoundManager::getInstance();

	// Gather the sound names for the menu states from the config.xml file.
	mXmlHelper.getFirstElements(states);
	for (uint i=0 ; i < states.size() ; ++i) {

		// For each main tag, search for "Sounds" entry.
		const TiXmlElement* sounds = states[i]->FirstChildElement("Sounds");
		if(!sounds) {
			debugWARNING("No sounds specified in the file \"%s\" for XML tag # %u\n",
					mXmlHelper.getFilename().c_str(), i);
			continue;
		} else {
			sounds = sounds->FirstChildElement("Sound");
		}

		while(sounds) {
			// Register currently pointed sound filename.
			Ogre::String sound = sounds->Attribute("filename");
			ASSERT(sound.size());
			mSoundsFilenames.insert(sound);
			// Get next sound filename.
			sounds = sounds->NextSiblingElement("Sound");
		}
	}

	// Load all found sound files.
	for (std::set<Ogre::String>::const_iterator sound = mSoundsFilenames.begin() ;
			sound != mSoundsFilenames.end() ;
			sound++) {

		Ogre::String soundFilePath("");
		Common::Util::getResourcePath(Ogre::String(SOUNDS_RESOURCE_GROUP_NAME),
				*sound, soundFilePath);
		if (!soundFilePath.size()) {
			debugWARNING("No resource found to map sound \"%s\"\n",	sound->c_str());
			continue;
		}

		SSbuftype buffType = BufferBuilder::bestBufferType(soundFilePath);
		SSerror err = sMgr.loadSound(*sound, SSformat::SS_NOTHING, buffType);
		ASSERT(err == SSerror::SS_NO_ERROR);
	}

	// Load some sources into the SoundManager to play these sounds.
	sMgr.addSSoundSources(4);
	sMgr.addLSoundSources(4);
}

////////////////////////////////////////////////////////////////////////////////
void MainMenuState::configureVideoAPI(void)
{
	// Construct video player api
	if(mVideoPlayerAPI == 0){
		mVideoPlayerAPI = new VideoPlayerAPI;
	}
	ASSERT(mVideoPlayerAPI);

	// Load menu video
	Ogre::String videoPath;
	Common::Util::getResourcePath( Ogre::String(VIDEOS_RESOURCE_GROUP),
	Ogre::String(mXmlHelper.findElement("MenuVideo")->Attribute("videoName")),
	videoPath);
	mVideoPlayerAPI->load(videoPath.c_str(),0,0);
	mVideoPlayerAPI->setVisible(true);
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
			break;
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

	return 0;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MainMenuState::MainMenuState() :
	IMainState("MainMenuState"),
	mActualState(0),
	mLastState(0),
	mLastEvent(mm_states::Done),
	mBeforeUpdateCalled(false),
	mVideoPlayerAPI(0),
	mCbReceiver(*this),
	mSoundsFilenames()
{
    // configure the callback for the IStates
    mm_states::IState::setStateMachineCb(&mCbReceiver);
}

////////////////////////////////////////////////////////////////////////////////
MainMenuState::~MainMenuState()
{
	SoundManager& sMgr(SoundManager::getInstance());

	// TODO: Remove all the memory and resources used by this state
	// calling exit()

	if(mVideoPlayerAPI){
		delete mVideoPlayerAPI;
	}

	// Unload the sounds loaded into the SoundManager for the MainMenu states.
	for (std::set<Ogre::String>::const_iterator it = mSoundsFilenames.begin();
			it != mSoundsFilenames.end() ;
			it++) {
		sMgr.unloadSound(*it);  // TODO Have all sounds been stopped?
	}
	mSoundsFilenames.clear();

    exit();
}

////////////////////////////////////////////////////////////////////////////////
void
MainMenuState::getResources(IMainState::ResourcesInfoVec &resourcesList,
                            const MainMachineInfo &info) const
{
    resourcesList.clear();

    IMainState::ResourcesInfo rinfo;
    rinfo.filePath = "/MainStates/MainMenuState/resources.cfg";
    rinfo.groupNames.push_back("MainMenuState");

    resourcesList.push_back(rinfo);
}

////////////////////////////////////////////////////////////////////////////////
void MainMenuState::enter(const MainMachineInfo &info)
{
	// load all the states
	ASSERT(mActualState == 0);
	ASSERT(mStates.empty());

	// parse xml file
	mXmlHelper.setFilename(CONFIG_FILENAME);
	mXmlHelper.openXml();
	ASSERT(mXmlHelper.hasOpenFile());

	// Load all the other stuff
	try {
		configureOvEffectManager();
		configureSoundManager();
		configureVideoAPI();
	} catch (ExceptionInfo &e) {
		debugERROR("Error occurred [%d]: %s\n", e.errCode, e.info.c_str());
		ASSERT(false);
	}


    if (!mm_states::StatesFactory::buildStates(mStates)){
        debugERROR("Error loading the States\n");
        ASSERT(false);
    }
    ASSERT(!mStates.empty());

	// configure all the states with the information to be used
	const int size = mStates.size();
	for(int i = 0; i < size; ++i){
		mStates[i]->setXmlElement(mXmlHelper.getRootElement());
	}

	// set the main state
	configureNewState(mStates[0]);

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

		// update position of the mouse cursor
		GLOBAL_CURSOR->updatePosition(
		        input::InputMouse::absX(),
		        input::InputMouse::absY());

		// render the frame
		if(!GLOBAL_ROOT->renderOneFrame()){
			break;
		}

		// update the state machine
		updateStateMachine();

		// update all the other things
		GLOBAL_MENU_MNGR->update();
        mVideoPlayerAPI->update(GLOBAL_TIME_FRAME);
        mOvEffManager.update();

		// This must be called when we use the renderOneFrame approach
		Ogre::WindowEventUtilities::messagePump();

		Common::GlobalObjects::lastTimeFrame = (timer.getMilliseconds() - timeStamp) * 0.001;
	}

	return MME_DONE;
}

////////////////////////////////////////////////////////////////////////////////
void MainMenuState::exit(void)
{
	// remove all the states
    for(size_t size = mStates.size(), i = 0; i < size; ++i){
        delete mStates[i];
    }
    mStates.clear();
    mXmlHelper.closeXml();
}
