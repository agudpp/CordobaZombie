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
////////////////////////////////////////////////////////////////////////////////

// @brief Helper method used to load a resources config file from ogre.
// @param file      The file name (not the path)
// @param p         The path where the file is.
bool
ogreLoadRsrcFile(const Ogre::String &file,
                 core::StackVector<Ogre::String, 512>& sections,
                 const Ogre::String &p = "")
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
    Ogre::String path = p;

    int last = path.size() - 1;
#ifdef _WIN32
    if(last >= 0 && path[last] != '\\') {
        path.append("\\");
    }
#else
    if (last >= 0 && path[last] != '/') {
        path.append("/");
    }
#endif

    Ogre::String secName, typeName, archName;
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
        }else{
        	seci.getNext();
        }

    }
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
    core::StackVector<Ogre::String, 512> sections;
    ASSERT(rg.sections().empty() && "TODO: we need to change this probably, check"
        " issue 186 already closed. We don't need it now");


    // Get the path to the resource folder
    core::size_t lastBar = 0;
#ifdef _WIN32
    lastBar = rg.ogreResourceFile().rfind('\\')+1;
#else
    lastBar = rg.ogreResourceFile().rfind('/')+1;
#endif

    ASSERT(lastBar > 1);
    Ogre::String basePath = rg.ogreResourceFile().substr(0,lastBar);

    if (!ogreLoadRsrcFile(rg.ogreResourceFile(), sections, basePath)) {
        debugERROR("We couldn't load the ogre resource file %s\n",
            rg.ogreResourceFile().c_str());
        return false;
    }

    // now we will load all the group targets and sa
    Ogre::ResourceGroupManager &rscMng = Ogre::ResourceGroupManager::getSingleton();

    for (Ogre::String& sec : sections) {

#ifdef DEBUG
    	if(!rscMng.resourceGroupExists(sec)){
    		debugERROR("Can't find section named %s\n", sec.c_str());
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
    if(rg.sections().empty()){
        debugWARNING("Resource group %s has no resources associated (sections)\n",
            rg.ogreResourceFile().c_str());
        return;
    }

    Ogre::ResourceGroupManager &rscMng = Ogre::ResourceGroupManager::getSingleton();
    for (auto it = rg.sections().begin(), end = rg.sections().end(); it != end; ++it) {
        rscMng.unloadResourceGroup(*it);
        rscMng.unloadResourceGroup(*it, false);
        rscMng.destroyResourceGroup(*it);
    }
    if (!rg.ogreResourceFile().empty()) {
        rscMng.removeResourceLocation(rg.ogreResourceFile());
    }

}

////////////////////////////////////////////////////////////////////////////////
bool
ResourceHandler::getResourcePath(const Ogre::String& resourceGroup,
                                 const Ogre::String& resourceName,
                                 Ogre::String &resourcePath)
{
    resourcePath = "";

    // First find file absolute path
    Ogre::ResourceGroupManager& resGM =
                    Ogre::ResourceGroupManager::getSingleton();
    Ogre::FileInfoListPtr files = resGM.findResourceFileInfo(
                    resourceGroup, resourceName);

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
ResourceHandler::getResourceFullPath(const Ogre::String &resourceRelPath,
		Ogre::String &fullPath)
{
	fullPath = mResRootPath;
	fullPath += resourceRelPath;
	return true;
}


} /* namespace rrh */
