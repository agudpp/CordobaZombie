/*
 * VideoSystemLoader.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#include "VideoSystemLoader.h"

#include <debug/DebugUtil.h>

namespace engine {

VideoSystemLoader::VideoSystemLoader(mm::OgreVideoPlayer*& videoPlayer,
                                     Ogre::SceneManager*& sceneMngr,
                                     Ogre::RenderWindow*& renderWindow,
                                     mm::OpenALHandler*& openalHandler) :
    IModuleLoader("VideoSystemLoader")
,   mVideoPlayer(videoPlayer)
,   mSceneManager(sceneMngr)
,   mRenderWindow(renderWindow)
,   mOpenalHandler(openalHandler)
{

}

VideoSystemLoader::~VideoSystemLoader()
{
}

////////////////////////////////////////////////////////////////////////////
// Inherited methods
//

////////////////////////////////////////////////////////////////////////////
bool
VideoSystemLoader::load(const EngineConfiguration& config)
{
    if (mVideoPlayer != 0) {
        debugWARNING("Video player instance already created\n");
        return true;
    }

    // build the instance
    debugERROR("TODO: here we need to build the instance and configure it using "
        "whatever we need from the config (we need to add these information in"
        " the config file)\n"
        "Use the openal handler here to be set to the OgreVideoPlayer\n");
    mVideoPlayer = new mm::OgreVideoPlayer(-1, 1, 1, -1,
                                           mSceneManager,
                                           mRenderWindow->getHeight(),
                                           mRenderWindow->getWidth());
    mVideoPlayer->setALHandler(mOpenalHandler);
    mVideoPlayer->setVisible(false);

    return true;
}

////////////////////////////////////////////////////////////////////////////
bool
VideoSystemLoader::unload(void)
{
    debugERROR("TODO: destroy the instance and uninitialize whatever we need"
        " (not the opeanl handler)\n");

    // after destroying reset the pointer.
    delete mVideoPlayer;
    mVideoPlayer = 0;

    return true;
}

} /* namespace engine */
