/*
 * ResourcesLoadingTest.cpp
 *
 *  Created on: 1/11/2013
 *      Author: raul
 */


#include <memory>
#include <cstring>


#include <ResourceHandler.h>
#include <global_data/GlobalData.h>
#include <os_utils/OSHelper.h>
#include <OgreFontManager.h>


#include "ResourcesLoadingTest.h"


// helper stuff
//
namespace {

// Construct the mouse input keys we will use
//
static std::vector<input::MouseButtonID>
getMouseButtons(void)
{
    std::vector<input::MouseButtonID> buttons;
    buttons.reserve(10); // just in case :p

    buttons.push_back(input::MouseButtonID::MB_Left);
    buttons.push_back(input::MouseButtonID::MB_Right);

    return buttons;
}


// Construct the keyboard keys we will use
//
static std::vector<input::KeyCode>
getKeyboardKeys(void)
{
    std::vector<input::KeyCode> buttons;
    buttons.reserve(18); // just in case :p

    buttons.push_back(input::KeyCode::KC_ESCAPE);
    buttons.push_back(input::KeyCode::KC_L);
    buttons.push_back(input::KeyCode::KC_U);

    return buttons;
}

}


namespace tests {



ResourcesLoading::ResourcesLoading() :
    core::AppTester(cz::GlobalData::lastTimeFrame)
,   mInputHelper(getMouseButtons(), getKeyboardKeys())
{

	debug("\n\nTESTING...\n\n");

	mRrh = new rrh::ResourceHandler();
    mRg = new rrh::ResourceGroup();
    // configure the input
    input::Mouse::setMouse(mMouse);
    input::Keyboard::setKeyboard(mKeyboard);
    setUseDefaultInput(false);

    // configure global data
    cz::GlobalData::sceneMngr = mSceneMgr;
    cz::GlobalData::camera = mCamera;

    Ogre::String recPath = Ogre::String("");

    // Build up the resources group with the big files
    char *ENV = 0;
    ASSERT(core::OSHelper::getEnvVar("CZ01_RC_PATH",ENV));
    mRrh->setResourceRootPath(Ogre::String(ENV));
    ASSERT(mRrh->getResourceFullPath(
    		"test/resources_loading/bigResources.cfg", recPath));
    if(!core::OSHelper::fileExists(recPath.c_str())){
    	debugERROR("You need a bigResources.cfg file in %s, to be able to run "
    			"this test.\n", ENV);
    }

    debug("Setting %s as the resource group file\n", recPath.c_str());

    mRg->setOgreResourceFile(recPath);

}

ResourcesLoading::~ResourcesLoading()
{
    // TODO Auto-generated destructor stub
    delete mRrh;
    delete mRg;
}

/* Load additional info */
void
ResourcesLoading::loadAditionalData(void)
{
    // TODO: ugly way to load all the fonts at the beginning
    Ogre::ResourceManager::ResourceMapIterator iter =
        Ogre::FontManager::getSingleton().getResourceIterator();
    while (iter.hasMoreElements()) { iter.getNext()->load(); }
}


///////////////////////////////////////////////////////////////////////////////

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void
ResourcesLoading::update()
{
    // update the input system
    mInputHelper.update();


    if (mInputHelper.isKeyPressed(input::KeyCode::KC_ESCAPE)) {
        // we have to exit
        mStopRunning = true;
        return;
    }

    if (mInputHelper.isKeyReleased(input::KeyCode::KC_L)) {
        mRrh->loadResourceGroup(*mRg);

        Ogre::OverlayManager* om = Ogre::OverlayManager::getSingletonPtr();

        mOverlay = om->getByName("Test/BigImagesOverlay");
        ASSERT(mOverlay && "No overlay with name Test/BigImagesOverlay"
            " was found");

        mOverlay->show();

        debug("Big resource loaded.\n");
    }


    if (mInputHelper.isKeyReleased(input::KeyCode::KC_U)) {

    	mOverlay->hide();
    	/*
    	 * TODO Issue 224
    	 */
    	//Ogre::OverlayManager::getSingletonPtr()->destroy(mOverlay);
    	mRrh->unloadResourceGroup(*mRg);
        mRg->clearSections();

        debug("Big resource un-loaded.\n");
    }

    

}

}
