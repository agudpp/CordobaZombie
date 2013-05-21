/*
 * test.cpp
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */


#include <iostream>
#include <string>
#include <auto_ptr.h>
// For stack trace print and signal handling
#include <stdio.h>
#include <stdlib.h>
#include <csignal>
// Ogre
#include <OgreString.h>
#include <OgreConfigFile.h>
// Local
#include <Common/DebugUtil/DebugUtil.h>
#include <MainStateMachine/SystemLoader/SystemLoader.h>
#include <Common/Util/Util.h>
#include <Common/GlobalObjects/GlobalObjects.h>
#include <Utils/MouseCursor/MouseCursor.h>

#include "MeshPlayer.h"

#include "tinyxml.h"


class TiXmlElement;



#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif



/**
 * Print a message for windows an linux
 */
static void showErrorMessage(const std::string &msg)
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	MessageBox( NULL, msg.c_str(), ":(", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
	debugERROR("%s\n", msg.c_str());
#endif
}

/**
 * Function used to load all the main system using a config file
 * @param	configFile	The config file path name.
 * @retun	truen		on success
 * 			false		on error
 */
static bool loadSystem(SystemLoader &loader,
		const std::string &configFile = "system.config")
{
	try{
		loader.setConfigFilename(configFile);
		loader.loadOgreSystem();
		loader.loadInputSystem();
//		loader.loadSoundSystem();
	} catch (SystemLoader::ErrorException &e){
		showErrorMessage("Error caught!!! " + e.info);
		return false;
	}

	return true;
}
/**
 * Configure the GlobalObjects instance to set all the actual configuration
 */
static void configureGlobalObjects(SystemLoader &sl)
{
	// configure ogre
	Common::GlobalObjects::camera = sl.getOgreInfo().camera;
	ASSERT(Common::GlobalObjects::camera);

	Common::GlobalObjects::ogreRoot = sl.getOgreInfo().ogreRoot;
	ASSERT(Common::GlobalObjects::ogreRoot);

	Common::GlobalObjects::sceneManager = sl.getOgreInfo().sceneManager;
	ASSERT(Common::GlobalObjects::sceneManager);

	Common::GlobalObjects::window = sl.getOgreInfo().window;
	ASSERT(Common::GlobalObjects::window);

	// configure input
	Common::GlobalObjects::keyboard = sl.getInputInfo().keyboard;
	ASSERT(Common::GlobalObjects::keyboard);

	Common::GlobalObjects::mouse = sl.getInputInfo().mouse;
	ASSERT(Common::GlobalObjects::mouse);

	// configure the MouseCursor used by all the states
    Common::GlobalObjects::mouseCursor = new MouseCursor;
    Common::GlobalObjects::mouseCursor->setVisible(true);
    Common::GlobalObjects::mouseCursor->setWindowDimensions(
            GLOBAL_WINDOW->getWidth(),
            GLOBAL_WINDOW->getHeight());
}

// Load local resources file
//
void
loadAditionalResourcesFromFile(const Ogre::String &file, const Ogre::String &p)
{
      // Load resource paths from config file
    Ogre::ConfigFile cf;
    try{
        cf.load(file);
    } catch(...){
        debugRED("ERROR: file: %s not found\n", file.c_str());
        return;
    }

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    Ogre::String path = p;

    int last = path.size()-1;
#ifdef _WIN32
    if(last >= 0 && path[last] != '\\'){
            path.append("\\");
    }
#else
    if(last >= 0 && path[last] != '/'){
            path.append("/");
    }
#endif

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = path + i->second;
            debugGREEN("DEBUG: Loading resource: %s -- %s -- %s\n",
                        archName.c_str(),
                        typeName.c_str(),
                        secName.c_str());

            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}



#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char *argv[])
#endif
{
    SystemLoader 		sysLoader;

    debugBLUE("TODO: podemos recibir el nombre de archivos de configuracion "
            "por los argumentos del main, ahora ta hardcodeado\n");

    // load the system
    if (!loadSystem(sysLoader)) {
        return -1;
    }

    // configure global objects
    debugRED("TODO: todavia falta setear el LevelManager que es global..\n");
    configureGlobalObjects(sysLoader);

    debugBLUE("Loading hardcoded resources.cfg in the same folder that the"
        " binary\n");
    loadAditionalResourcesFromFile("resources.cfg", "");

    extras::MeshPlayer meshPlayer;
    meshPlayer.run();


    return 0;
}

#ifdef __cplusplus
}
#endif



