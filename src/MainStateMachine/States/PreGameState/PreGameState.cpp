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
#include "MenuManager.h"
#include "MouseCursor.h"


const std::string PreGameState::PREGAMEDIRNAME = "PreGameState/";

const char *PreGameState::BUTTONS_NAMES[NUMBER_BUTTONS] =  {
        "ExitButton",
        "PrevButton",
        "NextButton",
};


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
/**
 * Entering the state with additional info
 */
void PreGameState::enter(const MainMachineInfo &info)
{
	debugGREEN("Entering PreGame State\n");

	// Configure overlay effects manager
	configureOEffMngr();

	// get level pre-game path
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

	// Slide player
	const TiXmlElement *slidesNode = config->FirstChildElement("SlidePlayer");
	ASSERT(slidesNode);
	buildSlidePlayer( slidesNode->Attribute("Overlays"),
					  slidesNode->Attribute("Effects"),
					  slidesNode->Attribute("Names"));

	// Buttons
	createButtons(config);

	// Close xml file
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

		// capture input
		GLOBAL_KEYBOARD->capture();
		GLOBAL_MOUSE->capture();

		// Check pressed keys
		if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_ESCAPE)){
			if(!press){
				press = true;
				debugGREEN("EXIT\n");
				break;
			}
		}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_B)){
			if(!press){
				press = true;
				mSlidePlayer->prev();
			}
		}else if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_N)){
			if(!press){
				press = true;
				mSlidePlayer->next();
			}
		}else {press = false;}


		// update position of the mouse cursor
		GLOBAL_CURSOR->updatePosition(
		        GLOBAL_MOUSE->getMouseState().X.abs,
		        GLOBAL_MOUSE->getMouseState().Y.abs);

		// render the frame
		if(!GLOBAL_ROOT->renderOneFrame()){
			result = MME_ERROR;
			break;
		}

		// update everything else
		GLOBAL_MENU_MNGR->update();
        mOvEffManager.update();
		mSlidePlayer->update();

		// This must be called when we use the renderOneFrame approach
		Ogre::WindowEventUtilities::messagePump();

		GLOBAL_TIME_FRAME =	(timer.getMilliseconds() - timeStamp) * 0.001;

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

////////////////////////////////////////////////////////////////////////////////
void PreGameState::createButtons(const TiXmlElement *root){

    mButtonNames.reserve(NUMBER_BUTTONS);
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

    // show the overlay
    Ogre::Overlay *overlay = Ogre::OverlayManager::getSingleton().getByName(
            "preGameStateOverlay/buttons");
    ASSERT(overlay);
    overlay->show();

	// Enable buttons
	for(size_t i = 0, size = ButtonIndex::NUMBER_BUTTONS; i < size; ++i){
		mCbButtons[i].getButton()->setEnable(true);
		ASSERT(mCbButtons[i].getEffect());
		mCbButtons[i].getEffect()->start();
		mCbButtons[i].getEffect()->getElement()->show();
	}
}

////////////////////////////////////////////////////////////////////////////////
void PreGameState::operator()(CbMenuButton * b, CbMenuButton::ButtonID id)
{
	debugGREEN("Button %s callback!!\n", b->getName().c_str());

	if(b == mCbButtons[PreGameState::exitButton].getButton()
			&& id == CbMenuButton::LEFT_BUTTON)
	{
		debugGREEN("EXIT\n");
		//TODO exit here ...
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
		debugERROR("Invalid button or button event:S\n");
		ASSERT(false);
	}

}

////////////////////////////////////////////////////////////////////////////////
void PreGameState::operator()(OvEff::OverlayEffect::EventID id){


	debugBLUE("operator()\n");

    if(id == OvEff::OverlayEffect::ENDING ){ //TODO && state == EXIT
        // Disable buttons
//   		for(size_t i = 0, size = mCbButtons.size(); i < size; ++i){
//			mCbButtons[i].getButton()->setEnable(false);
//		}

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






