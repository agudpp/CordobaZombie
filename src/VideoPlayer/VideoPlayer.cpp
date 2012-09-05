/*
 * VideoPlayer.cpp
 *
 *  Created on: Jan 13, 2012
 *      Author: Emi
 */

#include "VideoPlayer.h"

VideoPlayer::VideoPlayer()
{
	//We set the name of the player, which is also the name of the Ogre texture and the Ogre material the player creates
	pName = "videoPlayer";

	//Internal vaariable used by the update() method
	started = true;

}

VideoPlayer::~VideoPlayer()
{
	//We called unload, in case it was not called before destroying the videoPlayer
	//If it was unloaded before, this method does not do anything
	unload();

}

int VideoPlayer::load(Ogre::String file)
{
	if(!loaded){
		fileName = file;
		videoMgr = new TheoraVideoManager(2);
		videoMgr->setDefaultNumPrecachedFrames(NUM_PRECACHED_FRAMES);

		iface_factory=new OpenAL_AudioInterfaceFactory();
		videoMgr->setAudioInterfaceFactory(iface_factory);

		videoClip = videoMgr->createVideoClip(fileName,TH_BGRA);
		videoClip->setAutoRestart(true);
		width = videoClip->getWidth();
		height = videoClip->getHeight();


		buffSize = width*height*4;

		createTexture();

		//We do not want the video to play instantly
		videoClip->pause();

		loaded=true;
	}
	else{
		//Already loaded!!! We ignore this call
	}
	return 0;
}


void VideoPlayer::unload()
{
	if(loaded){
		//We stop theplayback before unload the resources
		videoClip->stop();

		videoMaterial->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();

		//We remove the texture and the material from each Manager
		Ogre::MaterialManager::getSingleton().remove(pName);
		Ogre::TextureManager::getSingleton().remove(pName);


		delete iface_factory;

		videoMgr->destroyVideoClip(videoClip);

		delete videoMgr;

		//We set loaded to false, so the player does not do anything when updateVideo(), or unload() are called
		loaded = false;
	}
	else{
		//Nothing to unload!!! We ignore this call
	}

}


void VideoPlayer::printVideoInfo()
{
	cout << "Video info\n__________\n";
	cout << "Video name: " << videoClip->getName() << endl;
	cout << "Duration: " <<videoClip->getDuration() << endl;
	cout << "FPS: " <<videoClip->getFPS() << endl;
	cout << "Video Height: " <<videoClip->getHeight() << endl;
	cout << "Video Width: " <<videoClip->getWidth() << endl;
	cout << "Total number of frames: " << videoClip->getNumFrames() <<"\n\n";

}

void VideoPlayer::update(float time_increase)
{
	if (started)
	{
				// let's wait until the system caches up a few frames on startup
				if (videoClip->getNumReadyFrames() < videoClip->getNumPrecachedFrames()*0.5f)
					return;
				started=0;
	}
	videoMgr->update(time_increase);

}

void VideoPlayer::updateVideo(float updateTime)
{
	if(loaded){
		showVideoFrame();
		update(updateTime);
	}
}

void VideoPlayer::showVideoFrame()
{
	videoFrame=videoClip->getNextFrame();

	if(videoFrame){
		texData=(unsigned char*) videoTexture->getBuffer()->lock(Ogre::HardwareBuffer::HBL_DISCARD);
		videoData=videoFrame->getBuffer();

		memcpy(texData,videoData,buffSize);

		videoTexture->getBuffer()->unlock();
		videoClip->popFrame();

	}
}

void VideoPlayer::createTexture()
{
	videoTexture = Ogre::TextureManager::getSingleton().createManual(pName,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
								Ogre::TEX_TYPE_2D,width,height,1,1,Ogre::PF_X8R8G8B8,Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);


	videoMaterial = Ogre::MaterialManager::getSingleton().create(pName,
										Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	vtUnitState = videoMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("videoTexture");


	vtUnitState->setTextureName(pName,Ogre::TEX_TYPE_2D);

}

void VideoPlayer::play()
{
	videoClip->play();
}


void VideoPlayer::pause()
{
	videoClip->pause();
}


void VideoPlayer::stop()
{
	videoClip->stop();
}




