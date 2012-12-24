/*
 * LoadingState.cpp
 *
 *  Created on: 23/05/2012
 *      Author: agustin
 */

#include <OgreOverlayManager.h>
#include <OgreWindowEventUtilities.h>
#include <OgreResourceManager.h>

#include "GlobalObjects.h"
#include "DebugUtil.h"
#include "LoadingState.h"
#include "GUIHelper.h"
#include "MouseCursor.h"
#include "Util.h"




const std::string LoadingState::BACKGROUND_NAME = "LoadingStBack";
const std::string LoadingState::LOADING_BAR = "LoadingStBar";



////////////////////////////////////////////////////////////////////////////////
void
LoadingState::Updater::operator()(Loader *l)
{
	ASSERT(mBar);
	ASSERT(l);
	mAccum += l->getWeight();
	mBar->setActualValue(mAccum);

	// update the ogre render queue
	mTimeStamp = mTimer.getMilliseconds();

	// render the frame
	if(!GLOBAL_ROOT->renderOneFrame()){
		debugERROR("Error rendering ogre...\n");
	}

	// This must be called when we use the renderOneFrame approach
	Ogre::WindowEventUtilities::messagePump();

	Common::GlobalObjects::lastTimeFrame = (mTimer.getMilliseconds() - mTimeStamp) * 0.001;
}
////////////////////////////////////////////////////////////////////////////////
void
LoadingState::Updater::setLoadingBar(LoadingBar *b)
{
	ASSERT(b);
	mBar = b;
	mAccum = 0;
	mTimeStamp = 0;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void
LoadingState::showBackground()
{
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();

	mBackground = om.getByName(BACKGROUND_NAME);
	if(!mBackground){
		debugERROR("Error loading the overlay %s\n", BACKGROUND_NAME.c_str());
		return;
	}

	mBackground->show();
}

////////////////////////////////////////////////////////////////////////////////
void
LoadingState::destroyBackground(void)
{
	if(mBackground == 0) return;
	mBackground->hide();
	Ogre::OverlayManager::getSingleton().destroy(mBackground);
	mBackground = 0;
}

////////////////////////////////////////////////////////////////////////////////
void
LoadingState::configureLoaderManager(const std::string &levelPath)
{
	ASSERT(mLoaderManager);

	// here we have to retrieve the LevelInfo.xml file
	Ogre::String fileName = helper::MetaRscManager::getResourcesPath() +
	        levelPath + "LevelInfo.xml";

	delete mDoc; mDoc = 0;
	mDoc = new TiXmlDocument(fileName.c_str());
	if(!mDoc->LoadFile() || mDoc->Error()){
		debugERROR("Error loading Document %s, error: %s\n",
		        fileName.c_str(), mDoc->ErrorDesc());
		ASSERT(false);
		return;
	}

	// first unload all the loaded data
	const int errCode = mLoaderManager->unload();
	if (errCode < 0) {
	    debugERROR("Error unloading data from the loader manager\n");
	    // TODO: What we can do in this case? will be a memory leak!
	    ASSERT(false);
	}

	mLoaderManager->removeAllElements();
	mLoaderManager->addElement(mDoc->RootElement());

	// get the max value
	const float max = mLoaderManager->getSumOfWeights();
	mLoadingBar.setMaximumValue(max + 0.1f);
    debugYELLOW("mBar->setMaximumValue(): %f\n", max + 0.1f);
	mUpdater.setLoadingBar(&mLoadingBar);
	mLoaderManager->setCallback(&mUpdater);
}

////////////////////////////////////////////////////////////////////////////////
void
LoadingState::configureLoadingBar(void)
{
	ASSERT(mLoaderManager);
	debugGREEN("Loading OverlayBar: %s\n", LOADING_BAR.c_str());
	mLoadingBar.clear();
	mLoadingBar.setOverlayName(LOADING_BAR);


}

////////////////////////////////////////////////////////////////////////////////
void
LoadingState::unloadLoadingBar(void)
{
	mLoadingBar.clear();
}





LoadingState::LoadingState() :
IMainState("LoadingState"),
mLoaderManager(0),
mBackground(0),
mDoc(0)
{

}

LoadingState::~LoadingState()
{
	destroyBackground();
	unloadLoadingBar();

	delete mDoc;
}


/**
 * Set the loader manager to use
 */
void
LoadingState::setLoaderManager(LoaderManager *lm)
{
	ASSERT(lm);
	mLoaderManager = lm;
}

void
LoadingState::getResources(ResourcesInfoVec &resourcesList) const
{
    // TODO? probablemente no le digamos nada
}

/**
 * Entering the state with additional info
 */
void
LoadingState::enter(const MainMachineInfo &info)
{
	debugRED("ENTERING STATE\n");

	// get the level path name
	std::string levelPath = info.params.at("LEVEL_PATH");
	ASSERT(!levelPath.empty());

	helper::MetaRscManager &mrm = helper::MetaRscManager::getInstance();
	mRsrcFiles.reserve(2);
	helper::MetaRscManager::FileID fid =
	        mrm.loadResourceFile(levelPath + "Loading/resources.cfg");
	mRsrcFiles.push_back(fid);
	fid = mrm.loadResourceLocation(levelPath, "LevelInfo");
	mRsrcFiles.push_back(fid);

	showBackground();
	configureLoadingBar();
	configureLoaderManager(levelPath);
	GLOBAL_CURSOR->setVisible(false);
}

/**
 * Update the state... This function will be called once.
 * @param	info	The structure used to pass throw the states. If we want
 * 					to fill it with some information then we can do it.
 * @return	event	The event that was happend.
 */
MainMachineEvent
LoadingState::update(MainMachineInfo &info)
{
	MainMachineEvent result = MME_DONE;

	// The idea of creating a new thread here to load the entities and resources
	// it is not possible because Ogre crash when I try to create an entity
	// from other thread
	const int err = mLoaderManager->load();
	if (err < 0) {
	    debugERROR("LoadingManager fails when loading %d\n", err);
	    result = MME_ERROR;
	}

	return result;
}


/**
 * Function called when the state is not "the actual" anymore
 */
void
LoadingState::exit(void)
{
	debugRED("Exit STATE\n");
	destroyBackground();
	unloadLoadingBar();

	// detroy the document
	mLoaderManager->removeAllElements();
	delete mDoc;
	mDoc = 0;

	// unload the resources
	for(size_t size = mRsrcFiles.size(), i = 0; i < size; ++i){
	    helper::MetaRscManager::getInstance().unloadResourceFile(mRsrcFiles[i]);
	}
	mRsrcFiles.clear();
	GLOBAL_CURSOR->setVisible(true);
}


