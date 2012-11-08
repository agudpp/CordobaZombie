/*
 * PreGameState.cpp
 *
 *  Created on: 28/10/2012
 *      Author: raul
 */

//#include <OgreOverlayManager.h>
//#include <OgreResourceManager.h>

//#include "GlobalObjects.h"
#include "DebugUtil.h"
#include "PreGameState.h"
//#include "GUIHelper.h"


const std::string PreGameState::PREGAMEDIRNAME = "PreGameState/";

const char *PreGameState::BUTTONS_NAMES[NUMBER_BUTTONS] =  {
        "exitButton",
        "prevButton",
        "nextButton",
};



////////////////////////////////////////////////////////////////////////////////
void PreGameState::showBackground(const Ogre::String &overlayName)
{
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();

	ASSERT(!mBackground);
	mBackground = om.getByName(overlayName);
	if(!mBackground){
		debugERROR("Error loading the overlay %s\n", overlayName.c_str());
		return;
	}

	mBackground->show();
}

////////////////////////////////////////////////////////////////////////////////
void PreGameState::destroyBackground(void)
{
	if(!mBackground) return;
	GUIHelper::fullDestroyOverlay(mBackground);
}
////////////////////////////////////////////////////////////////////////////////
PreGameState::PreGameState() :
IMainState("LoadingState"),
mBackground(0),
mPreGamePath(""),
mSlidePlayer(0)
{
}

////////////////////////////////////////////////////////////////////////////////
PreGameState::~PreGameState()
{
	destroyBackground();
	delete mSlidePlayer;
}

////////////////////////////////////////////////////////////////////////////////
void PreGameState::createButtons(const TiXmlElement *root){

	for(int i = 0; i < ButtonIndex::NUMBER_BUTTONS; ++i){
		mButtonNames.push_back(BUTTONS_NAMES[i]);
	}
	CbBHelper::buildButtons(root, mButtonNames, mCbButtons);
	ASSERT(mButtonNames.size()==mCbButtons.size());
	//Seteamos el callback de los botones
	for(size_t i = 0; i < NUMBER_BUTTONS; ++i){
		mCbButtons[i].getEffect()->addCallback(
		        boost::bind(&PreGameState::operator(), this, _1));
	}

	// Enable buttons
	for(size_t i = 0, size = ButtonIndex::NUMBER_BUTTONS; i < size; ++i){
		mCbButtons[i].getButton()->setEnable(true);
		ASSERT(mButtons[i].getEffect());
		mCbButtons[i].getEffect()->start();
		mCbButtons[i].getEffect()->getElement()->show();
	}


}


////////////////////////////////////////////////////////////////////////////////
void PreGameState::createSlidePlayer( const char *overlays
									, const char *effects
									, const char* slidenames)
{

	mSlidePlayer = SlidePlayer(overlays, effects);
	ASSERT(mSlidePlayer);
	// Load the slides
	Ogre::MaterialManager& materialman =
			Ogre::MaterialManager::getSingleton();
	const std::string MatName = slidenames;
	MatName += '/';
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

	mSlidePlayer->show();
}


////////////////////////////////////////////////////////////////////////////////
/**
 * Entering the state with additional info
 */
void PreGameState::enter(const MainMachineInfo &info)
{
	debugGREEN("ENTERING PRE GAME STATE\n");

	std::string levelPath = info.params.at("LEVEL_PATH");
	mPreGamePath = levelPath + PREGAMEDIRNAME;

	// Load resources
	xmlhelper.setFilename(mPreGamePath);
	const TiXmlElement *config = xmlhelper.getRootElement();
	// Background
	std::string backOverlay = config->Attribute("Background");
	ASSERT(!backOverlay.empty());
	this->showBackground(backOverlay);

	// Slides
	createSlidePlayer(config->Attribute("SlideOverlays"),
			   	      config->Attribute("SlideOverlayEffects"),
			   	      config->Attribute("SlideNames"));

	//Buttons
	createButtons(config);

	/* TODO aca quede. Revisar que no me falte mostrar nada mas y en especial
	 * lo del overlaymanager que me habia cagado la ultima vez con el history
	 * state.
	 */

}

////////////////////////////////////////////////////////////////////////////////
/**
 * Update the state... This function will be called once.
 * @param	info	A structure used to pass information between states.
 * @return	event	The resulting event to send back to the main state machine.
 */
MainMachineEvent PreGameState::update(MainMachineInfo &info)
{

	Ogre::Timer timer;
	float timeStamp = 0;
	MainMachineEvent result = MME_DONE;

	float count = 0;
	bool press = false;



	// here we load the level... note that the RenderOneFrame operation is in
	// the "Updater" class... Here we only call the LoadingManager and
	// wait their callbacks call to update the windows


	// here is the main loop
	// TODO: fix the FrameElapsedTime and check how to get the ogres one.
//	while(true) {
//		timeStamp = timer.getMilliseconds();
//
//		// TODO: solo chequeamos escape aca pero no va...
//		GLOBAL_KEYBOARD->capture();
//		if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)){
//			if(!press){
//				press = true;
//			}
//		} else {press = false;}
//
//
//		if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_ESCAPE)){
//			break;
//		}
//		mLoadingBar.setState(count);
//		count += 0.1f;
//		if(count >= 100.0f){count = 0;}
//
//		// render the frame
//		if(!GLOBAL_ROOT->renderOneFrame()){
//			result = -1; //TODO: poner un erro real aca
//			break;
//		}
//
//		// This must be called when we use the renderOneFrame approach
//		Ogre::WindowEventUtilities::messagePump();
//
//		Common::GlobalObjects::lastTimeFrame = (timer.getMilliseconds() - timeStamp) * 0.001;
//	}

	return result;
}


/**
 * Function called when the state is not "the actual" anymore
 */
void LoadingState::exit(void)
{
	debugRED("Exit STATE\n");
	destroyBackground();
	unloadLoadingBar();

	// detroy the document
	mLoaderManager->removeAll();
	delete mDoc;
	mDoc = 0;

}


