/*
 * OgreVideoTest.cpp
 *
 *  Created on: October 23, 2013
 *     Company: CordobaZombie
 *      Author: Raul
 */


#include <OgreResourceManager.h>
#include <OgreFontManager.h>
#include <vector>
#include <string>
#include <Ogre.h>

#include <OgreVideoPlayer.h>

#include "OgreVideoTest.h"





/* Multiplatform auxiliary function */
#if defined(_WIN32) || defined(CYGWIN)
static inline bool
fileExists(std::string fname)
{
	debugERROR("TODO: change this with the correct module of "
			"resource handling, issue #173\n");
	return System::IO::File::Exists(fname);
}
#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__)
#  include <unistd.h>
static inline bool
fileExists(std::string fname)
{
	debugERROR("TODO: change this with the correct module of "
			"resource handling, issue #173\n");
	return !access(fname.c_str(), R_OK);
}
#else
#  error "Unsupported platform. ABORTING COMPILATION."
#endif







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
	buttons.reserve(2);

	buttons.push_back(input::KeyCode::KC_ESCAPE);
	buttons.push_back(input::KeyCode::KC_E);
	buttons.push_back(input::KeyCode::KC_R);

	return buttons;
}

}


namespace{

// For finding videos between the resources


const int VIDEO_STATE_LIST_SIZE = 3;

const char* VIDEO_STATE_LIST[VIDEO_STATE_LIST_SIZE] =
			{
			"perrosInfectados.ogg",
			"5seg2.ogg",
			"menu.ogg"
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
			if (fileExists(sNameFullPath)) {
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

	// get next video to play
	for(int i = 0; i< VIDEO_STATE_LIST_SIZE; i++){
		Ogre::String videoPath("");
		if(0 != getResourcePath(Ogre::String("Videos"),
				Ogre::String(VIDEO_STATE_LIST[i]), videoPath)){
			debugERROR("Can't find video %s\n", videoPath.c_str());
			continue;
		}

		if(mm::OgreVideoPlayer::ERROR ==
				ovp->queue(videoPath.c_str(), videoPath.c_str(), 0.0, 5.0)
		  )
		{
			debugERROR("Can't load %s at %s :S.\n",
					VIDEO_STATE_LIST[i], videoPath.c_str());
		}
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
	mVPlayer(0)

{
	// Load fonts
	loadAdditionalData();


	// Construct a video player and fill its queue with videos.
	mVPlayer = new mm::OgreVideoPlayer(-1,1,1,-1,getSceneManager(),1024,768);
	loadVideos(mVPlayer);
	mVPlayer->setRepeatPlayList(true);

	// Start playing (when updated)
	debugERROR("ANTES DE PLAY\n");
	mVPlayer->play();

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
	delete mVPlayer;
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
		mVPlayer->setRepeatVideo(true);
		debug("Repeat video --> true\n");
	}

	if (mInputHelper.isKeyPressed(input::KeyCode::KC_E)){
		mVPlayer->setRepeatVideo(false);
		debug("Repeat video --> false\n");
	}

	// update the video
	if(mm::OgreVideoPlayer::ERROR == mVPlayer->update(mTimeFrame)){
		debugERROR("ALGO NO ANDA ACA\n");
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

