/*
 * IState.h
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#ifndef ISTATE_H_
#define ISTATE_H_

#include <vector>

#include "VideoRange.h"


namespace mm_states {

class IState {
public:

	// The different video states
	enum VideoState {
		ENTERING	= 0, // when we are reproducing the video and we are entering
		LOOPING,		 // when we have past the entering state
		EXITING,		 // when we are exiting the state
	};

public:
	IState();
	virtual ~IState();

	/**
	 * Returns the associated video ranges to be used in this state
	 * -	The first range is associated with the "entering transition range".
	 * -	The second range is associated with the "looping range" (the video to
	 * 		be reproduced during the state)
	 * -	The third range is associated with the "exiting transition range"
	 * @param	vr	The video ranges
	 */
	virtual void getVideoRanges(std::vector<VideoRange> &vr) const = 0;

	/**
	 * Sets/Gets the actual video state duration (used by the MainMenuState)
	 */
	inline void setActualVideoStateDuration(float t);
	inline float getActualVideoStateDuration(void);

	/**
	 * Function called to load all the resources used by the state.
	 * This function is called before we start the entering transition range
	 */
	virtual void load(void) = 0;

	/**
	 * This function is called when we start to reproduce some of the video
	 * ranges associated with this state.
	 * @param	vs		The videoState that is been reproduced, with this flag
	 * 					we can determine with is the actual state.
	 * @note Every time we enter (or during) a new VideoState we can get the
	 * 		 duration of the video corresponding to that state from
	 * 		 getActualVideoStateDuration
	 */
	virtual void update(VideoState vs) = 0;

	/**
	 * Function called once the state will be closed, so we have to unload all
	 * the resources used by this class.
	 */
	virtual void unload(void) = 0;

protected:

	/**
	 * TODO: Aca deberiamos definir la interfaz usada para reproducir sonidos
	 * y cargarlos al SoundManager (static *). Ya que la gran mayoria de los
	 * estados tienen sonidos + musica.
	 */

private:
	float			mActualTimeDuration;

};


inline void IState::setActualVideoStateDuration(float t)
{
	mActualTimeDuration = t;
}
inline float IState::getActualVideoStateDuration(void)
{
	return mActualTimeDuration;
}

}

#endif /* ISTATE_H_ */
