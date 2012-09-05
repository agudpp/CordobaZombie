/*
 * test.cpp
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */

#include <iostream>
#include <string>

#include "DebugUtil.h"
#include "SystemLoader.h"



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
	debugRED("%s\n", msg.c_str());
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
		showErrorMessage("Error catched!!! " + e.info);
	}
}


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
    	SystemLoader sysLoader;

    	if(!loadSystem(sysLoader)){
    		return -1;
    	}

        return 0;
    }

#ifdef __cplusplus
}
#endif



