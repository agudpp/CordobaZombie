/*
 * PreGameState.h
 *
 *  Created on: 28/10/2012
 *  Author: raul
 *
 *  Description: IMainState that runs before each playing level. Displays a
 *  set of slides with information about coming playing level.
 */

#ifndef PREGAMESTATE_H_
#define PREGAMESTATE_H_

#include "IMainState.h"
#include "SlidePlayer.h"
#include "MenuButtonEffect.h"
#include "CbMenuButton.h"
#include "OverlayEffect.h"


class PreGameState : public IMainState, public CbMenuButton::Cb
{

	enum ButtonIndex {
		exitButton = 0,
		prevButton,
		nextButton,
		NUMBER_BUTTONS,  // only to know the number of buttons
	};

	static const char *BUTTONS_NAMES[NUMBER_BUTTONS];

	enum InternalState{
		ENTER = 0,
		LOOP,
		HIDE,
		EXIT,
	};

private:

	static const std::string PREGAMEDIRNAME;

public:
	PreGameState();
	virtual ~PreGameState();

	/**
	 * Entering the state with additional info
	 * @note	We get the xml to parse throw the
	 * 			info.params["LOAD_LEVEL_XML_NAME"] to get the filename of the
	 * 			xml.
	 */
	virtual void enter(const MainMachineInfo &info);

	/**
	 * Update the state... This function will be called once.
	 * @param	info	The structure used to pass throw the states. If we want
	 * 					to fill it with some information then we can do it.
	 * @return	event	The event that was happend.
	 */
	virtual MainMachineEvent update(MainMachineInfo &info);

	/**
	 * Function called when the state is not "the actual" anymore
	 */
	virtual void exit(void);

	/**
	 * Function used to get the resources files used by the state.
	 * The list returned is the list of the resources used by and only by this
	 * state.
	 */
	virtual void getResources(IMainState::ResourcesInfoVec &resourcesList) const;

	/**
	 * Call-back function for menu buttons being pressed.
	 */
	void operator()(CbMenuButton * b, CbMenuButton::ButtonID id);

	/*
	 * Call back function to let buttons effect tell us when they have done
	 * hiding.
	 */
	void operator()(OvEff::OverlayEffect::EventID id);


protected:
	/**
	 * Show/Destroy background.
	 */
	void showBackground(const Ogre::String &overlayName);
	void destroyBackground(void);

	/*
	 * Create each menu button. 'root' for getting the description of each of
	 * them.
	 */
	void createButtons(const TiXmlElement *root);

	/*
	 * Set mOvEffManager as the overlay effect manager.
	 */
	inline void configureOEffMngr(void);

	/*
	 * Construct slide player and put slides in it.
	 * TODO mejorar el cargado de slides para que no tenga que hacer tanto
	 * desde aca :S
	 */
	void buildSlidePlayer( const char* overlays, const char* effects
						 , const char *slidenames);


	/*
	 * Hide buttons and overlays before going out of this state. (normaly
	 * using effects.
	 */
	void hideToExit(void);

	/*
	 * Check if there are keys being pressed.
	 */
	void checkKeyInput(void);




protected:
	typedef std::vector<OvEff::MenuButtonEffect>	ButtonsEffectVec;
	typedef std::vector<Ogre::String>				StrVec;

	Ogre::Overlay				*mBackground;
	SlidePlayer					*mSlidePlayer;
	std::string					mPreGamePath;
	StrVec					 	mButtonNames;
	ButtonsEffectVec			mCbButtons;
	OvEff::OverlayEffectManager	mOvEffManager;
	int							mState;

};



inline void PreGameState::configureOEffMngr(void)
{
	OvEff::OverlayEffect::setManager(&mOvEffManager);
}



#endif /* LOADINGSTATE_H_ */
