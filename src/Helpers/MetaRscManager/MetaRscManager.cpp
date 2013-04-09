/*
 * MetaRscManager.cpp
 *
 *  Created on: 28/10/2012
 *      Author: agustin
 *
 */

#include "MetaRscManager.h"

#include <OgreString.h>
#include <OgreConfigFile.h>
#include <OgreResourceGroupManager.h>

#include "DebugUtil.h"
#include "Util.h"



// Helper functions
//
namespace {

void
getRscPath(std::string &result)
{
    debugERROR("We are using the hardcoded {ESTRATEGIA2012_RC_PATH}."
            " This will not work in other machines (that install the game from"
            " the installer\n");

    std::string path;
    if (!Common::Util::readEnvVar("ESTRATEGIA2012_RC_PATH", result)) {
        debugERROR("Error reading the environment var %s\n"
                "You have to configure your system... read the mails :)\n",
                "ESTRATEGIA2012_RC_PATH");
        ASSERT(false);
    }
    const int last = result.size() - 1;
#ifdef _WIN32
    if(last >= 0 && result[last] != '\\') {
        result.append("\\");
    }
#else
    if (last >= 0 && result[last] != '/') {
        result.append("/");
    }
#endif
}


////////////////////////////////////////////////////////////////////////////////
bool
ogreLoadRsrcFile(const Ogre::String &file,
                 const Ogre::String &p,
                 std::vector<std::string> &groups)
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

    groups.clear();
    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements()) {
        secName = seci.peekNextKey();

        if (!secName.empty()) groups.push_back(secName);

        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i) {
            typeName = i->first;
            archName = path + i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                    archName, typeName, secName);
        }
    }

    return true;
}

}


namespace helper {

std::string
MetaRscManager::getResourcesPath(void)
{
    std::string result;
    getRscPath(result);
    return result;
}

MetaRscManager::~MetaRscManager()
{
}


////////////////////////////////////////////////////////////////////////////////
MetaRscManager::FileID
MetaRscManager::loadResourceFile(const std::string &filename)
{
    std::string path;
    getRscPath(path);

    RscInfo info;
    info.filename = filename;
    // load all the resources files first
    if (!ogreLoadRsrcFile(path + filename, path, info.groupNames)){
        debugERROR("Error loading resource file: %s\n", filename.c_str());
    } else {
        debugGREEN("Loading Resource file for %s\n:", filename.c_str());
    }

    // now we will load all the group targets
    Ogre::ResourceGroupManager &rscMng = Ogre::ResourceGroupManager::getSingleton();
    for(size_t size = info.groupNames.size(), i = 0; i < size; ++i){
        debugGREEN("Initializing group: %s\n", info.groupNames[i].c_str());
        rscMng.initialiseResourceGroup(info.groupNames[i]);
        rscMng.loadResourceGroup(info.groupNames[i]);
    }

    return getFreeId(info);
}


MetaRscManager::FileID
MetaRscManager::loadResourceLocation(const std::string &filename,
                            const std::string &group,
                            const std::string &type)
{
    Ogre::ResourceGroupManager &rscMng =
                Ogre::ResourceGroupManager::getSingleton();

    std::string path;
    getRscPath(path);
    RscInfo info;
    info.filename = path + filename;
    info.groupNames.push_back(group);
    rscMng.addResourceLocation(info.filename, type, group);
    rscMng.initialiseResourceGroup(group);
    rscMng.loadResourceGroup(group);

    return getFreeId(info);
}

////////////////////////////////////////////////////////////////////////////////
void
MetaRscManager::unloadResourceFile(MetaRscManager::FileID f)
{
    if ((f < 0) || f >= mFiles.size()) {
        ASSERT(false);
        return;
    }

    Ogre::ResourceGroupManager &rscMng =
            Ogre::ResourceGroupManager::getSingleton();
    std::vector<std::string> &vec = mFiles[f].groupNames;
    for (size_t size = vec.size(), i = 0; i < size; ++i) {
            rscMng.unloadResourceGroup(vec[i]);
            rscMng.unloadUnreferencedResourcesInGroup(vec[i], false);
            rscMng.destroyResourceGroup(vec[i]);
    }

    std::string path;
    getRscPath(path);

    // unload all the resources locations
    rscMng.removeResourceLocation(path + mFiles[f].filename);
}

////////////////////////////////////////////////////////////////////////////////
void
MetaRscManager::unloadAll(void)
{
    for (size_t size = mFiles.size(), i = 0; i < size; ++i) {
        unloadResourceFile(static_cast<FileID>(i));
    }
}

}
