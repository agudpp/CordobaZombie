/*
 * IModuleLoader.h
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#ifndef IMODULELOADER_H_
#define IMODULELOADER_H_

#include <string>

#include <EngineConfiguration.h>



// @brief Here we will define the common interface to be used by all the
//        module loaders.
//

namespace engine {

class IModuleLoader
{
public:
    IModuleLoader(const std::string& name) : mName(name) {};
    virtual
    ~IModuleLoader() {};

    // @brief Return the name of the current module.
    //
    inline const std::string&
    name(void) const {return mName;}

    // @brief Load / initialize the module using the current configuration.
    // @param config        The configuration to be used
    // @return true on success | false on error
    //
    virtual bool
    load(const EngineConfiguration& config) = 0;

    // @brief Unload the current module or uninitialize the module.
    // @return true on succes | false otherwise.
    //
    virtual bool
    unload(void) = 0;

private:
    std::string mName;
};

} /* namespace engine */
#endif /* IMODULELOADER_H_ */
