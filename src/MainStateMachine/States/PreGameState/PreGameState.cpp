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
void PreGameState::operator()(CbMenuButton * b, CbMenuButton::ButtonID id)
{
	if(b == mCbButtons[PreGameState::exitButton].getButton()
			&& id == CbMenuButton::LEFT_BUTTON)
	{
		debugGREEN("EXIT\n");
		//TODO exit here ...
		//mState = STATE_HIDDING;
		//this->hideToExit();
	}
	else if(b == mCbButtons[PreGameState::prevButton].getButton()
			&& id == CbMenuButton::LEFT_BUTTON)
	{
		mSlidePlayer->prev();
	}
	else if(b == mCbButtons[PreGameState::nextButton].getButton()
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
void PreGameState::createSlidePlayer( const char *overlays
									, const char *effects
									, const char *slidenames)
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
	XMLHelper xmlhelper;
	xmlhelper.setFilename(mPreGamePath + "config.xml");
	const TiXmlElement *config = xmlhelper.getRootElement();
	// Background
	const TiXmlElement *bkgrdNode = config->FirstChildElement("Background");
	ASSERT(bkgrdNode);
	std::string backOverlay = bkgrdNode->Attribute("path");
	ASSERT(!backOverlay.empty());
	this->showBackground(backOverlay);

	const TiXmlElement *slidesNode = config->FirstChildElement("SlidePlayer");
	ASSERT(slidesNode);
	// Slides
	createSlidePlayer(slidesNode->Attribute("Overlays"),
					  slidesNode->Attribute("Effects"),
					  slidesNode->Attribute("Names"));

	//Buttons
	createButtons(config);
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

	// here is the main loop
	// TODO: fix the FrameElapsedTime and check how to get the ogres one.
	while(true) {
		timeStamp = timer.getMilliseconds();

		GLOBAL_KEYBOARD->capture();
		if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)){
			if(!press){
				press = true;
				debugGREEN("EXIT\n");
				// break
			}
		} else {press = false;}

		// render the frame
		if(!GLOBAL_ROOT->renderOneFrame()){
			result = -1; //TODO: poner un error real aca
			break;
		}

		// This must be called when we use the renderOneFrame approach
		Ogre::WindowEventUtilities::messagePump();

		Common::GlobalObjects::lastTimeFrame =
				(timer.getMilliseconds() - timeStamp) * 0.001;

		// Update the slide player
		mSlidePlayer->update();
	}

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
