/*
 * ResourcesSystemLoader.h
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#ifndef RESOURCESSYSTEMLOADER_H_
#define RESOURCESSYSTEMLOADER_H_

#include <ResourceHandler.h>

#include "IModuleLoader.h"

namespace engine {

class ResourcesSystemLoader : public IModuleLoader
{
public:
    // We need the resource handler instance
    ResourcesSystemLoader(rrh::ResourceHandler*& rh);
    virtual
    ~ResourcesSystemLoader();


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
    rrh::ResourceHandler*& mResourceHandler;
};

} /* namespace engine */
#endif /* RESOURCESSYSTEMLOADER_H_ */
