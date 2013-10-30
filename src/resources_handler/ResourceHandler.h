/*
 * ResourceHandler.h
 *
 *  Created on: Oct 28, 2013
 *      Author: agustin
 */

#ifndef RESOURCEHANDLER_H_
#define RESOURCEHANDLER_H_

#include <vector>

#include <OgreString.h>

#include "ResourceGroup.h"

namespace rrh {

class ResourceHandler
{
public:
    ResourceHandler();
    ~ResourceHandler();

    // @brief Add a resource group to be loaded.
    // @param rg        The resource group to be loaded.
    // @return true on success | false otherwise
    // @note that rg will be filled with the associated sections it are not
    //       specified
    //
    bool
    loadResourceGroup(ResourceGroup& rg);

    // @brief Unload a specific (pre-loaded) resource group by its ID.
    // @param id        The resource group ID
    //
    void
    unloadResourceGroup(const ResourceGroup& rg);


    // @brief Get a resource path from a resource group / name.
    // @param resourceGroup     The resource group where the resource is
    // @param resourceName      The resource name we are looking for.
    // @param resourcePath      The resulting path of the resource.
    // @return true on success | false otherwise
    static bool
    getResourcePath(const Ogre::String& resourceGroup,
                    const Ogre::String& resourceName,
                    Ogre::String &resourcePath);

private:
};

} /* namespace rrh */
#endif /* RESOURCEHANDLER_H_ */
