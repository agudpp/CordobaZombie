/*
 * SystemLoader.cpp
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */

#include <OgreString.h>
#include <OgreConfigFile.h>

#include "DebugUtil.h"
#include "SystemLoader.h"
#include "Util.h"




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


SystemLoader::SystemLoader()
{

}

SystemLoader::~SystemLoader()
{
	destroyInputSystem();
	destroySoundSystem();
	destroyOgreSystem();
}

////////////////////////////////////////////////////////////////////////////////
void SystemLoader::checkFilesCorruption(void) throw (SystemLoader::ErrorException)
{
	throw ErrorException(-1, "Not implemented");
}

////////////////////////////////////////////////////////////////////////////////
void SystemLoader::setConfigFilename(const std::string &fname)
	throw (SystemLoader::ErrorException)
{
	if(!mConfigParser.parseFile(fname)){
		throw ErrorException(-1, "Error parsing the config file " + fname);
	}

	debugGREEN("ConfigFile %s parsed correctly: \n", fname.c_str());

#ifdef DEBUG
	mConfigParser.print();
#endif
}

////////////////////////////////////////////////////////////////////////////////
////							OGRE FUNCTIONS								////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
void SystemLoader::ogreLoadRsrcFile(const Ogre::String &file,
		const Ogre::String &p) throw (SystemLoader::ErrorException)
{
	  // Load resource paths from config file
	Ogre::ConfigFile cf;
	try{
		cf.load(file);
	} catch(...){
		throw ErrorException(-6, "Resources file not found" + file);
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
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

////////////////////////////////////////////////////////////////////////////////
void SystemLoader::ogreLoadResources(void) throw (SystemLoader::ErrorException)
{
	// check if we have file to load
	if(mConfigParser.hasKey("RESOURCES_FILES")){
		std::list<std::string> rfiles;
		mConfigParser.getValue("RESOURCES_FILES", rfiles);
		for(std::list<std::string>::iterator it = rfiles.begin();
				it != rfiles.end(); ++it){
			ogreLoadRsrcFile(*it);
		}
	}

	debugRED("Pro ahora vamos a cargar manualmente de esta forma el archivo "
			"de recursos de dropbox usando la ENV_VAR, esto deberia cambiar "
			"luego\n");
	// Load the aditional resources.
	std::string path;
	if(!Common::Util::readEnvVar("ESTRATEGIA2012_RC_PATH", path)){
		debugRED("Error reading the environment var %s\n"
				"You have to configure your system... read the mails :)\n",
				"ESTRATEGIA2012_RC_PATH");
		ASSERT(false);
	}
	// append the resources.cfg
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
	std::string fname = path + "resources.cfg";
	debugGREEN("Trying to parse the resource file %s\n", fname.c_str());
	ogreLoadRsrcFile(fname, path);
}

////////////////////////////////////////////////////////////////////////////////
void SystemLoader::ogreConfigRenderWindow(void) throw (SystemLoader::ErrorException)
{
	debugRED("TODO: Notar que ahora estamos mostrando el ConfigDialog hecho "
				"por ogre, la idea es que mostremos el nuestro o usemos el ogre.cfg"
				" predefinido (ya sea que lo definio el usuario desde alguna app"
				" nuestra por separado\n");
	if(mOgreInfo.ogreRoot->showConfigDialog()) {
		// If returned true, user clicked OK so initialize
		// Here we choose to let the system create a default rendering window by
		// passing 'true'
		mOgreInfo.window = mOgreInfo.ogreRoot->initialise(true, "Cordoba Zombie");
	} else {
		throw ErrorException(-3, "Not configuration was set, we have to change this"
				". Read the debugRED() message");
	}

}

////////////////////////////////////////////////////////////////////////////////
void SystemLoader::ogreConfigSceneManager(void) throw (ErrorException)
{
	// creates a generic sceneManager
	Ogre::SceneType type = Ogre::ST_GENERIC;
	Ogre::String strType;
	mConfigParser.getValue("SCENE_MANAGER_TYPE", strType);

	// select the type
	if(strType == "ST_GENERIC"){
		type = Ogre::ST_GENERIC;
	} else if(strType == "ST_EXTERIOR_CLOSE"){
		type = Ogre::ST_EXTERIOR_CLOSE;
	} else if(strType == "ST_EXTERIOR_FAR"){
		type = Ogre::ST_EXTERIOR_FAR;
	} else if(strType == "ST_EXTERIOR_REAL_FAR"){
		type = Ogre::ST_EXTERIOR_REAL_FAR;
	} else if(strType == "ST_INTERIOR"){
		type = Ogre::ST_INTERIOR;
	} else {
		throw ErrorException(-4, "Type of scene manager is invalid: " + strType);
	}

	mOgreInfo.sceneManager = mOgreInfo.ogreRoot->createSceneManager(type);
	if(!mOgreInfo.sceneManager) {
		throw ErrorException(-5, "Error while creating the SceneManager of type "
				+ strType);
	}

	// create main light
	debugRED("TODO: por ahora vamos a crear la luz principal para tener una luz "
			"la idea es que despues generemos dinamicamente las luces dependiendo"
			" el nivel y esas cosas\n");

	mOgreInfo.sceneManager->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	Ogre::Light* l = mOgreInfo.sceneManager->createLight("MainLight");
	//setup the light
	l->setType(Ogre::Light::LT_DIRECTIONAL);
	l->setDirection(-0.5, -0.5, 0);
}

////////////////////////////////////////////////////////////////////////////////
void SystemLoader::ogreConfigViewportAndCamera(void) throw (ErrorException)
{
	ASSERT(mOgreInfo.sceneManager);

	// creates the sceneNode of the camera
	mOgreInfo.camera = mOgreInfo.sceneManager->createCamera("MainCamera");
	if(!mOgreInfo.camera){
		throw ErrorException(-7, "Error while creating the main camera");
	}

	// set the query flags 0?
	mOgreInfo.camera->setQueryFlags(0);

	debugBLUE("TODO: Estamos configurando por defecto la camara con algunos "
			"parametros por defecto, deberian estar o en el configFile o "
			"cambiarlos donde haga falta\n");

	mOgreInfo.camera->setNearClipDistance(5);

	// creates a default viewport
	ASSERT(mOgreInfo.window);
	Ogre::Viewport* vp = mOgreInfo.window->addViewport(mOgreInfo.camera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
	vp->setAutoUpdated(true);

	// Alter the camera aspect ratio to match the viewport
	mOgreInfo.camera->setAspectRatio(
			Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

}

////////////////////////////////////////////////////////////////////////////////
void SystemLoader::loadOgreSystem(void) throw (SystemLoader::ErrorException)
{
	if(mOgreInfo.ogreRoot){
		throw ErrorException(-1, "OgreSystem already loaded");
	}

	// create the root
	// get the plugins file
	std::string plugins, ogrecfg;
	mConfigParser.getValue("PLUGINS_FILE", plugins);
	mConfigParser.getValue("OGRE_CONFIG", ogrecfg);
	mOgreInfo.ogreRoot = new Ogre::Root(plugins, ogrecfg);
	if(!mOgreInfo.ogreRoot){
		throw ErrorException(-2, "Error creating the OgreRoot with file " + plugins);
	}
	debugGREEN("Ogre Root created\n");

	// config the window
	ogreConfigRenderWindow();
	debugGREEN("Ogre Render Window created\n");

	// create the SceneManager
	ogreConfigSceneManager();
	debugGREEN("Ogre SceneManager created\n");

	// load and initialize the resources
	ogreLoadResources();
	debugGREEN("Ogre Resources loaded\n");

	// create the camera and viewport
	ogreConfigViewportAndCamera();
	debugGREEN("Viewport and camera configured\n");

	// reinit all the resources group
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();


}

////////////////////////////////////////////////////////////////////////////////
SystemLoader::OgreInfo &SystemLoader::getOgreInfo(void)
{
	return mOgreInfo;
}


////////////////////////////////////////////////////////////////////////////////
////							INPUT FUNCTIONS								////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void SystemLoader::loadInputSystem(void) throw (SystemLoader::ErrorException)
{
	// creates OIS Input
	if(mInputInfo.inputManager) {
		throw ErrorException(-10, "The input system is already created");
	}

	size_t hWnd = 0;

	ASSERT(mOgreInfo.window);
	mOgreInfo.window->getCustomAttribute("WINDOW", &hWnd);
	mInputInfo.inputManager = OIS::InputManager::createInputSystem(hWnd);

	// creates the mouse and keyboard objects (we will use unbuffered input
	// system, the second parameter set this)
	mInputInfo.keyboard = static_cast<OIS::Keyboard*>(
			mInputInfo.inputManager->createInputObject(OIS::OISKeyboard, false));

	mInputInfo.mouse = static_cast<OIS::Mouse*>(
			mInputInfo.inputManager->createInputObject(OIS::OISMouse, false));

	ASSERT(mInputInfo.mouse && mInputInfo.keyboard);

	debugBLUE("TODO: No estamos soportando que haya un resize the ventana para "
			"reconfigurar las cosas del motor, por ejemplo, el movimiento del"
			" mouse y esas cosas\n");
	// config the mouse to the screen size
	unsigned int width, height, depth;
	int left, top;
	mOgreInfo.window->getMetrics(width, height, depth, left, top);
	const OIS::MouseState &mouseState = mInputInfo.mouse->getMouseState();
	mouseState.width  = width;
	mouseState.height = height;

	debugGREEN("Input system loaded ok\n");
}

////////////////////////////////////////////////////////////////////////////////
SystemLoader::InputInfo &SystemLoader::getInputInfo(void)
{
	return mInputInfo;
}

////////////////////////////////////////////////////////////////////////////////
//							SOUND SYSTEM									////
////////////////////////////////////////////////////////////////////////////////
void SystemLoader::loadSoundSystem(void) throw (SystemLoader::ErrorException)
{
	throw ErrorException(-1, "Not implemented");
}





////////////////////////////////////////////////////////////////////////////////
//							SYSTEM DESTRUCTORS								////
////////////////////////////////////////////////////////////////////////////////
void SystemLoader::destroyOgreSystem(void)
{
	if(!mOgreInfo.ogreRoot) return;
	mOgreInfo.sceneManager->clearScene();
	mOgreInfo.ogreRoot->destroySceneManager(mOgreInfo.sceneManager);
	delete mOgreInfo.ogreRoot;
}
void SystemLoader::destroyInputSystem(void)
{
	if(!mInputInfo.inputManager) return;
	mInputInfo.inputManager->destroyInputObject(mInputInfo.mouse);
	mInputInfo.inputManager->destroyInputObject(mInputInfo.keyboard);
	OIS::InputManager::destroyInputSystem(mInputInfo.inputManager);
}
void SystemLoader::destroySoundSystem(void)
{
	debugRED("TODO: hacer esto\n");
}


