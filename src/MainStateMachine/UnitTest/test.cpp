/*
 * test.cpp
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */

#include <iostream>
#include <string>
#include <auto_ptr.h>

#include "DebugUtil.h"
#include "SystemLoader.h"
#include "MainStateMachine.h"
#include "MainTFBuilder.h"
#include "Util.h"
#include "LoaderManager.h"
#include "LoaderBuilder.h"
#include "GlobalObjects.h"
#include "MouseCursor.h"

#include "tinyxml.h"


class TiXmlElement;


// DEFINES
#define MAINSTATEMACHINE_FNAME		"mainStateConfig.xml"


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
		return false;
	}

	return true;
}

/**
 * Function used to load and configure the MainMachineState
 * @param	msm		The MainStateMachine
 * @param	fname	The xml of the machine state configuration
 */
static bool configureMainStateMachine(MainStateMachine &msm,
		const std::string &fname, MainTFBuilder &builder)
{
	std::auto_ptr<TiXmlDocument> doc(Common::Util::loadXmlDocument(fname.c_str()));

	if(!doc.get()){
		showErrorMessage("Error trying to parse the xml file " + fname + " "
				"to use with the MainStateMAchine");
		return true;
	}

	MainTransitionFunction *mtf = builder.build(doc->RootElement());
	ASSERT(mtf);
	msm.setTransitionFunction(mtf);

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




#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
    	SystemLoader 		sysLoader;
    	MainStateMachine	stateMachine;
    	MainTFBuilder		smBuilder;
    	LoaderManager		loaderManager;



    	debugBLUE("TODO: podemos recibir el nombre de archivos de configuracion "
    			"por los argumentos del main, ahora ta hardcodeado\n");

    	// load the system
    	if(!loadSystem(sysLoader)){return -1;}

    	// configure global objects
    	debugRED("TODO: todavia falta setear el LevelManager que es global..\n");
    	configureGlobalObjects(sysLoader);

    	// load the Loaders to be used
    	{
    	    std::vector<Loader*> loaders;
    	    LoaderBuilder::getLoaders(loaders);
    	    for(size_t size = loaders.size(), i = 0; i < size; ++i){
    	        loaderManager.addLoader(loaders[i]);
    	    }
    	}


    	// Configure the main state machine
    	smBuilder.setLoaderManager(&loaderManager);
    	if(!configureMainStateMachine(stateMachine, MAINSTATEMACHINE_FNAME,
    			smBuilder)){
    		return -2;
    	}

    	// Now we start the machine state.
    	debugRED("Configuramos hardcoded el primer main info\n");
    	stateMachine.getInfo().params["LOADING_BACKGROUND"] = "LoadingBackground/Demo";
    	stateMachine.getInfo().params["LOADING_BAR"] = "LoadingBarOverlay/Demo";
    	stateMachine.getInfo().params["LEVEL_PATH"] = "Levels/Demo/";
    	stateMachine.start();


    	loaderManager.deleteAll();

        return 0;
    }

#ifdef __cplusplus
}
#endif



