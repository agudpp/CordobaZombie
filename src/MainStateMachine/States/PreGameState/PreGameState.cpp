/*
 * PreGameState.cpp
 *
 *  Created on: 28/10/2012
 *      Author: raul
 */

#include "PreGameState.h"

#include <boost/bind.hpp>
#include "DebugUtil.h"
#include "GUIHelper.h"
#include "ButtonHelper.h"



const std::string PreGameState::PREGAMEDIRNAME = "PreGameState/";

const char *PreGameState::BUTTONS_NAMES[NUMBER_BUTTONS] =  {
        "ExitButton",
        "PrevButton",
        "NextButton",
};


////////////////////////////////////////////////////////////////////////////////
void
PreGameState::getResources(IMainState::ResourcesInfoVec & resourcesList) const{
    resourcesList.clear();

    //TODO A esta funcion le faltaría un parámetro, el cual es un diccionario
    //como en la funcion de Enter, de allí deberia sacar el nombre del nivel
    // y no hardcodear 'Demo' como estoy haciendo.

    IMainState::ResourcesInfo rinfo;
    rinfo.filePath = "/Levels/Demo/" + PREGAMEDIRNAME + "resources.cfg";
    rinfo.groupNames.push_back("PreGameState");

    resourcesList.push_back(rinfo);
}




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

	CbBHelper::buildButtons(*root, mButtonNames, mCbButtons);
	ASSERT(mButtonNames.size() == mCbButtons.size());

	//Seteamos el callback de los botones
	for(size_t i = 0; i < NUMBER_BUTTONS; ++i){
		mCbButtons[i].getEffect()->addCallback(
		        boost::bind(&PreGameState::operator(), this, _1));
	}

	debugGREEN("Enabling buttons\n");

	// Enable buttons
	for(size_t i = 0, size = ButtonIndex::NUMBER_BUTTONS; i < size; ++i){
		mCbButtons[i].getButton()->setEnable(true);
		ASSERT(mCbButtons[i].getEffect());
		mCbButtons[i].getEffect()->start();
		debugGREEN("Showing button %s\n",
				   mCbButtons[i].getButton()->getName().c_str());
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
void PreGameState::operator()(OvEff::OverlayEffect::EventID id){

    if(id == OvEff::OverlayEffect::ENDING){
        // Disable buttons
   		for(size_t i = 0, size = mCbButtons.size(); i < size; ++i){
			mCbButtons[i].getButton()->setEnable(false);
		}

		debugRAUL("Operator going out\n");

        //TODO terminar de salir a donde sea que tenga que salir.

    }else{
        return;
    }

	return;
}



////////////////////////////////////////////////////////////////////////////////
void PreGameState::buildSlidePlayer( const char *overlays
								   , const char *effects
								   , const char *slidenames)
{

	mSlidePlayer = new SlidePlayer(overlays, effects);
	ASSERT(mSlidePlayer);
	// Load the slides
	Ogre::MaterialManager& materialman =
			Ogre::MaterialManager::getSingleton();
	std::string MatName = slidenames;
	MatName += "/";
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
	xmlhelper.setFilename("config.xml");
	xmlhelper.openXml();
	const TiXmlElement *config = xmlhelper.getRootElement();
	ASSERT(config);

	// Background
	const TiXmlElement *bkgrdNode = config->FirstChildElement("Background");
	ASSERT(bkgrdNode);
	std::string backOverlay = bkgrdNode->Attribute("path");
	ASSERT(!backOverlay.empty());
	this->showBackground(backOverlay);

	const TiXmlElement *slidesNode = config->FirstChildElement("SlidePlayer");
	ASSERT(slidesNode);
	// Slides
	buildSlidePlayer( slidesNode->Attribute("Overlays"),
					  slidesNode->Attribute("Effects"),
					  slidesNode->Attribute("Names"));

	//Buttons
	createButtons(config);
	xmlhelper.closeXml();
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
		if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_ESCAPE)){
			if(!press){
				press = true;
				debugGREEN("EXIT\n");
				break;
			}
		}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_B)){
			if(!press){
				press = true;
				debugGREEN("Prev slide\n");
				mSlidePlayer->prev();
			}
		}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_N)){
			if(!press){
				press = true;
				debugGREEN("Next slide\n");
				mSlidePlayer->next();
			}
		}else {press = false;}

		// render the frame
		if(!GLOBAL_ROOT->renderOneFrame()){
			result = MME_ERROR;
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
void PreGameState::exit(void)
{
	debugRED("Exit STATE\n");
	destroyBackground();

	delete mSlidePlayer;
	mSlidePlayer = 0;

}
