/*
 * XMLResourceParser.h
 *
 *  Created on: Nov 10, 2013
 *      Author: agustin
 */

#ifndef XMLRESOURCEPARSER_H_
#define XMLRESOURCEPARSER_H_

#include <string>
#include <vector>
#include <memory>

#include <OgreSceneManager.h>
#include <OgreEntity.h>
#include <OgreMesh.h>

#include <tinyxml/tinyxml.h>

#ifndef RESOURCES_PATH
#define RESOURCES_PATH          "CZ01_RC_PATH"
#endif

namespace tool {

// @brief This class will be used to parse the resources we need from an xml
//        The definition of the xml file is represented in the XMLResourceSample.xml
//

// Helper classes
//
struct XMLResourceLocation {
    std::string filename;
    std::string location;
};

typedef std::vector<XMLResourceLocation> XMLResourceLocationVec;

enum ResourceType {
    XMLRT_NONE,
    XMLRT_MESH,
};

struct XMLResource {
    ResourceType type;
    std::string resource;

    XMLResource() : type(ResourceType::XMLRT_NONE)
    {}
};
typedef std::vector<XMLResource> XMLResourceVec;

class XMLResourceParser
{
public:
    XMLResourceParser();
    ~XMLResourceParser();

    // @brief Set some information we will need to load different type of resources
    // @param sceneMngr     The scene manager
    //
    inline void
    setSceneManager(Ogre::SceneManager* sceneMngr);

    // @brief Parse a XMLResource file.
    // @param xml       The file to parse
    // @return true on success | false on error
    //
    bool
    parse(const std::string& xml);

    // @brief Returns the list of resource locations parsed
    //
    inline const XMLResourceLocationVec&
    resourceLocations(void) const;

    // @brief Return the list of resources already parsed
    //
    inline const XMLResourceVec&
    resources(void) const;

    // @brief Helper method used to load all the resource files (parsed) using the
    //        associated ResourceHandler instance
    // @return true on success | false otherwise
    //
    bool
    loadResourceLocation(void);

    // @brief Helper methods used to build different type of resources
    //        using a given XMLResource
    // @param resource      The resource we want to load.
    // @param result        the associated resource loaded | 0 on error.
    // @return true on success | false otherwise
    //
    bool
    buildResrouce(const XMLResource& resource, Ogre::Entity*& result) const;

private:
    // @brief Parse the locations
    //
    bool
    parseLocations(const TiXmlElement* xml);

    // @brief Set the correct type of a particular resource
    //
    void
    setType(XMLResource& resource);

    // @brief Parse the resources
    //
    bool
    parseResources(const TiXmlElement* xml);

private:
    XMLResourceLocationVec mLocations;
    XMLResourceVec mResources;
    std::unique_ptr<TiXmlDocument> mXMLDoc;
    Ogre::SceneManager* mSceneMngr;

};








////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
XMLResourceParser::setSceneManager(Ogre::SceneManager* sceneMngr)
{
    mSceneMngr = sceneMngr;
}


inline const XMLResourceLocationVec&
XMLResourceParser::resourceLocations(void) const
{
    return mLocations;
}

inline const XMLResourceVec&
XMLResourceParser::resources(void) const
{
    return mResources;
}

} /* namespace tool */
#endif /* XMLRESOURCEPARSER_H_ */
