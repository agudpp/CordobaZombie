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


/*
 * Buttons names as they should appear in the '*.overlay' file.
 */
const char *PreGameState::BUTTONS_NAMES[NUMBER_BUTTONS] =  {
        "ExitButton",
        "PrevButton",
        "NextButton",
};


////////////////////////////////////////////////////////////////////////////////
PreGameState::PreGameState() :
IMainState("PreGameState"),
mBackground(0),
mPreGamePath(""),
mSlidePlayer(0),
mState(ENTER)
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
	showBackground(backOverlay);

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

	// Change internal state to looping
	mState = LOOP;
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

	// TODO: fix the FrameElapsedTime and check how to get the ogres one.

	// Main loop :

	while(true) {

		if (mState == EXIT) break;

		timeStamp = timer.getMilliseconds();

		// capture input
		GLOBAL_KEYBOARD->capture();
		GLOBAL_MOUSE->capture();

		if(mState == LOOP) checkKeyInput();

		// update position of the mouse cursor
		GLOBAL_CURSOR->updatePosition( GLOBAL_MOUSE->getMouseState().X.abs
									 , GLOBAL_MOUSE->getMouseState().Y.abs);

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


////////////////////////////////////////////////////////////////////////////////
/**
 * Function called when the state is not "the actual" anymore
 */
void PreGameState::exit(void)
{
	debugRED("Exit STATE\n");
	destroyBackground();

	delete mSlidePlayer;
	mSlidePlayer = 0;

	for(int i = 0, size = mCbButtons.size(); i < size; ++i){
		delete mCbButtons[i].getButton();
		delete mCbButtons[i].getEffect();
	}
	mCbButtons.clear();
	mButtonNames.clear();

}


////////////////////////////////////////////////////////////////////////////////
void
PreGameState::getResources(IMainState::ResourcesInfoVec & resourcesList,
		                   const MainMachineInfo &info) const{

    resourcesList.clear();

    //TODO A esta funcion le faltaría un parámetro, el cual es un diccionario
    //como en la funcion de Enter, de allí deberia sacar el nombre del nivel
    // y no hardcodear 'Demo' como estoy haciendo.

    IMainState::ResourcesInfo rinfo;
    rinfo.filePath = "/Levels/Demo/" + PREGAMEDIRNAME + "resources.cfg";
    rinfo.groupNames.push_back("PreGameState");

    debugRAUL("%s\n",rinfo.filePath.c_str());

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
	Ogre::OverlayManager::getSingleton().destroy(mBackground);
	mBackground = 0;
}



////////////////////////////////////////////////////////////////////////////////
void PreGameState::createButtons(const TiXmlElement *root){

    mButtonNames.reserve(NUMBER_BUTTONS);
	for(int i = 0; i < ButtonIndex::NUMBER_BUTTONS; ++i){
		mButtonNames.push_back(BUTTONS_NAMES[i]);
	}

	CbBHelper::buildButtons(*root, mButtonNames, mCbButtons);
	ASSERT(mButtonNames.size() == mCbButtons.size());

	//Set buttons callbacks
	for(size_t i = 0; i < NUMBER_BUTTONS; ++i){
		mCbButtons[i].getEffect()->addCallback(
		        boost::bind(&PreGameState::operator(), this, _1));
		static_cast<CbMenuButton*>(mCbButtons[i].getButton())->setCallback(this);
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
		hideToExit();
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

    if (id == OvEff::OverlayEffect::ENDING && mState == HIDE) {
    	mState = EXIT;
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

	mSlidePlayer->loadSlides(slidenames);

	mSlidePlayer->show();
}


////////////////////////////////////////////////////////////////////////////////

void PreGameState::hideToExit(void){

	ASSERT(mState < HIDE);
	mState = HIDE;

	for(int i = 0, size = mCbButtons.size(); i < size; ++i){
		mCbButtons[i].getButton()->setActive(false);
		mCbButtons[i].getButton()->setEnable(false);
		ASSERT(mCbButtons[i].getEffect());
		mCbButtons[i].getEffect()->complement();
		mCbButtons[i].getEffect()->start();
	}

}


////////////////////////////////////////////////////////////////////////////////

void PreGameState::checkKeyInput(void){

	static bool press = false;

	// Check pressed keys
	if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_ESCAPE)){
		if(!press){
			press = true;
			debugGREEN("EXIT\n");
			hideToExit();
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

}




