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





////////////////////////////////////////////////////////////////////////////////
void LoadingState::Updater::operator()(Loader *l)
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
void LoadingState::Updater::setLoadingBar(LoadingBar *b)
{
	ASSERT(b);
	mBar = b;
	mAccum = 0;
	mTimeStamp = 0;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void LoadingState::showBackground(const Ogre::String &overlayName)
{
	Ogre::OverlayManager &om = Ogre::OverlayManager::getSingleton();

	ASSERT(!mBackground);
	mBackground = om.getByName(overlayName);
	if(!mBackground){
		debugRED("Error loading the overlay %s\n", overlayName.c_str());
		return;
	}

	mBackground->show();
}

////////////////////////////////////////////////////////////////////////////////
void LoadingState::destroyBackground(void)
{
	if(!mBackground) return;
	GUIHelper::fullDestroyOverlay(mBackground);
}

////////////////////////////////////////////////////////////////////////////////
void LoadingState::configureLoaderManager(const MainMachineInfo &info)
{
	ASSERT(mLoaderManager);

	// here we have to retrieve the LevelInfo.xml file
	MainMachineParams::const_iterator it = info.params.find("LEVEL_INFO_FILE");
	ASSERT(it != info.params.end());
	Ogre::String fileName = it->second;
	ASSERT(!fileName.empty());

	delete mDoc; mDoc = 0;
	mDoc = new TiXmlDocument(fileName.c_str());
	if(!mDoc){
		debugRED("Error loading Document %s\n", fileName.c_str());
		ASSERT(false);
		return;
	}
	mLoaderManager->addElement(mDoc->RootElement());

	// get the max value
	float max = mLoaderManager->getSumOfWeights();
	mLoadingBar.setMaximumValue(max + 0.1f);
	mUpdater.setLoadingBar(&mLoadingBar);
	mLoaderManager->setCallback(&mUpdater);
}

////////////////////////////////////////////////////////////////////////////////
void LoadingState::configureLoadingBar(const Ogre::String &overlayName)
{
	ASSERT(mLoaderManager);
	debugGREEN("Loading OverlayBar: %s\n", overlayName.c_str());
	mLoadingBar.clear();
	mLoadingBar.setOverlayName(overlayName);


}

////////////////////////////////////////////////////////////////////////////////
void LoadingState::unloadLoadingBar(void)
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
void LoadingState::setLoaderManager(LoaderManager *lm)
{
	ASSERT(lm);
	mLoaderManager = lm;
}

/**
 * Entering the state with additional info
 */
void LoadingState::enter(const MainMachineInfo &info)
{
	debugRED("ENTERING STATE\n");


	// show the background and loadingbar
	std::string backOverlay = info.params.at("LOADING_BACKGROUND");
	ASSERT(!backOverlay.empty());
	std::string loadingBar = info.params.at("LOADING_BAR");
	ASSERT(!loadingBar.empty());

	configureLoaderManager(info);
	showBackground(backOverlay);
	configureLoadingBar(loadingBar);
}

/**
 * Update the state... This function will be called once.
 * @param	info	The structure used to pass throw the states. If we want
 * 					to fill it with some information then we can do it.
 * @return	event	The event that was happend.
 */
MainMachineEvent LoadingState::update(MainMachineInfo &info)
{
	debugRED("Por ahora estamos haciendo esto solo para debug, aca tenemos "
			"que salir solamente cuando se termina de cargar el nivel "
			"completamente sin posibilidad de salir antes (cancelar el cargado)"
			"\n");

	Ogre::Timer timer;
	float timeStamp = 0;
	MainMachineEvent result = MME_DONE;

	float count = 0;
	float press = false;

	// here we load the level... note that the RenderOneFrame operation is in
	// the "Updater" class... Here we only call the LoadingManager and
	// wait their callbacks call to update the windows
	debugERROR("Tenemos que verificar esto si esta funcionando bien\n");
	int err = mLoaderManager->load();


	// here is the main loop
	// TODO: fix the FrameElapsedTime and check how to get the ogres one.
//	while(true) {
//		timeStamp = timer.getMilliseconds();
//
//		// TODO: solo chequeamos escape aca pero no va...
//		GLOBAL_KEYBOARD->capture();
//		if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_SPACE)){
//			if(!press){
//				press = true;
//			}
//		} else {press = false;}
//
//
//		if(GLOBAL_KEYBOARD->isKeyDown(OIS::KC_ESCAPE)){
//			break;
//		}
//		mLoadingBar.setState(count);
//		count += 0.1f;
//		if(count >= 100.0f){count = 0;}
//
//		// render the frame
//		if(!GLOBAL_ROOT->renderOneFrame()){
//			result = -1; //TODO: poner un erro real aca
//			break;
//		}
//
//		// This must be called when we use the renderOneFrame approach
//		Ogre::WindowEventUtilities::messagePump();
//
//		Common::GlobalObjects::lastTimeFrame = (timer.getMilliseconds() - timeStamp) * 0.001;
//	}

	return result;
}


/**
 * Function called when the state is not "the actual" anymore
 */
void LoadingState::exit(void)
{
	debugRED("Exit STATE\n");
	destroyBackground();
	unloadLoadingBar();

	// detroy the document
	mLoaderManager->removeAll();
	delete mDoc;
	mDoc = 0;

}


