/*
 * PreGameState.h
 *
 *  Created on: 28/10/2012
 *      Author: raul
 */

#ifndef PREGAMESTATE_H_
#define PREGAMESTATE_H_
#include "IMainState.h"
#include "SlidePlayer.h"


class PreGameState : public IMainState
{

	enum ButtonIndex {
		exitButton = 0,
		prevButton,
		nextButton,
		NUMBER_BUTTONS,  // only to know the number of buttons
	};

	static const char *BUTTONS_NAMES[NUMBER_BUTTONS];

private:

	static const std::string PREGAMEDIRNAME;

public:
	PreGameState();
	virtual ~PreGameState();

	/**
	 * Entering the state with additional info
	 * @note	We get the xml to parse throw the
	 * 			info.params["LOAD_LEVEL_XML_NAME"] to get the filename of the xml
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


protected:
	/**
	 * Show/Destroy background
	 */
	void showBackground(const Ogre::String &overlayName);
	void destroyBackground(void);
	void createButtons(const TiXmlElement *root);
	void createSlidePlayer(const char* overlays, const char* effects);

protected:
	typedef std::vector<OvEff::MenuButtonEffect>	ButtonsEffectVec;
	typedef std::vector<Ogre::String>				StrVec;

	Ogre::Overlay				*mBackground;
	SlidePlayer					*mSlidePlayer;
	std::string					mPreGamePath;
	XMLHelper 					mXmlhelper;
	StrVec					 	mButtonNames;
	ButtonsEffectVec			mCbButtons;
};

#endif /* LOADINGSTATE_H_ */
