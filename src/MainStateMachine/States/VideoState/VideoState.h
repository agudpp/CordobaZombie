/*
 * VideoState.h
 *
 *  Created on: 01/09/2012
 *      Author: raul
 */

#ifndef VIDEO_STATE_H_
#define VIDEO_STATE_H_

#include "IMainState.h"
#include "FfmpegVideoPlayerAPI.h"


class VideoState : public IMainState
{

private:

	enum{
		ERROR = -1,
		OK,
		DONE,
	};

public:
	VideoState();
	virtual ~VideoState();

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


	/**
	 * Function used to get the resources files used by the state.
	 * The list returned is the list of the resources used by and only by this
	 * state.
	 */
	void getResources(ResourcesInfoVec &resourcesList);

private:

	/**
	 * Check input
	 */
	int checkInput(void);

	/**
	 * Load and play next video.
	 */
	int nextVideo(void);


public:

	VideoPlayerAPI	*mVpapi;
	int				mVideoIndex;

};

#endif /* VIDEO_STATE_H_ */
