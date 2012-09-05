/*
 * SystemLoader.h
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */

#ifndef SYSTEMLOADER_H_
#define SYSTEMLOADER_H_


// Ogre
#include <OgreSceneManager.h>
#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OgreRenderWindow.h>

// input
#include <OIS.h>

#include <string>


#include "ConfigFileParser.h"

class SystemLoader
{
public:
	enum {
		NO_ERROR = 0,
	};

	struct ErrorException {
		ErrorException(int e, const std::string &i) :
			code(e),
			info(i)
		{}

		int						code;
		std::string				info;
	};

	// Ogre Info
	struct OgreInfo {
		/* The scene manager that we are using */
		Ogre::SceneManager 	*sceneManager;
		// The OgreRoot
		Ogre::Root			*ogreRoot;
		// Render Window
		Ogre::RenderWindow	*window;
		// Camera used
		Ogre::Camera		*camera;

		OgreInfo() :
			sceneManager(0),
			ogreRoot(0),
			window(0),
			camera(0)
		{}


	};

	// input information
	struct InputInfo {
		OIS::Keyboard 		*keyboard;
		OIS::Mouse 			*mouse;
		OIS::InputManager	*inputManager;

		InputInfo() :
			mouse(0),
			keyboard(0),
			inputManager(0)
		{}
	};

public:
	SystemLoader();
	~SystemLoader();

	/**
	 * Check if all the files are ok (using the MD5SecurityChecker)
	 */
	void checkFilesCorruption(void) throw (ErrorException);

	/**
	 * Set the configuration filename
	 */
	void setConfigFilename(const std::string &fname) throw (ErrorException);

	/**
	 * Load/Start the OgreSystem
	 */
	void loadOgreSystem(void) throw (ErrorException);
	OgreInfo &getOgreInfo(void);

	/**
	 * Load Input system
	 */
	void loadInputSystem(void) throw (ErrorException);
	InputInfo &getInputInfo(void);

	/**
	 * Load SoundSystem
	 */
	void loadSoundSystem(void) throw (ErrorException);

private:

	/**
	 * Ogre Loading functions
	 */
	void ogreLoadRsrcFile(const Ogre::String &path,
			const Ogre::String &p = "")  throw (SystemLoader::ErrorException);
	void ogreLoadResources(void) throw (ErrorException);
	void ogreConfigRenderWindow(void) throw (ErrorException);
	void ogreConfigSceneManager(void) throw (ErrorException);
	void ogreConfigViewportAndCamera(void) throw (ErrorException);

	/**
	 * Destroy functions
	 */
	void destroyOgreSystem(void);
	void destroyInputSystem(void);
	void destroySoundSystem(void);




private:
	ConfigFileParser	mConfigParser;
	OgreInfo			mOgreInfo;
	InputInfo			mInputInfo;

};

#endif /* SYSTEMLOADER_H_ */
