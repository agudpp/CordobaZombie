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
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <csignal>
#include <unistd.h>

#include "DebugUtil.h"
#include "SystemLoader.h"
#include "MainStateMachine.h"
#include "MainTFBuilder.h"
#include "Util.h"
#include "LoaderManager.h"
#include "LoaderBuilder.h"
#include "GlobalObjects.h"
#include "MouseCursor.h"
#include "MenuManager.h"

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
 * Function used to handle the sigsevs and all that signals and print the
 * stack trace automatically
 */
void
bt_sighandler(int sig, siginfo_t *info, void *secret)
{

    char pid_buf[30];
    sprintf(pid_buf, "%d", getpid());
    char name_buf[512];
    name_buf[readlink("/proc/self/exe", name_buf, 511)]=0;
    int child_pid = fork();
    if (!child_pid) {
        dup2(STDERR_FILENO,STDOUT_FILENO); // redirect output to stderr
        fprintf(stdout, DEBUG_RED DEBUG_INVERT "Caught signal %d (%s)\33[0m\n", sig,
        		 sig == SIGSEGV ? "SIGSEGV" : (sig == SIGABRT ? "SIGABRT" :
        		(sig == SIGKILL ? "SIGKILL" : (sig == SIGTERM ? "SIGTERM" :
        		(sig == SIGUSR1 ? "SIGUSR1" : "UNKNOWN")))));
        fprintf(stdout,"Stack trace for %s pid=%s\n", name_buf, pid_buf);
        execlp("gdb", "gdb", "--batch", "-n", "-ex", "thread", "-ex", "bt",
        		name_buf, pid_buf, NULL);
        abort(); /* If gdb failed to start */
    } else {
        waitpid(child_pid,NULL,0);
    }
    exit(EXIT_FAILURE);
}
/**
 * Configure signals
 */
static void
configureSignals(void)
{
    /* Install our signal handler */
    struct sigaction sa;

    sa.sa_sigaction = bt_sighandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGKILL, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    /* ... add any other signal here */
}

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
		showErrorMessage("Error caught!!! " + e.info);
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
//	std::auto_ptr<TiXmlDocument> doc(Common::Util::loadXmlDocument(fname.c_str()));
//
//	if(!doc.get()){
//		showErrorMessage("Error trying to parse the xml file " + fname + " "
//				"to use with the MainStateMAchine");
//		return true;
//	}
//
//	MainTransitionFunction *mtf = builder.build(doc->RootElement());

	debugERROR("We've to load the MainTF configuration from XML file\n");

	MainTransitionFunction *mtf = builder.build(0);
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
    Common::GlobalObjects::mouseCursor = new f_e::MouseCursor;
    Common::GlobalObjects::mouseCursor->setVisible(true);
    Common::GlobalObjects::mouseCursor->setWindowDimensions(
            GLOBAL_WINDOW->getWidth(),
            GLOBAL_WINDOW->getHeight());

    // Building Menu Manager
    GLOBAL_MENU_MNGR = new MenuManager;
    GLOBAL_MENU_MNGR->build(GLOBAL_WINDOW->getWidth(),
    						GLOBAL_WINDOW->getHeight(),
    						5,5);
    IMenu::setMenuManager(GLOBAL_MENU_MNGR);

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

    	// configure the signals for stack trace
    	configureSignals();

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


    	// Configure the main state machine.
    	smBuilder.setLoaderManager(&loaderManager);
    	if(!configureMainStateMachine(stateMachine, MAINSTATEMACHINE_FNAME,
    			smBuilder)){
    		return -2;
    	}

    	// Finally start the machine.
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



