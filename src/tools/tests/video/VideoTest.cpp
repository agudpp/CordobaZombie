/*
 * VideoTest.cpp
 *
 *  Created on: October 3, 2013
 *     Company: CordobaZombie
 *      Author: Monti, Raul E.
 */
#include <OgreResourceManager.h>
#include <OgreFontManager.h>
#include <vector>
#include <string>
#include <Ogre.h>

#include <VideoBuffer.h>
#include <VideoPlayer.h>

#include "VideoTest.h"




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
			"menu_ar3:2.ogg"
			};



////////////////////////////////////////////////////////////////////////////////

int getResourcePath( Ogre::String resourceGroup
				   , Ogre::String resourceName
				   , Ogre::String &resourcePath
				   )
{
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


}




/******************************************************************************/
/***********************    CLASS IMPLEMENTATION    ***************************/
/******************************************************************************/


namespace tests {


/******************************************************************************/
/*************************** OGREVIDEOSCREEN CLASS ****************************/
/******************************************************************************/


///////////////////////////////////////////////////////////////////////////////

OgreVideoScreen::OgreVideoScreen(Ogre::Real left, Ogre::Real top,
								 Ogre::Real right, Ogre::Real bottom,
								 Ogre::SceneManager *SCN_MNGR,
								 unsigned int height,
								 unsigned int width):
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


/******************************************************************************/
/****************************** VIDEOTEST CLASS *******************************/
/******************************************************************************/



///////////////////////////////////////////////////////////////////////////////
VideoTest::VideoTest() :
    core::AppTester(mTimeFrame),
    mInputHelper(getMouseButtons(),getKeyboardKeys()),
    mVPlayer(0)

{
	// Load fonts
	loadAdditionalData();


	// Construct a video player.
	// Video screen with arbitrary default size.
	mVideoScreen = new OgreVideoScreen(-1,1,1,-1,getSceneManager(),1024,768);
	// Construct video player and attach screen.
	mVPlayer = new mm::VideoPlayer(mVideoScreen);


	// Load a video, get its sizes, and resize the screen.
	mVPlayer->load(videolist[0].c_str()) ;
	unsigned int h=0,w=0;
	mVPlayer->getSizes(h,w);
	mVideoScreen->resize(h,w);

	// Start playing (when updated)
	mVPlayer->play();

    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);
    setUseDefaultInput(false);

	return;
}





///////////////////////////////////////////////////////////////////////////////
VideoTest::~VideoTest()
{
    // TODO Auto-generated destructor stub
	delete mVPlayer;
	delete mVideoScreen;
}



///////////////////////////////////////////////////////////////////////////////
void
VideoTest::loadAdditionalData(void)
{
    // Ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();

    while (iter.hasMoreElements()) { iter.getNext()->load(); }

}



///////////////////////////////////////////////////////////////////////////////
void
VideoTest::update()
{
    // update the input system
    mInputHelper.update();

    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
        return;
    }

//    if (mInputHelper.isMousePressed(input::MouseButtonID::MB_Left)){
//    	if( mVPAPI->isPlaying()){
//    		mVPAPI->pause();
//    	}else{
//    		mVPAPI->play();
//    	}
//    }
//
//    if (mInputHelper.isMousePressed(input::MouseButtonID::MB_Right)){
//    	if( mVPAPI->isPlaying()){
//    		mVPAPI->pause();
//    		mVPAPI->unload();
//    		mVPAPI->load(videolist[0]);
//    		mVPAPI->play();
//    	}
//    }


    // update the sound system
    mVPlayer->update(mTimeFrame);
}


/* Implement this class */
void
VideoTest::handleKeyboardInput(void)
{}

void
VideoTest::handleMouseInput(void)
{}




} //namespace tests

