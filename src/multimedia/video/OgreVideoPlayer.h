/*
 * OgreVideoPlayer.h
 *
 *  Created on: 21/10/2013
 *  Author: raul
 *
 */

#ifndef OGRE_VIDEO_PLAYER_H
#define OGRE_VIDEO_PLAYER_H

#include <Ogre.h>

#include <vector>
#include <string>
#include <VideoBuffer.h>

#include "VideoPlayer.h"

namespace mm{

class Video{

public:
	enum{
		OK = 0,
		ERROR
	};

public:

	Video(char* path, char* name, double start, double end);

	~Video();

	inline char*
	getName(void);

	inline char*
	getPath(void);

	inline int
	setStart(double start);

	inline int
	setEnd(double end);

	inline int
	getStart(double & start);

	inline int
	getEnd(double & end);

private:

	int			mStart;
	int 		mEnd;
	std::string *mPath;
	std::string *mName;
};



class OgreVideoScreen : public VideoBuffer
{

public:


	/*
	 * Parameters 'left', 'top', 'right', and 'bottom'
	 * define the relative size position and size of the Ogre
	 * screen in the monitor. Read the Ogre::Rectangle2D::setCorners
	 * description for more information. 'height' and 'width' determine the
	 * default video definition in number of pixels.
	 */
	OgreVideoScreen(Ogre::Real left,
					Ogre::Real top,
			 	 	Ogre::Real right,
			 	 	Ogre::Real bottom,
			 	 	Ogre::SceneManager *SCN_MNGR,
			 	 	unsigned int height,
			 	 	unsigned int width);


	/*
	 * Destroyer.
	 */
	virtual
	~OgreVideoScreen(void);


	/*
	 * Place data from 'source' into the buffer. 'offset' is the starting point
	 * in the buffer counted as number of bytes. 'size' indicates how much bytes
	 * should we write into the buffer. 'format' identifies the image data
	 * format as it is in 'source'.
	 */
	int
	fillBuffer(unsigned char* source = 0,
					Format format = RGB,
					unsigned int size = 0);


	/*
	 * Set corresponding size texture to the Ogre Screen, so to be able
	 * to present correctly a video of 'h' pixels high and 'w' pixels
	 * of width.
	 */
	int
	resize(unsigned int h, unsigned int w);


private:

	Ogre::Rectangle2D 			*mScreen;
	Ogre::SceneNode 			*mScreenNode;
	Ogre::TexturePtr 			mRttTexture;
	Ogre::MaterialPtr 			mRenderMaterial;
	unsigned int				mHeight;
	unsigned int				mWidth;


};



class OgreVideoPlayer{

public:
	enum{
		OK = 0,
		ERROR,
		ENDED
	};
public:

	/*
	 * Parameters correspond to the ones used by the OgreScreen.
	 *
	 */
	OgreVideoPlayer(Ogre::Real left,
					Ogre::Real top,
					Ogre::Real right,
					Ogre::Real bottom,
					Ogre::SceneManager *SCN_MNGR,
					unsigned int height,
					unsigned int width);


	~OgreVideoPlayer();

	/*
	 * Add video to playlist. Will be played from second 'start' until
	 * second 'end'. On success it returns the index of the video in the
	 * playlist.
	 */
	int
	queue(char *path, char* name, double start, double end);

	/*
	 * Start or continue playing.
	 */
	int
	play(void);

	/*
	 * Pause playing.
	 */
	int
	stop(void);

	/*
	 * Load the 'index' video in the playlist.
	 */
	int
	load(int idex);

	/*
	 * Update the videoPlayer with the time since last frame. Returns ENDED
	 * if the playlist reached it's end, ERROR if this video player is not
	 * playing, and OK on success.
	 */
	int
	update(double tslf);

	/*
	 * Repeat playlist
	 */
	inline void
	setRepeatPlayList(bool rep);

	/*
	 * Repeat video
	 */
	inline void
	setRepeatVideo(bool rep);

private:

	mm::VideoPlayer			*mVideoPlayer;
	mm::OgreVideoScreen 	*mScreen;
	std::vector<mm::Video>	mPlayList;
	bool					mIsPlaying;
	bool					mRepeatV;
	bool					mRepeatP;
	int						mIndex;  	// Index of the loaded video.
	mm::Video				*mVideo; 	// Loaded video.
};





///////////////////////////////////////////////////////////////////////////////
// Video class inline methods
///////////////////////////////////////////////////////////////////////////////

inline char*
Video::getName(void)
{
	ASSERT(mName);
	return mName.c_str();
}

///////////////////////////////////////////////////////////////////////////////
inline char*
Video::getPath(void)
{
	ASSERT(mPath);
	return mPath.c_str();
}

///////////////////////////////////////////////////////////////////////////////

inline int
Video::setEnd(int end)
{
	if (mStart > end){
		return ERROR;
	}else{
		mEnd = end;
		return OK;
	}
}

///////////////////////////////////////////////////////////////////////////////

inline int
Video::setStart(int start)
{
	if (mEnd < start){
		return ERROR;
	}else{
		mStart = start;
		return OK;
	}
}

///////////////////////////////////////////////////////////////////////////////

inline int
Video::getStart(double & start)
{
	start = mStart;
	return OK;
}

///////////////////////////////////////////////////////////////////////////////

inline int
Video::getEnd(double & end)
{
	end = mEnd;
	return OK;
}


///////////////////////////////////////////////////////////////////////////////
// OgreVideoPlayer inline methods
///////////////////////////////////////////////////////////////////////////////



inline void
OgreVideoPlayer::setRepeatPlayList(bool rep){
	mRepeatP = rep;
}

///////////////////////////////////////////////////////////////////////////////

inline void
OgreVideoPlayer::setRepeatVideo(bool rep){
	mRepeatV = rep;
}


///////////////////////////////////////////////////////////////////////////////
}

#endif //OGRE_VIDEO_PLAYER_H
