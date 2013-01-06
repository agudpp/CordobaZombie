/*
 * FirstInformationState.h
 *
 *  Created on: 14/08/2012
 *      Author: agustin
 */

#ifndef FIRSTINFORMATIONSTATE_H_
#define FIRSTINFORMATIONSTATE_H_

#include <OgreOverlay.h>
#include <OgreTextureUnitState.h>

#include <vector>

#include "IMainState.h"

class FirstInformationState : public IMainState
{
	static const float	SHOWING_TIME		=	3.65f;
	static const float	FADING_TIME			=	1.0f;

	enum {
		STATE_FADE_IN = 0,
		STATE_SPONSORS,
		STATE_FADE_OUT,
		STATE_END,
	};


public:
	FirstInformationState();
	virtual ~FirstInformationState();

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
    virtual void getResources(IMainState::ResourcesInfoVec &resourcesList,
                              const MainMachineInfo &info) const;

	/**
	 * Entering the state with additional info
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

private:
	/**
	 * Function used to show the next Overlay if there are any other.
	 * @return ov		The overlay to show, or 0 if there are no more overlays
	 */
	Ogre::Overlay *showNext(void);

	/**
	 * Faders functions
	 * @return	true	If the fader finish
	 * 			false	if the fader not finish
	 */
	bool enterFade(Ogre::Real ftime);
	bool exitFade(Ogre::Real ftime);

	/**
	 * Showing the infoPanel
	 */
	bool showInfo(Ogre::Real ftime);

	/**
	 * Deallocates all the resources
	 */
	void deallocateAll(void);

private:
	std::vector<Ogre::Overlay *>		mOverlays;
	std::vector<Ogre::Overlay *>		mToRemoveOverlays;
	Ogre::Overlay						*mOverlay;
	Ogre::Overlay 						*mFader;
	int									mState;
	Ogre::Real							mFaderTime;
	Ogre::Real							mSponsorsTime;
	Ogre::Real							mAccumTime;
	Ogre::TextureUnitState				*mTexture;
};

#endif /* FIRSTINFORMATIONSTATE_H_ */
