/*
 * OgreVideoPlayer.cpp
 *
 *  Created on: 21/10/2013
 *  Author: raul
 *
 */

#include <string>

#include <debug/DebugUtil.h>
#include <types/basics.h>

#include "OgreVideoPlayer.h"


namespace mm{



///////////////////////////////////////////////////////////////////////////////
// VIDEO CLASS
///////////////////////////////////////////////////////////////////////////////


Video::Video(const char* path, const char* name, double start, double end):
	mStart(start),
	mEnd(end)
{
	ASSERT(path != NULL);
	ASSERT(name != NULL);
	ASSERT(end < 0.0 || start <= end);

	mPath = path;
	mName = name;

}

///////////////////////////////////////////////////////////////////////////////


Video::~Video()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// OGRE VIDEO SCREEN CLASS
///////////////////////////////////////////////////////////////////////////////

OgreVideoScreen::OgreVideoScreen(Ogre::Real left,  Ogre::Real top,
								 Ogre::Real right, Ogre::Real bottom,
								 Ogre::SceneManager *SCN_MNGR,
								 unsigned int height,
								 unsigned int width) :
		mScreen(true),
		mScreenNode(0),
		mRenderMaterial(0),
		mRttTexture(0),
		mHeight(height),
		mWidth(width)
{
    // The screen
    mScreen.setCorners(left, top, right, bottom);
    mScreen.setBoundingBox(Ogre::AxisAlignedBox(
    		-100000.0f * Ogre::Vector3::UNIT_SCALE,
    		100000.0f * Ogre::Vector3::UNIT_SCALE));

    ASSERT(SCN_MNGR);
	mScreenNode = SCN_MNGR->getRootSceneNode()->createChildSceneNode();

	SCN_MNGR->getRootSceneNode()->setVisible(true, true);
    mScreenNode->attachObject(&mScreen);


    // Material for the screen
    static Ogre::MaterialPtr rendmat =
    		Ogre::MaterialManager::getSingleton().create("RttMat",
    		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    mRenderMaterial = rendmat;
    ASSERT(mRenderMaterial.get());

    Ogre::Technique* matTechnique = mRenderMaterial->createTechnique();
    matTechnique->createPass();
    mRenderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(true);

    //A temporary texture for the screen before something is loaded
    static Ogre::TexturePtr rtttex =
    		Ogre::TextureManager::getSingleton().createManual("RttTex",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D, width,
			height, 0,
			Ogre::PF_X8R8G8B8, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

    /*
     * TODO: note that rtttex->getFormat() and rtttex->getDesiredFormat() are
     * not allways the same (Ogre does what ever he wants :S) so it would be
     * good to check the getFormat() value and build the fillbuffer method
     * based on it.
     *
     * debugGREEN("format: real -> %i, desired -> %i\n",rtttex->getFormat(),
     * rtttex->getDesiredFormat());
     */


    mRttTexture = rtttex;
    ASSERT(mRttTexture.get());

    //set material texture
	mRenderMaterial->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
    mRenderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(
    		"RttTex");

    //set screen material
    mScreen.setMaterial("RttMat");

}

///////////////////////////////////////////////////////////////////////////////

OgreVideoScreen::~OgreVideoScreen(void)
{
    if (mScreenNode) {
        Ogre::SceneManager* creator = mScreenNode->getCreator();
        if (creator) {
            creator->getRootSceneNode()->removeAndDestroyChild(mScreenNode->getName());
            mScreenNode = 0;
        }
    }
}



///////////////////////////////////////////////////////////////////////////////

int
OgreVideoScreen::fillBuffer(unsigned char* source,
		mm::VideoBuffer::Format format,
		unsigned int size)
{

	if (format == RGB){

		Ogre::HardwarePixelBufferSharedPtr PixelBuffer =
				mRttTexture->getBuffer();

		PixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

		unsigned char * pDest =
			static_cast<unsigned char*>(PixelBuffer->getCurrentLock().data);

		unsigned int bound = (size/3)*4;
		if(size > mWidth*mHeight*3){
			bound = mWidth*mHeight*4;
			debugWARNING("Screen is too little for that frame.\n");
		}

		for (core::size_t i = 0; i < bound; i+=4){
			pDest[i] 		= source[0];
			pDest[i + 1] 	= source[0 + 1];
			pDest[i + 2]    = source[0 + 2];
			source += 3;
		}

		PixelBuffer->unlock();

	}else{
		debugERROR("Unexpected video format!\n")
		return C_ERROR;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////


int
OgreVideoScreen::resize(unsigned int h, unsigned int w){

	// Texture for the screen
    Ogre::TextureManager::getSingleton().remove("RttTex");
	mRttTexture =
			Ogre::TextureManager::getSingleton().createManual("RttTex",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D, w, h, 0,
			Ogre::PF_X8R8G8B8, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	//set material texture
	mRenderMaterial->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
    mRenderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(
    		"RttTex");
    //set screen material
    mScreen.setMaterial("RttMat");

    mHeight = h;
    mWidth = w;

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// OGRE VIDEO PLAYER CLASS
///////////////////////////////////////////////////////////////////////////////


OgreVideoPlayer::OgreVideoPlayer(	Ogre::Real left,
									Ogre::Real top,
									Ogre::Real right,
									Ogre::Real bottom,
									Ogre::SceneManager *SCN_MNGR,
									unsigned int height,
									unsigned int width) :
    mScreen(left,top,right,bottom,SCN_MNGR,height,width),
    mVideoPlayer(&mScreen),
	mIndex(-1),
	mIsPlaying(false),
	mRepeatV(false),
	mRepeatP(false)
{
}


///////////////////////////////////////////////////////////////////////////////
OgreVideoPlayer::~OgreVideoPlayer(void)
{
}

///////////////////////////////////////////////////////////////////////////////
int
OgreVideoPlayer::queue(const char* path, const char* name,
		double start, double end)
{
	//TODO ASSERT's
	mPlayList.push_back(Video(path, name, start, end));
	// If playlist was empty then load this video.
	if(mPlayList.size()==1){
		if(C_ERROR == load(0)){
			return C_ERROR;
		}else{
			play();
		}
	}
	return mPlayList.size()-1;
}

///////////////////////////////////////////////////////////////////////////////
void
OgreVideoPlayer::dequeueAll(void)
{
    stop();
    mIndex = -1;
    mPlayList.clear();
    mVideoPlayer.unload();
    mVideoPlayer.paint_black_screen();
}

///////////////////////////////////////////////////////////////////////////////

int
OgreVideoPlayer::load(int index){

	if(index >= mPlayList.size()){
		return C_ERROR;
	}else{
		// Get full path to the video
		Video& video = mPlayList[index];
		mIndex = index;
		// Load video player, resize screen and seek for starting point;
		mVideoPlayer.unload();
		debug("Loading video %s\n", video.getName());
		if(VideoPlayer::VIDEO_OK != mVideoPlayer.load(video.getPath())){
			return C_ERROR;
		}
		unsigned int h = 0, w = 0;
		mVideoPlayer.getSizes(h,w);
		mScreen.resize(h,w);
		double start = 0;
		video.getStart(start);
		if( start != 0.0){
			mVideoPlayer.seek_time_stamp(start);
		}
		double end = 0;
		video.getEnd(end);
//		debug("Loading video %s, from second %lf to %lf\n", video.getName(),
//				start, end);

	}

	return C_OK;
}

///////////////////////////////////////////////////////////////////////////////
int
OgreVideoPlayer::play()
{
	if(!mVideoPlayer.is_loaded()){
		return C_ERROR;
	}
	mVideoPlayer.play();
	mIsPlaying = true;
	return C_OK;
}

///////////////////////////////////////////////////////////////////////////////
int
OgreVideoPlayer::stop()
{
	mIsPlaying = false;
	return C_OK;
}

///////////////////////////////////////////////////////////////////////////////
int
OgreVideoPlayer::update(double tslf)
{
	if(!mIsPlaying){
		//debug("The actual video player is not playing\n");
		return C_ERROR;
	}else{
		ASSERT(mVideoPlayer.is_loaded());
		double t = 0;
		mVideoPlayer.get_playing_time_in_secs(t);
		double end = 0;
		Video& video = mPlayList[mIndex];
		video.getEnd(end);
		// negative value for end will play till it finishes.
		if(end >= 0.0 && t >= end){
//			debug("Video time limit reached\n");
			if(loadNext() == C_OK){
				play();
			}else{
			    mIsPlaying = false;
				return C_ENDED;
			}
		// has to play till the end or the end time limit hasn't been reached
		}else{
			if(VideoPlayer::VIDEO_ENDED == mVideoPlayer.update(tslf)){
				// If the video has ended
//				debug("Video ended (the actual video player says so)\n");
				if(loadNext() == C_OK){
					play();
				}else{
				    mIsPlaying = false;
					return C_ENDED;
				}
			}
		}
	}
	return C_OK;
}

///////////////////////////////////////////////////////////////////////////////
int
OgreVideoPlayer::loadNext(void)
{
//	debug("Load next: actual index %i\n", mIndex);
	if(mRepeatV){
		load(mIndex);
//		debug("Load next: load index %i\n", mIndex);
	}else if(mIndex < mPlayList.size()-1){
//		debug("Load next: load index %i\n", mIndex+1);
		load(mIndex+1);
	}else{
		if(mRepeatP){
//			debug("Load next: restart playlist\n");
			load(0);
		}else{
//			debug("Load next: no more videos to load\n");
			return C_ERROR;
		}
	}
	return C_OK;
}
///////////////////////////////////////////////////////////////////////////////

int
OgreVideoPlayer::next(void){

	int error = C_OK;
	bool saverepeatvideo = mRepeatV;
	bool saverepeatpl = mRepeatP;

	setRepeatPlayList(true);
	setRepeatVideo(false);

	error = loadNext();
	if (error != C_ERROR){
		error = play();
	}

	setRepeatPlayList(saverepeatpl);
	setRepeatVideo(saverepeatvideo);

	return error;
}


///////////////////////////////////////////////////////////////////////////////
}
