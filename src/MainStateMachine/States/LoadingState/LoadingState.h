/*
 * LoadingState.h
 *
 *  Created on: 23/05/2012
 *      Author: agustin
 */

#ifndef LOADINGSTATE_H_
#define LOADINGSTATE_H_

#include <OgreOverlay.h>
#include <OgreString.h>
#include <OgreTimer.h>

#include <string>

#include "LoaderManager.h"
#include "LoadingBar.h"
#include "IMainState.h"
#include "MetaRscManager.h"

class TiXmlDocument;


class LoadingState : public IMainState
{
    static const std::string BACKGROUND_NAME;
    static const std::string LOADING_BAR;


	struct Updater : public LoaderManager::LoaderCallback {
		void operator()(Loader *l);

		void setLoadingBar(LoadingBar *b);
	private:
		LoadingBar	*mBar;
		float		mAccum;
		Ogre::Timer mTimer;
		float 		mTimeStamp;
	};

public:
	LoadingState();
	virtual ~LoadingState();

	/**
	 * Set the loader manager to use
	 */
	void setLoaderManager(LoaderManager *lm);

	/**
     * Function used to get the resources files used by the state.
     * The list returned is the list of the resources used by and only by this
     * state.
     */
    virtual void getResources(ResourcesInfoVec &resourcesList) const;
	/**
	 * Entering the state with additional info
	 * @note	We get the xml to parse throw the
	 * 			info.params["LOAD_LEVEL_XML_NAME"] to get the filename of the xml
	 */
	virtual void enter(const MainMachineInfo &info);

	/**
	 * Update the state... This function will be called once.
	 * @param	info	The structure used to pass throw the states. If we want
	 * 					to fill it with some information then we can do it.
	 * @return	event	The event that was happend.
	 */
	virtual MainMachineEvent update(MainMachineInfo &info);

	/**
	 * Function called when the state is not "the actual" anymore
	 */
	virtual void exit(void);


protected:
	/**
	 * Show/Destroy background
	 */
	void showBackground(void);
	void destroyBackground(void);

	/**
	 * Load the xml of the level and set it to the LoaderManager
	 */
	void configureLoaderManager(const MainMachineInfo &info);

	/**
	 * Load the Loading bar after the LoaderManager was configured
	 */
	void configureLoadingBar(const Ogre::String &overlayName);
	void unloadLoadingBar(void);

protected:
	LoaderManager		*mLoaderManager;
	LoadingBar			mLoadingBar;
	Ogre::Overlay		*mBackground;
	Updater				mUpdater;
	TiXmlDocument		*mDoc;
	helper::MetaRscManager::FileID mRsrcFile;


};

#endif /* LOADINGSTATE_H_ */
