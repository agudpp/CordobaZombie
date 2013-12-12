/*
 * OgreVideoTest.cpp
 *
 *  Created on: October 23, 2013
 *     Company: CordobaZombie
 *      Author: Raul
 */

#include "OgreVideoTest.h"

#include <vector>
#include <string>

#include <OgreResourceManager.h>
#include <OgreFontManager.h>
#include <Ogre.h>

#include <os_utils/OSHelper.h>



/******************************************************************************/
/**********************    LOCAL HELPER FUNCTIONS    **************************/
namespace {

// Construct the mouse input keys we will use
//
static std::vector<input::MouseButtonID>
getMouseButtons(void)
{
	std::vector<input::MouseButtonID> buttons;
	buttons.reserve(2); // just in case :p

	buttons.push_back(input::MouseButtonID::MB_Left);

	return buttons;
}


// Construct the keyboard keys we will use
//
static std::vector<input::KeyCode>
getKeyboardKeys(void)
{
	std::vector<input::KeyCode> buttons;
	buttons.reserve(8);

	buttons.push_back(input::KeyCode::KC_ESCAPE);
	buttons.push_back(input::KeyCode::KC_E);
	buttons.push_back(input::KeyCode::KC_R);
	buttons.push_back(input::KeyCode::KC_P);
	buttons.push_back(input::KeyCode::KC_N);
	buttons.push_back(input::KeyCode::KC_S);
	buttons.push_back(input::KeyCode::KC_A);

	return buttons;
}

}


namespace{

// For finding videos between the resources


const int VIDEO_STATE_LIST_SIZE = 4;

const char* VIDEO_STATE_LIST[VIDEO_STATE_LIST_SIZE] =
			{
                "intro_video.ogg",
                "5seg2.ogg",
                "menu.ogg",
                "gorillaz.mp4"
			};



////////////////////////////////////////////////////////////////////////////////



static int
getResourcePath( Ogre::String resourceGroup
			   , Ogre::String resourceName
			   , Ogre::String &resourcePath
			   )
{

	debugERROR("TODO: change this with the correct module of "
			"resource handling, issue #173\n");

	std::string sNameFullPath;

	/* First find file absolute path */
	Ogre::ResourceGroupManager& resGM =
			Ogre::ResourceGroupManager::getSingleton();
	Ogre::FileInfoListPtr files = resGM.findResourceFileInfo(
			resourceGroup, resourceName);

	if (files.isNull()) {
		debug("%s","Recurso no encontrado.\n");
		return -1;

	} else {
		Ogre::FileInfoList::iterator it;
		for (it = files->begin() ; it < files->end() ; it++) {
			/* Compose file absolute path */
			sNameFullPath.append(it->archive->getName()+"/"+resourceName);
			if (core::OSHelper::fileExists(sNameFullPath.c_str())) {
				break;
			} else {
				sNameFullPath.clear();
			}
		}
		/* Found? */
		if (it == files->end() || sNameFullPath.size() <= 0) {
			debug("%s","Recurso no encontrado.\n");
			return -1;		}
	}

	resourcePath = sNameFullPath;

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
static int
loadVideos(mm::OgreVideoPlayer* ovp)
{

	double end = -1.0;
	double start = 0.0;

	// get next video to play
	for(int i = 0; i< VIDEO_STATE_LIST_SIZE; i++){
		Ogre::String videoPath("");
		if(0 != getResourcePath(Ogre::String("Videos"),
				Ogre::String(VIDEO_STATE_LIST[i]), videoPath)){
			debugERROR("Can't find video %s\n", videoPath.c_str());
			continue;
		}

		if(mm::OgreVideoPlayer::C_ERROR ==
				ovp->queue(videoPath.c_str(), videoPath.c_str(), start, end)
		  )
		{
			debugERROR("Can't load %s at %s :S.\n",
					VIDEO_STATE_LIST[i], videoPath.c_str());
		}

		// make the next video last more.
		end += 10.0;
		// make it start a little bit further
		start += 1.0;
	}

	return 0;

}


}




/******************************************************************************/
/***********************    CLASS IMPLEMENTATION    ***************************/
/******************************************************************************/


namespace tests {


/******************************************************************************/
/**************************** OgreVideoTest CLASS *****************************/
/******************************************************************************/



///////////////////////////////////////////////////////////////////////////////
OgreVideoTest::OgreVideoTest() :
	core::AppTester(mTimeFrame),
	mInputHelper(getMouseButtons(),getKeyboardKeys()),
	mVPlayer(-1,1,1,-1,getSceneManager(),1024,768)

{
	// Load fonts
	loadAdditionalData();


	// Construct a video player and fill its queue with videos.
	loadVideos(&mVPlayer);
	mVPlayer.setRepeatPlayList(true);

	// Start playing (when updated)
	debugERROR("ANTES DE PLAY\n");
	mVPlayer.play();

	// configure the input
	input::Mouse::setMouse(mMouse);
	input::Keyboard::setKeyboard(mKeyboard);
	setUseDefaultInput(false);

	return;
}





///////////////////////////////////////////////////////////////////////////////
OgreVideoTest::~OgreVideoTest()
{
	// TODO Auto-generated destructor stub
}



///////////////////////////////////////////////////////////////////////////////
void
OgreVideoTest::loadAdditionalData(void)
{
	// Ugly way to load all the fonts at the beginning
	Ogre::ResourceManager::ResourceMapIterator iter =
		Ogre::FontManager::getSingleton().getResourceIterator();

	while (iter.hasMoreElements()) { iter.getNext()->load(); }

}



///////////////////////////////////////////////////////////////////////////////
void
OgreVideoTest::update()
{

	// update the input system
	mInputHelper.update();

	if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
		// we have to exit
		mStopRunning = true;
		return;
	}

	if (mInputHelper.isKeyPressed(input::KeyCode::KC_R)){
		mVPlayer.setRepeatVideo(true);
		debug("Repeat video --> true\n");
	}

	if (mInputHelper.isKeyPressed(input::KeyCode::KC_E)){
		mVPlayer.setRepeatVideo(false);
		debug("Repeat video --> false\n");
	}

	if (mInputHelper.isKeyReleased(input::KeyCode::KC_N)){
		mVPlayer.next();
		debug("Play next video\n");
	}

	if (mInputHelper.isKeyPressed(input::KeyCode::KC_S)){
		mVPlayer.stop();
		debug("Stop!\n");
	}

	if (mInputHelper.isKeyPressed(input::KeyCode::KC_P)){
		mVPlayer.play();
		debug("Play!\n");
	}

	if (mInputHelper.isKeyPressed(input::KeyCode::KC_A)){
		// TODO hacer mas testing acá
		mVPlayer.dequeueAll();
		debug("Run automatic test\n");
	}

	// update the video
	if(mm::OgreVideoPlayer::C_ERROR == mVPlayer.update(mTimeFrame) &&
        mVPlayer.isPlaying()){
		debugERROR("O el video esta detenido o algo anda mal aca\n");
	}
}


/* Implement this class */
void
OgreVideoTest::handleKeyboardInput(void)
{}

void
OgreVideoTest::handleMouseInput(void)
{}




} //namespace tests

