/*
 * SoundSystemLoader.h
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#ifndef SOUNDSYSTEMLOADER_H_
#define SOUNDSYSTEMLOADER_H_

#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>

#include <sound/SoundManager.h>

#include "IModuleLoader.h"

namespace engine {

class SoundSystemLoader : public IModuleLoader
{
public:
    // Pass the pointer we of the manager to be built
    SoundSystemLoader(mm::SoundManager*& soundManager);
    virtual
    ~SoundSystemLoader();

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
    mm::SoundManager*& mSoundManager;
};

} /* namespace engine */
#endif /* SOUNDSYSTEMLOADER_H_ */
