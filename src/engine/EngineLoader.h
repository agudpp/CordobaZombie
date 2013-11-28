/*
 * EngineLoader.h
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#ifndef ENGINELOADER_H_
#define ENGINELOADER_H_

#include <string>

#include <types/StackVector.h>
#include <modules/IModuleLoader.h>

#include "EngineConfiguration.h"


// @brief This class will be used to load all the modules that we add to be loaded.
//

namespace engine {

class EngineLoader
{
    // define and hardcode the number of module loaders that we will be able to
    // handle right now
    //
    static const unsigned int MAX_LOADERS = 6;
public:
    EngineLoader();
    ~EngineLoader();

    // @brief Add a new module to be used when we will load all the engine.
    // @param loader        The module loader.
    // @note that this class is not the owner of the memory of anything
    //
    void
    addModuleLoader(IModuleLoader* loader);

    // @brief Remove all the loaders
    //
    void
    clearLoaders(void);

    // @brief Load all the modules using a given configuration.
    // @param config        The EngineConfiguration to be used.
    // @param verbose       If true we will print all the information we can.
    // @return true if everything goes fine | false if something fails.
    //
    bool
    load(const EngineConfiguration& config, bool verbose = true);

    // @brief Method used to unload all the information from the current loaders
    // @return true on success | false otherwise
    //
    bool
    unload(void);

private:
    core::StackVector<IModuleLoader*, MAX_LOADERS> mLoaders;
};

} /* namespace engine */
#endif /* ENGINELOADER_H_ */
