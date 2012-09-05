/*
 * VideoPlayer.h
 *
 *  Created on: Jan 13, 2012
 *      Author: Emi
 */

#ifndef VIDEOPLAYER_H_
#define VIDEOPLAYER_H_


#include <stdlib.h>
#include "OgreRoot.h"
#include <OgreMaterialManager.h>
#include <OgreTexture.h>
#include <OgrePixelFormat.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreTechnique.h>
#include <OgreTextureUnitState.h>


#include <TheoraVideoManager.h>
#include <TheoraVideoClip.h>
#include <TheoraVideoFrame.h>
#include <TheoraDataSource.h>


#include "OpenAL_AudioInterface.h"

using namespace std;


class VideoPlayer {
private:
	static const int NUM_PRECACHED_FRAMES = 32;


	Ogre::String fileName, pName;
	int  width, height, buffSize;
	bool started, loaded;

	TheoraVideoManager* videoMgr;
	TheoraVideoClip *videoClip;
	TheoraVideoFrame *videoFrame;


	OpenAL_AudioInterfaceFactory* iface_factory;


	//We set pointers to Ogre resourses
	Ogre::TexturePtr videoTexture;
	Ogre::MaterialPtr videoMaterial;
	Ogre::TextureUnitState* vtUnitState;


	//Buffers used to copy data from the Video Memory to the OgreTexture the player creates
	unsigned char *texData, *videoData;

	//Creates the OgreTexture and OgreMaterial and registers them into the system
	void createTexture();

	//Display a frame by updating the OgreTexture buffer
	void showVideoFrame();

	void update(float time_increase);

public:

	VideoPlayer();
	virtual ~VideoPlayer();
	Ogre::String getPlayerName(){return pName;};
	int getWidth(){return width;};
	int getHeight(){return height;};
	Ogre::TexturePtr getTexturePtr(){return videoTexture;};
	Ogre::String getTextureName(){return pName;};
	Ogre::MaterialPtr getMaterialPtr(){return videoMaterial;};
	Ogre::String getMaterialName(){return pName;};


	//Loads the video file and creates an Ogre material, which can be applied to any Ogre Entity
	int load(Ogre::String file);

	//Clears all the data. Destroys the video file and removes the texture and material
	void unload();

	void printVideoInfo();

	//Call this method every frame to update the video. If the player is paused or not loaded, this method
	//does not do anything
	void updateVideo(float updateTime);

	void play();
	void pause();
	void stop();
	bool isPaused(){return videoClip->isPaused();};


};

#endif /* VIDEOPLAYER_H_ */
