/*
 * VideoSystemLoader.h
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#ifndef VIDEOSYSTEMLOADER_H_
#define VIDEOSYSTEMLOADER_H_

#include <video/VideoPlayer.h>

#include "IModuleLoader.h"

namespace engine {

class VideoSystemLoader : public IModuleLoader
{
public:
    // Pass the reference to the pointer where we will construct the video player
    VideoSystemLoader(mm::VideoPlayer*& videoPlayer);
    virtual
    ~VideoSystemLoader();

    ////////////////////////////////////////////////////////////////////////////
    // Inherited methods
    //

    // @brief Load / initialize the module using the current configuration.
    // @param config        The configuration to be used
    // @return true on success | false on error
    //
    virtual bool
    load(const EngineConfiguration& config);

    // @brief Unload the current module or uninitialize the module.
    // @return true on succes | false otherwise.
    //
    virtual bool
    unload(void);

private:
    mm::VideoPlayer* mVideoPlayer;
};

} /* namespace engine */
#endif /* VIDEOSYSTEMLOADER_H_ */
