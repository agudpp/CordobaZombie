/*
 * SceneAssetLoader.cpp
 *
 *  Created on: Nov 18, 2013
 *      Author: agustin
 */

#include "SceneAssetLoader.h"

#include <string.h>

#include <unordered_map>

#include <OgreVector3.h>
#include <OgreSceneNode.h>
#include <OgreQuaternion.h>
#include <OgreStringConverter.h>

#include <tinyxml/tinyxml.h>

#include <debug/DebugUtil.h>
#include <xml/XMLHelper.h>
#include <asset/AssetLoader.h>


// Helper methods
//

#define CHECK_XML_ATTR(xml, varAttrName, errRetVal) \
    if (xml == 0) {\
        debugERROR("xml is null, when trying to get attr " #varAttrName "\n");\
        return errRetVal; \
    }\
    const char* varAttrName = xml->Attribute(#varAttrName);\
    if(varAttrName == 0) {\
        debugERROR("Error parsing the attribute " #varAttrName "\n");\
        return errRetVal;\
    }\

namespace {

struct EntInfo {
    std::string name;
    std::string meshFile;
    Ogre::Entity* entity;
};

struct NodeInfo {
    Ogre::SceneNode* node;
    EntInfo entInfo;
};

// @brief Parse a vector from an xml node
//
inline bool
parseVector(const TiXmlElement* xml, Ogre::Vector3& result)
{
    CHECK_XML_ATTR(xml, x, false);
    CHECK_XML_ATTR(xml, y, false);
    CHECK_XML_ATTR(xml, z, false);
    result.x = Ogre::StringConverter::parseReal(x);
    result.y = Ogre::StringConverter::parseReal(y);
    result.z = Ogre::StringConverter::parseReal(z);
    return true;
}

// @brief Parse a quaternion
//
inline bool
parseQuaternion(const TiXmlElement* xml, Ogre::Quaternion& result)
{
    CHECK_XML_ATTR(xml, qx, false);
    CHECK_XML_ATTR(xml, qy, false);
    CHECK_XML_ATTR(xml, qz, false);
    CHECK_XML_ATTR(xml, qw, false);
    result.x = Ogre::StringConverter::parseReal(qx);
    result.y = Ogre::StringConverter::parseReal(qy);
    result.z = Ogre::StringConverter::parseReal(qz);
    result.w = Ogre::StringConverter::parseReal(qw);
    return true;
}

// @brief Parse a scene Node info from an xml element
// @param xml       The xml element pointing to node element
// @param sceneMngr The scene manager used to create the nodes
// @param nodeInfo  The resulting parsed information
// @return true on success | false otherwise
//
bool
parseNode(const TiXmlElement* xml,
          Ogre::SceneManager* sceneMngr,
          NodeInfo& nodeInfo)
{
    ASSERT(xml);
    ASSERT(sceneMngr);
    ASSERT(xml->Value());
    ASSERT(strcmp(xml->Value(), "node") == 0);

    // parse the node information
    {
        CHECK_XML_ATTR(xml, name, false);
        nodeInfo.node = sceneMngr->createSceneNode(name);
    }
    const TiXmlElement* elem = xml->FirstChildElement("position");
    Ogre::Vector3 vec;
    Ogre::Quaternion quat;
    if (!parseVector(elem, vec)) return false;
    nodeInfo.node->setPosition(vec);

    elem = xml->FirstChildElement("scale");
    if (!parseVector(elem, vec)) return false;
    nodeInfo.node->setScale(vec);

    elem = xml->FirstChildElement("rotation");
    if (!parseQuaternion(elem, quat)) return false;
    nodeInfo.node->setOrientation(quat);

    {
        xml = xml->FirstChildElement("entity");
        CHECK_XML_ATTR(xml, name, false);
        CHECK_XML_ATTR(xml, meshFile, false);
        nodeInfo.entInfo.name = name;
        nodeInfo.entInfo.meshFile = meshFile;
        nodeInfo.entInfo.entity = sceneMngr->createEntity(name, meshFile);
    }

    return true;
}

}


namespace cz {

SceneAssetLoader::SceneAssetLoader()
{

}

SceneAssetLoader::~SceneAssetLoader()
{

}

bool
SceneAssetLoader::loadScene(const std::string& scenePath,
                            const std::string& rootRscPath,
                            Ogre::SceneManager* sceneMngr)
{
    ASSERT(sceneMngr);

    // try to load the scene first
    TiXmlDocument doc;
    if (!doc.LoadFile(scenePath.c_str())) {
        debugERROR("Error parsing scene file %s\n", scenePath.c_str());
        return false;
    }

    // let the builders know that we will begin
    for (SceneAssetBuilder* b : mBuilders) {
        b->reset();
    }

    // parse all the scene nodes now
    const TiXmlElement* nodes = doc.RootElement();
    if (strcmp(nodes->Value(), "scene") != 0) {
        debugERROR("Invalid xml file? expected root element to be scene and is %s\n",
            nodes->Value());
        return false;
    }
    nodes = nodes->FirstChildElement("nodes");
    if (nodes == 0) {
        debugERROR("No nodes found\n");
        return false;
    }
    nodes = nodes->FirstChildElement("node");

    // Configure the asset loader
    core::AssetLoader assetLoader;
    assetLoader.setRootDirectory(rootRscPath);
    debugWARNING("We are setting here the root directory instead of using an instance "
        "of the current resource handler, that is probably what we need to do!\n."
        "Also check that we are not setting the group where the resources are "
        "located... this is probably wrong also\n");
    const Ogre::String group = "";

    // we will use a hash to map entName -> asset
    //
    debugOPTIMIZATION("Use a StackHashMap here instead\n");
    std::unordered_map<std::string, core::Asset> nameToAsset;
    core::StackVector<NodeInfo, 512> nodeInfoVec;
    // we support only until 512 elements
    while (nodes != 0) {
        // first parse the node
        nodeInfoVec.resize(nodeInfoVec.size()+1);
        NodeInfo& nodeInfo = nodeInfoVec.back();

        if (!parseNode(nodes, sceneMngr, nodeInfo)) {
            debugERROR("Error parsing the node info! aborting, a lot of mem leaks!!\n");
            return false;
        }

        // now get the associated Asset information if we need
        core::Asset asset;
        auto assetIt = nameToAsset.find(nodeInfo.entInfo.meshFile);
        if (assetIt == nameToAsset.end()) {
            // we need to get the new one
            if (!assetLoader.loadAssetFromOgreResource(nodeInfo.entInfo.meshFile,
                                                       group,
                                                       asset)) {
                debugERROR("Error getting the asset, aborting right now, leaks!!!\n");
                return false;
            }
            nameToAsset[nodeInfo.entInfo.meshFile] = asset;
        }

        // we parse the asset,
        nodes = nodes->NextSiblingElement("node");
    }

    // well, we have everything we need to start to process all the information
    for (NodeInfo& ni : nodeInfoVec) {
        ASSERT(nameToAsset.find(ni.entInfo.meshFile) != nameToAsset.end());
        core::Asset& asset = nameToAsset.find(ni.entInfo.meshFile)->second;
        bool processed = false;

        // for each builder
        for (SceneAssetBuilder* b : mBuilders) {
            if (b->canProcess(asset)) {
                if (!b->build(asset, ni.node, ni.entInfo.entity)) {
                    debugERROR("Error processing one of the assets %s!! LEAKS\n",
                        asset.fullAssetPath.c_str());
                    return false;
                }
                processed = true;
            }
        }

        // check if we could process it
        if (!processed) {
            debugERROR("Asset %s couldn't be processed.... we didn't found a "
                "builder to process it!\n", asset.fullAssetPath.c_str());
        }
    }

    // everything is good
    return true;
}

} /* namespace cz */
