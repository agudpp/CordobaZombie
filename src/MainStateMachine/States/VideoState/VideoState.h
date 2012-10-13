/*
 * VideoState.h
 *
 *  Created on: 01/09/2012
 *      Author: raul
 */

#ifndef VIDEO_STATE_H_
#define VIDEO_STATE_H_

//#include <OgreOverlay.h>
#include "IMainState.h"
#include "FfmpegVideoPlayerAPI.h"


class VideoState : public IMainState
{

private:
	static const int VIDEO_STATE_LIST_SIZE = 0;
	static const char* VIDEO_STATE_LIST[VIDEO_STATE_LIST_SIZE] = {};
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

	VideoPlayerAPI	mVpapi;
	int				mVideoIndex;

};

#endif /* VIDEO_STATE_H_ */
