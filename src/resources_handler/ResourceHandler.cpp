/*
 * ResourceHandler.cpp
 *
 *  Created on: Oct 28, 2013
 *      Author: agustin
 */

#include "ResourceHandler.h"

#include <OgreResourceManager.h>
#include <OgreResourceGroupManager.h>
#include <OgreString.h>
#include <OgreConfigFile.h>

#include <debug/DebugUtil.h>
#include <types/StackVector.h>
#include <types/basics.h>
#include <os_utils/OSHelper.h>


// Helper functions
//
namespace {
///////////////////////////////////////////////////////////////////////////////
// @brief Helper method used to get the sections of an Ogre configuration file.
// @param file      The file name (not the path)
// @param p         The path where the file is.
static bool
getRsrcFileSections(const std::string &file,
        core::StackVector<std::string, 512>& sections,
        const std::string &p = "")
{
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	try {
		cf.load(file);
	} catch (...) {
		debugERROR("Resources file not found: %s\n", file.c_str());
		return false;
	}

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	std::string path = p;

	core::OSHelper::addEndPathVar(path);

	std::string secName;
	while (seci.hasMoreElements()) {
		secName = seci.peekNextKey();
		if(!secName.empty()){
			sections.push_back(secName);
		}
		seci.moveNext();
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////

// @brief Helper method used to load a resources config file from ogre.
// @param file      The file name (not the path)
// @param p         The path where the file is.
bool
ogreLoadRsrcFile(const std::string &file,
                 core::StackVector<std::string, 512>& sections,
                 const std::string &p = "")
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    try {
        cf.load(file);
    } catch (...) {
        debugERROR("Resources file not found: %s\n", file.c_str());
        return false;
    }

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    std::string path = p;
    core::OSHelper::addEndPathVar(path);

    std::string secName, typeName, archName;
    while (seci.hasMoreElements()) {
        secName = seci.peekNextKey();
        if(!secName.empty()){
			sections.push_back(secName);
			Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
			Ogre::ConfigFile::SettingsMultiMap::iterator i;
			for (i = settings->begin(); i != settings->end(); ++i) {
				typeName = i->first;
				archName = path + i->second;
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
						archName, typeName, secName);
			}
        }
        seci.moveNext();
    }
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    return true;
}

}



namespace rrh {

ResourceHandler::ResourceHandler():
		mResRootPath("")
{

}

ResourceHandler::~ResourceHandler()
{
}

////////////////////////////////////////////////////////////////////////////////
bool
ResourceHandler::loadResourceGroup(ResourceGroup& rg)
{
    if(rg.ogreResourceFile().empty()){
        debugWARNING("Resource group with empty ogre resource file\n");
        return false;
    }

    // try to load the resource group ogre file
    core::StackVector<std::string, 512> sections;
    ASSERT(rg.sections().empty() && "TODO: we need to change this probably, check"
        " issue 186 already closed. We don't need it now");


    // Get the path to the resource folder
    std::string basePath;
    if (!core::OSHelper::extractPath(rg.ogreResourceFile(), basePath)) {
        debugERROR("Error extracting base path from %s\n",
                   rg.ogreResourceFile().c_str());
        return false;
    }

    if (!ogreLoadRsrcFile(rg.ogreResourceFile(), sections, basePath)) {
        debugERROR("We couldn't load the ogre resource file %s\n",
            rg.ogreResourceFile().c_str());
        return false;
    }

    // now we will load all the group targets and sa
    Ogre::ResourceGroupManager &rscMng = Ogre::ResourceGroupManager::getSingleton();
    for (std::string& sec : sections) {

#ifdef DEBUG
        if (!rscMng.resourceGroupExists(sec)) {
            debugERROR("Can't find section named %s\n", sec.c_str());
            continue;
        }
#endif

    	rscMng.initialiseResourceGroup(sec);
        rscMng.loadResourceGroup(sec);
        rg.addSection(sec);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
ResourceHandler::unloadResourceGroup(const ResourceGroup& rg)
{

    core::StackVector<std::string, 512> sections;
    Ogre::ResourceGroupManager &rscMng = Ogre::ResourceGroupManager::getSingleton();

	if(rg.sections().empty()){
        // Get the path to the resource folder
        const std::string& ogreFile = rg.ogreResourceFile();
        std::string basePath;
        core::OSHelper::extractPath(ogreFile, basePath);

        if (!getRsrcFileSections(ogreFile, sections, basePath)) {
             debugERROR("We couldn't get the sections from %s\n",
                 rg.ogreResourceFile().c_str());
             return;
        }

//TODO somehow remove the duplicated code down here

        for (auto it = sections.begin(), end = sections.end(); it != end; ++it) {
            rscMng.unloadResourceGroup(*it);
//            rscMng.unloadResourceGroup(*it, true);
            rscMng.unloadUnreferencedResourcesInGroup(*it, false);
            rscMng.destroyResourceGroup(*it);
        }
    }else{
        for (auto it = rg.sections().begin(), end = rg.sections().end(); it != end; ++it) {
            rscMng.unloadResourceGroup(*it);
//            rscMng.unloadResourceGroup(*it, true);
            rscMng.unloadUnreferencedResourcesInGroup(*it, false);
            rscMng.destroyResourceGroup(*it);
        }

    }

    if (!rg.ogreResourceFile().empty()) {
        rscMng.removeResourceLocation(rg.ogreResourceFile());
    }

}

////////////////////////////////////////////////////////////////////////////////
bool
ResourceHandler::getResourcePath(const std::string& resourceGroup,
                                 const std::string& resourceName,
                                 std::string &resourcePath)
{
    resourcePath = "";

    // First find file absolute path
    Ogre::ResourceGroupManager& resGM =
                    Ogre::ResourceGroupManager::getSingleton();
    Ogre::FileInfoListPtr files;
    try {
        files= resGM.findResourceFileInfo(resourceGroup, resourceName);
    } catch (Ogre::Exception& e) {
        debugERROR("Exception: %s\n", e.what());
        return false;
    }

    if (files.isNull()) {
        // resource not found
        debug("Resource %s not found\n", resourceName.c_str());
        return false;
    }
    Ogre::FileInfoList::iterator it;
    for (it = files->begin() ; it != files->end() ; it++) {
        // Compose file absolute path
        resourcePath.append(it->archive->getName()+"/"+resourceName);
        if (core::OSHelper::fileExists(resourcePath.c_str())) {
            break;
        } else {
            resourcePath.clear();
        }
    }

    // Found?
    if (it == files->end() || resourcePath.empty()) {
        debug("Resource %s not found\n", resourceName.c_str());
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
ResourceHandler::getResourcePathSomeGroup(const std::string& resourceName,
                                          std::string &resourcePath)
{
    // find the group first
    Ogre::ResourceGroupManager& resGM =
                        Ogre::ResourceGroupManager::getSingleton();
    try {
        const std::string& group = resGM.findGroupContainingResource(resourceName);
        // call the main method
        return getResourcePath(group, resourceName, resourcePath);
    } catch (Ogre::Exception& e) {
        debugERROR("Couldn't find the group for the resource %s, exception %s\n",
                   resourceName.c_str(), e.what());
        return false;
    }

    // this never happens
    ASSERT(false);
    return false;
}


////////////////////////////////////////////////////////////////////////////////
void
ResourceHandler::setResourceRootPath(const std::string &resourcePath)
{
    mResRootPath = resourcePath;
    core::OSHelper::addEndPathVar(mResRootPath);
}

} /* namespace rrh */
