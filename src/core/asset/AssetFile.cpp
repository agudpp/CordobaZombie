/*
 * AssetFile.cpp
 *
 *  Created on: Feb 4, 2014
 *      Author: agustin
 */

#include "AssetFile.h"

#include <debug/DebugUtil.h>
#include <tinyxml/tinyxml.h>
#include <os_utils/OSHelper.h>

#include "Asset.h"
#include "AssetMaterialType.h"

#define ASSETFILE_VERSION "1"


#define CHECK_XML_ATTR(xml, varAttrName, errRetVal) \
    const char* varAttrName = xml->Attribute(#varAttrName);\
    if(varAttrName == 0) {\
        debugERROR("Error parsing the attribute " #varAttrName "\n");\
        return errRetVal;\
    }\

#define CHECK_XML_OPTIONAL_ATTR(xml, varAttrName) \
    const char* varAttrName = xml->Attribute(#varAttrName);\
    if(varAttrName == 0) {\
        debugWARNING("The attribute " #varAttrName " is not present...we will"\
                     " use the default value for it\n");\
    }\

namespace core {

////////////////////////////////////////////////////////////////////////////////
bool
AssetFile::assetToFile(const Asset& asset, const std::string& filename)
{
    // we will create all the different fields of the asset
    //
    TiXmlElement* assetElem = new TiXmlElement("Asset");
    assetElem->SetAttribute("version", ASSETFILE_VERSION);
    assetElem->SetAttribute("meshName", asset.meshName.c_str());

    const char* type = 0;
    if (!assetTypeEnumToStringType(asset.type, type)) {
        debugWARNING("Unknown asset type!: %d\n", asset.type);
    }
    assetElem->SetAttribute("type", type);

    const char* matType = 0;
    if (!assetMaterialEnumTypeToStringType(asset.materialType, matType)) {
        debugWARNING("Unknown material type? %d\n", asset.materialType);
    }
    assetElem->SetAttribute("materialType", matType);

    assetElem->SetAttribute("coll2DRepFile", asset.coll2DRepFile.c_str());
    assetElem->SetAttribute("coll3DRepFile", asset.coll3DRepFile.c_str());

    // now save the document
    //
    TiXmlDocument XMLDoc;
    XMLDoc.LinkEndChild(assetElem);
    return XMLDoc.SaveFile("filename");
}

////////////////////////////////////////////////////////////////////////////////
bool
AssetFile::assetFromFile(const std::string& filename, core::Asset& asset)
{
    // we could get the file path, put it in the result asset
    // but first transform the path of the asset into the correct name
    std::string fname, ext, path, fullFileName;

    if (!OSHelper::extractFileName(filename, fullFileName) ||
        !OSHelper::extractPath(filename, path) ||
        !OSHelper::extractFileExtension(fullFileName, fname, ext)) {
        debugERROR("Error extracting the name of the file: %s\n", filename.c_str());
        return false;
    }

    // use the file path and name to build the real asset file info
    OSHelper::addEndPathVar(path);
    asset.fullAssetPath = path + fname + ".assetInfo";

    // load the asset from a file
    TiXmlDocument XMLDoc;

    // load the file
    if (!XMLDoc.LoadFile(asset.fullAssetPath.c_str())) {
       debugERROR("Error parsing the file %s\n", asset.fullAssetPath.c_str());
       return false;
    }

    ASSERT(XMLDoc.RootElement() != 0);

    // now parse the asset file:

//    <Asset  version="1"
//            name="zombie"
//            meshName="zombie.mesh"
//            type="ASSET_STATIC_WORLD_ELEMENT"
//            materialType="..."
//            coll2DRepFile="zombie.2DColl"
//            coll3DRepFile="zombie.3DColl"
//            />

    const TiXmlElement* root = XMLDoc.RootElement();

    // check the current version
    CHECK_XML_OPTIONAL_ATTR(root, version);
    if (!version) {
        debugWARNING("No version set for asset file %s\n", asset.fullAssetPath.c_str());
    } else if (std::strcmp(version, ASSETFILE_VERSION) != 0) {
        debugWARNING("We are reading asset files with other version (%s) than"
            " the current one (%s)\n", version, ASSETFILE_VERSION);
    }

    // check it is a correct file
    if (!root || std::strcmp(root->Value(), "Asset") != 0) {
        debugERROR("Invalid xml element: %s, expected %s\n", root->Value(), "Asset");
        return false;
    }

    // get and check for all the attributes
    CHECK_XML_ATTR(root, name, false);
    CHECK_XML_ATTR(root, meshName, false);
    CHECK_XML_ATTR(root, type, false);
    CHECK_XML_OPTIONAL_ATTR(root, materialType);
    CHECK_XML_ATTR(root, coll2DRepFile, false);
    CHECK_XML_ATTR(root, coll3DRepFile, false);

    // now set the values to the asset.
    if (strlen(coll2DRepFile) == 0) {
        asset.coll2DRepFile.clear();
    } else {
        asset.coll2DRepFile = path + coll2DRepFile;
    }
    if (strlen(coll3DRepFile) == 0) {
        asset.coll3DRepFile.clear();
    } else {
        asset.coll3DRepFile = path + coll3DRepFile;
    }
    asset.meshName = meshName;
    asset.name = name;
    assetStringTypeToEnumType(type, asset.type);
    if (materialType) {
        assetStringTypeToMaterialEnumType(materialType, asset.materialType);
    }

    return true;
}



} /* namespace core */
