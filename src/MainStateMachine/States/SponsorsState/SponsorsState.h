/*
 * SponsorsState.h
 *
 *  Created on: 16/06/2012
 *      Author: agustin
 */

#ifndef SPONSORSSTATE_H_
#define SPONSORSSTATE_H_

#include <OgreOverlay.h>
#include "IMainState.h"

class SponsorsState : public IMainState
{
	enum {
		STATE_FADE_IN = 0,
		STATE_SPONSORS,
		STATE_FADE_OUT,
		STATE_END,
	};
public:
	SponsorsState();
	virtual ~SponsorsState();

	/**
	 * Configure the Times
	 * @param	ft		The fader time
	 * @param	st		The Sponsors showing time
	 */
	void configure(Ogre::Real ft, Ogre::Real st);

    /**
     * Function used to get the resources files used by the state.
     * The list returned is the list of the resources used by and only by this
     * state.
     */
    void getResources(IMainState::ResourcesInfoVec &resourcesList) const;

	/**
	 * Entering the state with additional info
	 */
	void enter(const MainMachineInfo &info);

	/**
	 * Update the state... This function will be called once.
	 * @param	info	The structure used to pass throw the states. If we want
	 * 					to fill it with some information then we can do it.
	 * @return	event	The event that was happend.
	 */
	MainMachineEvent update(MainMachineInfo &info);

	/**
	 * Function called when the state is not "the actual" anymore
	 */
	void exit(void);

protected:
	/**
	 * Faders functions
	 * @return	true	If the fader finish
	 * 			false	if the fader not finish
	 */
	bool enterFade(Ogre::Real ftime);
	bool exitFade(Ogre::Real ftime);

	/**
	 * Showing the sponsors
	 */
	bool showSponsors(Ogre::Real ftime);

public:
	Ogre::Overlay		*mOverlay;
	Ogre::Overlay 		*mFader;
	int					mState;
	Ogre::Real			mFaderTime;
	Ogre::Real			mSponsorsTime;
	Ogre::Real			mAccumTime;
	Ogre::TextureUnitState	*mTexture;
};

#endif /* SPONSORSSTATE_H_ */
