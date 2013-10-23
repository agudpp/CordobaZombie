/*
 * OgreVideoPlayer.cpp
 *
 *  Created on: 21/10/2013
 *  Author: raul
 *
 */

#include <string>
#include <DebugUtil.h>
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
	ASSERT(start <= end);

	mPath = new std::string(path);
	mName = new std::string(name);

}

///////////////////////////////////////////////////////////////////////////////


Video::~Video()
{
	ASSERT(mPath);
	ASSERT(mName);
	delete mPath;
	delete mName;
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
		mScreen(0),
		mScreenNode(0),
		mRenderMaterial(0),
		mRttTexture(0),
		mHeight(height),
		mWidth(width)
{
    // The screen
    mScreen = new Ogre::Rectangle2D(true);
    mScreen->setCorners(left, top, right, bottom);
    mScreen->setBoundingBox(Ogre::AxisAlignedBox(
    		-100000.0f * Ogre::Vector3::UNIT_SCALE,
    		100000.0f * Ogre::Vector3::UNIT_SCALE));

    ASSERT(SCN_MNGR);
	mScreenNode = SCN_MNGR->getRootSceneNode()->createChildSceneNode();

	SCN_MNGR->getRootSceneNode()->setVisible(true, true);
    mScreenNode->attachObject(mScreen);


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

    mRttTexture = rtttex;
    ASSERT(mRttTexture.get());

    //set material texture
	mRenderMaterial->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
    mRenderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(
    		"RttTex");

    //set screen material
    mScreen->setMaterial("RttMat");

	if(!mScreen){
		debugERROR("We didn't get a screen for our videos :S\n");
	}
}

///////////////////////////////////////////////////////////////////////////////

OgreVideoScreen::~OgreVideoScreen(void){
	if(mScreen){
		delete mScreen;
		mScreen = 0;
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

		for (size_t i = 0; i < bound; i+=4){
			pDest[i] 		= source[0];
			pDest[i + 1] 	= source[0 + 1];
			pDest[i + 2]    = source[0 + 2];
			source += 3;
		}

		PixelBuffer->unlock();

	}else{
		debugERROR("Unexpected video format!\n")
		return ERROR;
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
    mScreen->setMaterial("RttMat");

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
									unsigned int width):
	mIndex(-1),
	mIsPlaying(false),
	mVideo(0),
	mRepeatV(false),
	mRepeatP(false)
{
	mScreen = new OgreVideoScreen(left,top,right,bottom,SCN_MNGR,height,width);
	ASSERT(mScreen);
	mVideoPlayer = new mm::VideoPlayer(mScreen);
}


///////////////////////////////////////////////////////////////////////////////
OgreVideoPlayer::~OgreVideoPlayer(void)
{
	// delete screen
	delete mScreen;

	// delete videos queued into the playlist
	for(int i = 0; i < mPlayList.size(); i++){
		delete mPlayList[i];
		mPlayList[i] = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
int
OgreVideoPlayer::queue(const char* path, const char* name,
		double start, double end)
{
	//TODO ASSERT's

	Video *video = new Video(path, name, start, end);
	mPlayList.push_back(video);
	// If playlist was empty then load this video.
	if(mPlayList.size()==1){
		if( ERROR == load(0)){
			return ERROR;
		}else{
			play();
		}
	}
	return mPlayList.size()-1;
}

///////////////////////////////////////////////////////////////////////////////

int
OgreVideoPlayer::load(int index){

	if(index >= mPlayList.size()){
		return ERROR;
	}else{
		// Get full path to the video
		mVideo = mPlayList[index];
		mIndex = index;
		// Load video player, resize screen and seek for starting point;
		mVideoPlayer->unload();
		mVideoPlayer->load(mVideo->getPath());
		unsigned int h = 0, w = 0;
		mVideoPlayer->getSizes(h,w);
		mScreen->resize(h,w);
		double start = 0;
		mVideo->getStart(start);
		if( start != 0.0){
			mVideoPlayer->seek_time_stamp(start);
		}
	}

	return OK;
}

///////////////////////////////////////////////////////////////////////////////
int
OgreVideoPlayer::play()
{
	if(!mVideoPlayer->is_loaded()){
		return ERROR;
	}
	mVideoPlayer->play();
	mIsPlaying = true;
	return OK;
}

///////////////////////////////////////////////////////////////////////////////
int
OgreVideoPlayer::stop()
{
	mIsPlaying = false;
	return OK;
}

///////////////////////////////////////////////////////////////////////////////
int
OgreVideoPlayer::update(double tslf)
{
	if(!mIsPlaying){
		return ERROR;
	}else{
		ASSERT(mVideoPlayer->is_loaded());
		double t = 0;
		mVideoPlayer->get_playing_time_in_secs(t);
		double end = 0;
		ASSERT(mVideo != 0);
		mVideo->getEnd(end);
		if(t >= end){
			//stop();
			if(loadNext() == OK){
				play();
			}else{
				return ENDED;
			}
		}else{
			if(VideoPlayer::VIDEO_ENDED == mVideoPlayer->update(tslf)){
				//stop();
				if(loadNext() == OK){
					play();
				}else{
					return ENDED;
				}
			}
		}
	}
	return OK;
}

///////////////////////////////////////////////////////////////////////////////
int
OgreVideoPlayer::loadNext(void)
{
	if(mRepeatV){
		load(mIndex);
	}else if(mIndex < mPlayList.size()-1){
		load(mIndex+1);
	}else{
		if(mRepeatP){
			load(0);
		}else{
			return ERROR;
		}
	}
	return OK;
}


///////////////////////////////////////////////////////////////////////////////
}
