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

	/**
	 * This function have to be implemented to receive the input events from the
	 * keyboard (for example, if the user press Escape or other key).
	 * The Logic have to be implemented in the state itself and not in the
	 * MainMenuState
	 * @param	key		The keyboard pressed
	 *
	 * @note	We only will call this function with KeyPress event (only once
	 * 			if the user still pressing the key, this function will be called
	 * 			once).
	 */
	virtual void keyPressed(input::KeyCode key) = 0;

protected:

	/**
	 * Function used to stop the execution of this state, used to advise the
	 * StateMachine that this state has finish and pass the corresponding event
	 */
	void stateFinish(Event e);

	/**
	 * TODO: Aca deberiamos definir la interfaz usada para reproducir sonidos
	 * y cargarlos al SoundManager (static *). Ya que la gran mayoria de los
	 * estados tienen sonidos + musica.
	 */

	/**
	 * Get all the VideoRanges associated to this state name.
	 * @param	vr		The vector where it will be put the video ranges
	 */
	void getVideoRangesFromXML(std::vector<VideoRange> &vr) const;

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
			const std::vector<Ogre::String> &names) const;

	/**
	 * Function which returns the actual TiXmlElement used by this IState.
	 * Returns the tixmlelement which points to the root of this IState section
	 * or 0 on error.
	 */
	const TiXmlElement *getXmlElement(void) const;

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
			OvEff::MenuButtonEffect &button) const;

private:
	float			mActualTimeDuration;
	Ogre::String	mName;
	const TiXmlElement *mRootElement;

	static EventCallback *sEventCb;

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

}

#endif /* ISTATE_H_ */
