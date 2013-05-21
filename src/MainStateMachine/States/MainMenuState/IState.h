/*
 * IState.h
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#ifndef ISTATE_H_
#define ISTATE_H_

#include <OgreString.h>

#include <vector>

#include "VideoRange.h"
#include "XMLHelper.h"
#include "CbMenuButton.h"
#include "InputKeyboard.h"
#include "GeneralTypedefs.h"
#include "SoundFamilyTable.h"
#include "SoundEnums.h"

class TiXmlElement;

namespace OvEff {
class MenuButtonEffect;
}


namespace mm_states {

enum Event {
	Done	= 0,
	Exit,
	Credits,
	Config,
	History,
	PlayGame,

};

// Volume of the background music for all MainMenu states
#define  BACKGROUND_MUSIC_VOLUME  DEFAULT_ENV_GAIN

/********************************************************************
 *
 * Mapping between sound codes and config.xml attributes names
 * (see "Sound codes enum" at the end of IState class definition)
 *
 * @======================#====================@
 * |     Sound code       | XML attribute name |
 * #======================#====================#
 * | SS_MOUSE_CLICK       | "mouse_click"      |
 * | SS_BACKGROUND_MUSIC  | "background"       |
 * @======================#====================@
 *
 ********************************************************************/


class IState;

typedef GenericFunctor2<void, IState *, Event> EventCallback;


class IState : public CbMenuButton::Cb {
public:
	IState(const Ogre::String &name);
	virtual ~IState();

	/**
	 * Set the callback ptr where we have to call when the IState has to
	 * emit some new event (this is the "main MachineState").
	 */
	static void setStateMachineCb(EventCallback *cb);

	/**
	 * Returns the associated video ranges to be used in this state
	 * -	The first range is associated with the "entering transition range".
	 * -	The second range is associated with the "looping range" (the video to
	 * 		be reproduced during the state)
	 * -	The third range is associated with the "exiting transition range"
	 * @param	vr	The video ranges
	 *
	 * @note	The default implementation of this function calls the
	 * 			getVideoRangesFromXML using the class name
	 */
	virtual void getVideoRanges(std::vector<VideoRange> &vr) const;

	/**
	 * Sets/Gets the actual video state duration (used by the MainMenuState)
	 */
	inline void setActualVideoStateDuration(float t);
	inline float getActualVideoStateDuration(void);

	/**
	 * Set the XML to extract the information (if is needed)
	 */
	inline void setXmlElement(const TiXmlElement *elem);

	/**
	 * Get the filename of the State
	 */
	inline const Ogre::String &name(void);

	/**
	 * Callback that receives the CbMenuButton and the Button id
	 */
	virtual void operator()(CbMenuButton *, CbMenuButton::ButtonID id) = 0;

	/**
	 * Function called to load all the resources used by the state.
	 * This function is called before we start the entering transition range
	 */
	virtual void load(void) = 0;

	/**
	 * Function called right before we start the main loop (update()).
	 * This function is called once and after that we start calling the update
	 * function.
	 */
	virtual void beforeUpdate(void) = 0;

	/**
	 * This function is called every frame, here we have to implement all the
	 * state logic.
	 */
	virtual void update(void) = 0;

	/**
	 * Function called once the state will be closed, so we have to unload all
	 * the resources used by this class.
	 */
	virtual void unload(void) = 0;

protected:

	/**
	 * Function used to stop the execution of this state, used to advise the
	 * StateMachine that this state has finish and pass the corresponding event
	 */
	void stateFinish(Event e);

	/**
	 * Get all the VideoRanges associated to this state name.
	 * @param	vr		Vector where the video ranges will be registered
	 */
	void getVideoRangesFromXML(std::vector<VideoRange> &vr) const;

	/**
	 * @brief
	 * Retrieve and register sounds filenames for this state.
	 *
	 * @remarks
	 * Fills up mSounds table, registering filenames for each sound code.
	 */
	void getSoundsFromXML();

	/**
	 * Build a list of CbMenuButtons from a list of names (IDs in the xml).
	 * The button[i] will be constructed from the name[i]. The buttons
	 * should be an empty list
	 * @param	buttons		Empty list where it will be put the constructed
	 * 						buttons and the effects
	 * @param	names		The list of names IDs to be builded from the XML
	 *
	 * @note: The effects and buttons are allocated dynamically so the caller
	 * 		  is responsible for the posterior deletion of that elements
	 */
	void buildButtons(std::vector<OvEff::MenuButtonEffect> &buttons,
			const std::vector<Ogre::String> &names);

	/**
	 * Function which returns the actual TiXmlElement used by this IState.
	 * Returns the tixmlelement which points to the root of this IState section
	 * or 0 on error.
	 */
	const TiXmlElement *getXmlElement(void) const;

	/**
     * Check if some key was pressed
     * @param   key     The key to check
     */
    inline bool isKeyPressed(input::KeyCode key);

private:
	/**
	 * Parse a video range
	 * @param	xml		The element pointing to the range
	 * @param	vr		The video range to be filled
	 */
	void parseVideoRange(const TiXmlElement *xml, VideoRange &vr) const;

	/**
	 * Parse a CbMenuButton from an xml
	 * @param	xml		The xml to be parsed
	 * @param	button	The button to be configured
	 */
	void parseCbMenuButton(const TiXmlElement *xml,
			OvEff::MenuButtonEffect &button);

private:
	float					mActualTimeDuration;
	Ogre::String			mName;
	const TiXmlElement*		mRootElement;
	static EventCallback*	sEventCb;

protected:
	// Sound codes of each menu state
	enum {
		SS_MOUSE_CLICK,		// Mouse click on menu button
		SS_BACKGROUND_MUSIC	// Background music for this state
	};

	SoundFamilyTable		mSounds;
};



inline void IState::setActualVideoStateDuration(float t)
{
	mActualTimeDuration = t;
}


inline float IState::getActualVideoStateDuration(void)
{
	return mActualTimeDuration;
}


inline void IState::setXmlElement(const TiXmlElement *elem)
{
	mRootElement = elem;
}


inline const Ogre::String &IState::name(void)
{
	return mName;
}


inline bool IState::isKeyPressed(input::KeyCode key)
{
    return input::InputKeyboard::isKeyDown(key);
}

}

#endif /* ISTATE_H_ */
