/*
 * MainStateMachine.cpp
 *
 *  Created on: 05/06/2012
 *      Author: agustin
 */

#include "MainStateMachine.h"

#include <OgreString.h>
#include <OgreConfigFile.h>
#include <OgreResourceGroupManager.h>

#include "Util.h"
#include "DebugUtil.h"


// Helper functions
//
namespace {
////////////////////////////////////////////////////////////////////////////////
bool ogreLoadRsrcFile(const Ogre::String &file, const Ogre::String &p)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    try {
        cf.load(file);
    } catch (...) {
        debugERROR("Resources file not found: %s\n", file.c_str());
        return false;
    }

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    Ogre::String path = p;

    int last = path.size() - 1;
#ifdef _WIN32
    if(last >= 0 && path[last] != '\\') {
        path.append("\\");
    }
#else
    if (last >= 0 && path[last] != '/') {
        path.append("/");
    }
#endif

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements()) {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i) {
            typeName = i->first;
            archName = path + i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                    archName, typeName, secName);
        }
    }
    return true;
}

}

////////////////////////////////////////////////////////////////////////////////
void
MainStateMachine::loadResources(IMainState *state)
{
    if (state == 0){
        return;
    }

    IMainState::ResourcesInfoVec resources;
    state->getResources(resources);
    if(resources.empty()){
        return;
    }

    debugERROR("We are using the hardcoded {ESTRATEGIA2012_RC_PATH}."
            " This will not work in other machines (that install the game from"
            " the installer\n");

    std::string path;
    if(!Common::Util::readEnvVar("ESTRATEGIA2012_RC_PATH", path)){
        debugERROR("Error reading the environment var %s\n"
                "You have to configure your system... read the mails :)\n",
                "ESTRATEGIA2012_RC_PATH");
        ASSERT(false);
    }
    const int last = path.size()-1;
#ifdef _WIN32
    if(last >= 0 && path[last] != '\\'){
            path.append("\\");
    }
#else
    if(last >= 0 && path[last] != '/'){
            path.append("/");
    }
#endif

    // load all the resources files first
    for(size_t size = resources.size(), i = 0; i < size; ++i){
        if (!ogreLoadRsrcFile(path + resources[i].filePath, path)){
            debugERROR("Error loading resource file [%lu]: %s\n", i,
                    resources[i].filePath.c_str());
        }
        debugGREEN("Loading Resource file for %s state: %s\n:",
                state->getName().c_str(), resources[i].filePath.c_str());
    }

    // now we will load all the group targets
    Ogre::ResourceGroupManager &rscMng = Ogre::ResourceGroupManager::getSingleton();
    for(size_t size = resources.size(), i = 0; i < size; ++i){
        for(size_t size2 = resources[i].groupNames.size(), j = 0; j < size2; ++j){
            rscMng.initialiseResourceGroup(resources[i].groupNames[j]);
            rscMng.loadResourceGroup(resources[i].groupNames[j]);

            debugGREEN("Loading State resources group:\n"
                    "File: %s\nResourceGroup:%s\n",
                    resources[i].filePath.c_str(),
                    resources[i].groupNames[j].c_str());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void
MainStateMachine::unloadResources(IMainState *state)
{
    if (state == 0)
        return;

    IMainState::ResourcesInfoVec resources;
    state->getResources(resources);

    if(resources.empty()){
        return;
    }

    Ogre::ResourceGroupManager &rscMng =
            Ogre::ResourceGroupManager::getSingleton();
    for (size_t size = resources.size(), i = 0; i < size; ++i) {
        for (size_t size2 = resources[i].groupNames.size(), j = 0; j < size2; ++j) {
            rscMng.unloadResourceGroup(resources[i].groupNames[j]);
            rscMng.unloadUnreferencedResourcesInGroup(
                    resources[i].groupNames[j], false);
            rscMng.destroyResourceGroup(resources[i].groupNames[j]);
            debugGREEN("Removing State resources group:\n"
                    "File: %s\nResourceGroup:%s\n",
                    resources[i].filePath.c_str(),
                    resources[i].groupNames[j].c_str());
        }
    }

    debugERROR("We are using the hardcoded {ESTRATEGIA2012_RC_PATH}."
            " This will not work in other machines (that install the game from"
            " the installer\n");

    std::string path;
    if (!Common::Util::readEnvVar("ESTRATEGIA2012_RC_PATH", path)) {
        debugERROR("Error reading the environment var %s\n"
                "You have to configure your system... read the mails :)\n",
                "ESTRATEGIA2012_RC_PATH");
        ASSERT(false);
    }
    const int last = path.size()-1;
#ifdef _WIN32
    if(last >= 0 && path[last] != '\\'){
            path.append("\\");
    }
#else
    if(last >= 0 && path[last] != '/'){
            path.append("/");
    }
#endif

    // unload all the resources locations
    for(size_t size = resources.size(), i = 0; i < size; ++i){
        rscMng.removeResourceLocation(path + resources[i].filePath);
    }

}



MainStateMachine::MainStateMachine() :
mActualState(0),
mLastState(0),
mTransitionFunction(0)
{

}

MainStateMachine::~MainStateMachine()
{

}



/**
 * Set the Transition function.
 * This class DO NOT free any memory
 */
void MainStateMachine::setTransitionFunction(MainTransitionFunction *mtf)
{
	ASSERT(mtf);
	ASSERT(!mTransitionFunction);

	mTransitionFunction = mtf;

}

/**
 * Start the machine
 */
void MainStateMachine::start(void)
{
	ASSERT(mTransitionFunction);
	ASSERT(mTransitionFunction->getStartState());
	ASSERT(mActualState == 0);

	mActualState = mTransitionFunction->getStartState();
	mRunning = true;

	debugColor(DEBUG_BROWN, "Starting MACHINE!\n");

	while(mRunning){
		//starting state
		debugColor(DEBUG_BROWN, "Entering new state: %s\n",
				mActualState->getName().c_str());

		// load the resources of the state
		loadResources(mActualState);
		mActualState->enter(mInfo);

		// updating state
		debugColor(DEBUG_BROWN, "Updating state: %s\n",
				mActualState->getName().c_str());

		mLastEvent = mActualState->update(mInfo);
		IMainState *newState = mTransitionFunction->getNextState(mActualState,
				mLastEvent);

		// changing state
		debugColor(DEBUG_BROWN, "Exiting old state: %s\nNew event %d received\n",
				mActualState->getName().c_str(), mLastEvent);
		ASSERT(newState);
		mLastState = mActualState;
		mActualState = newState;
		mLastState->exit();
		unloadResources(mLastState);

	}

}

/**
 * Stop the machine / reset
 */
void MainStateMachine::stop(void)
{
	mRunning = false;
}
void MainStateMachine::reset(void)
{
	ASSERT(mTransitionFunction);
	ASSERT(mTransitionFunction->getStartState());

	mLastEvent = MME_DONE;
	mActualState = mLastState = 0;
}


MainMachineInfo &MainStateMachine::getInfo(void)
{
	return mInfo;
}
