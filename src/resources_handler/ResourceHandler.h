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


    // @brief Set the path to the resources folder.
    inline void
    setResourceRootPath(const Ogre::String &resourcePath);
    inline const Ogre::String &
    getResourceRootPath(void);


    // @brief Get the concatenated string between the resources folder path
    //        and the resource relative path resourceRelPath.
    // @param resourceRelPath 	The resource relative path inside the resource
    //							folder.
    // @param fullPath			The resulting concatenation.
    // @return true on success | false otherwise
    bool
    getResourceFullPath(const Ogre::String &resourceRelPath,
                        Ogre::String &fullPath);

private:

    Ogre::String mResRootPath;
};



////////////////////////////////////////////////////////////////////////////////


inline void
ResourceHandler::setResourceRootPath(const Ogre::String &resourcePath)
{
	mResRootPath = resourcePath;
}

////////////////////////////////////////////////////////////////////////////////


inline const Ogre::String &
ResourceHandler::getResourceRootPath(void)
{
	return mResRootPath;
}



} /* namespace rrh */
#endif /* RESOURCEHANDLER_H_ */
