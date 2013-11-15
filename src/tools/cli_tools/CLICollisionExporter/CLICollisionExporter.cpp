/*
 * CLICollisionExporter.cpp
 *
 *  Created on: Nov 13, 2013
 *      Author: agustin
 */

#include "CLICollisionExporter.h"

#include <OgreResourceManager.h>
#include <OgreMeshManager.h>

#include <ResourceHandler.h>
#include <os_utils/OSHelper.h>
#include <command_line/CLIPrintDefs.h>
#include <collisions/helpers/CollObjectExporter.h>

// useful define to print if we are in verbose mode
//
#define PRINT(x) if (mArgInfo.verbose) {x;}
#define CONVETION_2D_EXT        "_2D"
#define CONVETION_3D_EXT        "_3D"
#define EXTENSION_COLL_2D       ".2DColl"
#define EXTENSION_COLL_3D       ".3DColl"

namespace tool {


////////////////////////////////////////////////////////////////////////////////
void
CLICollisionExporter::printHelp(void) const
{
    CLI_PRINT(
            "\n-------------------------------------------------\n"
            "Collision Exporter Tool\n"
            "-------------------------------------------------\n"
            "cl_collExporter <resources_path>\n\n"
            "where:\n"
            " <resources_path>:\tThe resource file (xml) containing the locations\n"
            "                  \tand the resources list also. For more information\n"
            "                  \tcheck the wiki (CollisionExporterTool).\n"
            );
    // print some important information
    CLI_PRINT_RED(
            "IMPORTANT: We are hardcoding some things and we are assuming some "
            " others like, all the 2D and 3D representations are comming"
            " from .mesh files\n. "
            "Also we assume the conventions: " CONVETION_2D_EXT " for 2D and "
            CONVETION_3D_EXT " for the 3D files that should be in the same "
            "directory than the resource itself. For more information check"
            " the wiki tool and the Asset Wiki also\n"
            );

    // reset the colour of the console
    CLI_PRINT("\n");
}

////////////////////////////////////////////////////////////////////////////////
bool
CLICollisionExporter::parseArgs(const std::vector<std::string>& args)
{
    // only one arg for now
    if (args.size() != 2) {
        debugERROR("We only support one argument for now\n");
        return false;
    }

    // args[1] == path, args[0] == binary name
    mArgInfo.rscPath = args[1];

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CLICollisionExporter::getResourceNames(std::vector<std::string>& names)
{
    // get all the resources
    const XMLResourceVec& resources = mResourcesParser.resources();
    names.clear();
    names.reserve(128);
    for (const XMLResource& r : resources) {
        if (r.type == ResourceType::XMLRT_MESH) {
            names.push_back(r.resource);
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CLICollisionExporter::getTemporaryInfo(const std::string& rscName,
                                       TempObjectInfo& tmp) const
{
    // first get the resource full path of the resource
    std::string rscPath;
    const Ogre::String& group =
            Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME;
    if (!rrh::ResourceHandler::getResourcePath(group,
                                               rscName,
                                               rscPath)){
        debugERROR("We couldn't get the path of the resource %s,"
                " this probably because you don't load the path where the resource"
                " is located...\n", rscName.c_str());
        return false;
    }

    // now we have the path extract the path and name
    std::string fullName, name, aux, path;
    if (!core::OSHelper::extractFileName(rscPath, fullName) ||
        !core::OSHelper::extractFileExtension(fullName, name, aux) ||
        !core::OSHelper::extractPath(rscPath, path)) {
        debugERROR("Error extracting the name from %s\n", rscPath.c_str());
        return false;
    }

    tmp.name = name;

    // now we have the path and the name we should use here the convention.
    core::OSHelper::addEndPathVar(path);

    // check for 2D coll path
    aux = path + name + CONVETION_2D_EXT ".mesh";
    if (core::OSHelper::fileExists(aux.c_str())) {
        // we have a 2D file to process
        tmp.coll2DPath = aux;
    } else {
        tmp.coll2DPath.clear();
    }

    // check for 3D coll file
    aux = path + name + CONVETION_3D_EXT ".mesh";
    if (core::OSHelper::fileExists(aux.c_str())) {
        // we have a 3D file to process
        tmp.coll3DPath = aux;
    } else {
        tmp.coll3DPath.clear();
    }

    // pring some information that we are
    PRINT(CLI_PRINT_GREEN("Resource %s was parsed correctly\n", rscName.c_str()));

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CLICollisionExporter::processObject2D(const TempObjectInfo& tmp) const
{
    // Proccess the 2D information
    if (tmp.coll2DPath.empty()) {
        PRINT(CLI_PRINT_RED("Resource %s has not 2D collision object to process\n",
                            tmp.name.c_str()));
        return true; // this is not an error...
    }

    // else we have something to process
    PRINT(CLI_PRINT_GREEN("Processing 2D collision information for resource %s,"
            " analyzing file %s\n", tmp.name.c_str(), tmp.coll2DPath.c_str()));

    std::string fullName;
    if (!core::OSHelper::extractFileName(tmp.coll2DPath, fullName)) {
        debugERROR("Error extracting the name of %s\n", tmp.coll2DPath.c_str());
        return false;
    }

    // try to get the associated mesh
    Ogre::MeshPtr mesh;
    const Ogre::String& group =
            Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME;
    try {
        mesh = Ogre::MeshManager::getSingleton().load(fullName, group);
    } catch (Ogre::Exception& e) {
        debugERROR("Error trying to load mesh %s in group %s, with exception %s\n",
                   fullName.c_str(), group.c_str(), e.what());
        return false;
    }

    if (mesh.get() == 0) {
        debugERROR("Error getting the mesh %s\n", fullName.c_str());
        return false;
    }

    // now transform the mesh into a 2DColl file format
    std::string path;
    core::OSHelper::extractPath(tmp.coll2DPath, path);
    core::OSHelper::addEndPathVar(path);
    path += fullName + EXTENSION_COLL_2D;

    return coll::CollObjectExporter::transform(mesh.get(), path);
}
////////////////////////////////////////////////////////////////////////////////
bool
CLICollisionExporter::processObject3D(const TempObjectInfo& tmp) const
{
    // Proccess the 3D information
    if (tmp.coll3DPath.empty()) {
        PRINT(CLI_PRINT_RED("Resource %s has not 3D collision object to process\n",
                            tmp.name.c_str()));
        return true; // this is not an error...
    }

    // else we have something to process
    PRINT(CLI_PRINT_GREEN("Processing 3D collision information for resource %s,"
            " analyzing file %s\n", tmp.name.c_str(), tmp.coll3DPath.c_str()));

    std::string fullName;
    if (!core::OSHelper::extractFileName(tmp.coll3DPath, fullName)) {
        debugERROR("Error extracting the name of %s\n", tmp.coll3DPath.c_str());
        return false;
    }

    // try to get the associated mesh
    Ogre::MeshPtr mesh;
    const Ogre::String& group =
            Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME;
    try {
        mesh = Ogre::MeshManager::getSingleton().load(fullName, group);
    } catch (Ogre::Exception& e) {
        debugERROR("Error trying to load mesh %s in group %s, with exception %s\n",
                   fullName.c_str(), group.c_str(), e.what());
        return false;
    }

    if (mesh.get() == 0) {
        debugERROR("Error getting the mesh %s\n", fullName.c_str());
        return false;
    }

    // now we have to export it into a file
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CLICollisionExporter::CLICollisionExporter()
{
    mResourcesParser.setSceneManager(mSceneMgr);

}

CLICollisionExporter::~CLICollisionExporter()
{

}

////////////////////////////////////////////////////////////////////////////////
int
CLICollisionExporter::execute(const std::vector<std::string>& args)
{
    // parse the arguments
    if (!parseArgs(args)) {
        debugERROR("Error parsing the arguments\n");
        printHelp();
        return -1;
    }

    // parse the resource file using the ResourceParser
    if (!mResourcesParser.parse(mArgInfo.rscPath)) {
        debugERROR("Error when parsing the resources, invalid path probably\n");
        return false;
    }

    // load the resource locations now
    if (!mResourcesParser.loadResourceLocation()) {
        debugERROR("Error loading the resource locations, check if they are valid\n");
        return false;
    }

    // get the resource names
    std::vector<std::string> rscNames;
    if (!getResourceNames(rscNames)) {
        return false;
    }

    // now fill the temporary structure
    TempObjectInfo tmpObj;
    for (std::string& str : rscNames) {
        tmpObj.clear();
        if (getTemporaryInfo(str, tmpObj)) {
            // we will process this object

        }
    }
}

}
