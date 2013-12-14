/*
 * OpenALLoader.h
 *
 *  Created on: Dec 14, 2013
 *      Author: agustin
 */

#ifndef OPENALLOADER_H_
#define OPENALLOADER_H_

#include <openal_handler/OpenALHandler.h>

#include "IModuleLoader.h"

namespace engine {

class OpenALLoader : public IModuleLoader
{
public:

    // @brief Construct using a reference to the openal handler to be shared
    //        between sound system and video system
    //
    OpenALLoader(mm::OpenALHandler*& openalHandler);
    virtual
    ~OpenALLoader();

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
    mm::OpenALHandler*& mOpenalHandler;
};

} /* namespace engine */
#endif /* OPENALLOADER_H_ */
