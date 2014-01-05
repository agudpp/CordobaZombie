/*
 * ResourceHandler.h
 *
 *  Created on: Oct 28, 2013
 *      Author: agustin
 */

#ifndef RESOURCEHANDLER_H_
#define RESOURCEHANDLER_H_

#include <vector>
#include <string>

#include "ResourceGroup.h"

namespace rrh {

class ResourceHandler
{
public:
    ResourceHandler();
    // @brief Construct, and set "resourcePath" as path to the resources dir
    //
    ResourceHandler(const std::string& resourcePath);
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
    getResourcePath(const std::string& resourceGroup,
                    const std::string& resourceName,
                    std::string& resourcePath);
    static bool
    getResourcePathSomeGroup(const std::string& resourceName,
                             std::string& resourcePath);


    // @brief Set the path to the resources folder.
    void
    setResourceRootPath(const std::string& resourcePath);
    inline const std::string&
    getResourceRootPath(void) const;


    // @brief Get the concatenated string between the resources folder path
    //        and the resource relative path resourceRelPath.
    // @param resourceRelPath 	The resource relative path inside the resource
    //							folder.
    // @param fullPath			The resulting concatenation.
    // @return true on success | false otherwise
    inline bool
    getResourceFullPath(const std::string &resourceRelPath,
                        std::string &fullPath) const;

private:

    std::string mResRootPath;
};



////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//


////////////////////////////////////////////////////////////////////////////////
inline const std::string &
ResourceHandler::getResourceRootPath(void) const
{
    return mResRootPath;
}

////////////////////////////////////////////////////////////////////////////////
inline bool
ResourceHandler::getResourceFullPath(const std::string &resourceRelPath,
                                     std::string &fullPath) const
{
    fullPath = mResRootPath;
    fullPath += resourceRelPath;
    return true;
}


} /* namespace rrh */
#endif /* RESOURCEHANDLER_H_ */
