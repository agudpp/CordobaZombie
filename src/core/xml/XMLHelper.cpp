/*
 * XMLHelper.cpp
 *
 *  Created on: 15/06/2013
 *      Author: agustin
 */

#include "XMLHelper.h"

#include <cstring>

namespace core {

XMLHelper::XMLHelper() :
    mDocument(0)
{

}

XMLHelper::~XMLHelper()
{
    delete mDocument;
}

////////////////////////////////////////////////////////////////////////////////
void
XMLHelper::setFilename(const Ogre::String &name)
{
    mFileName = name;
}

////////////////////////////////////////////////////////////////////////////////
void
XMLHelper::openXml(void)
{
    if (mDocument){
        return;
    }
    mDocument = loadXmlDocument(mFileName.c_str());
    if (mDocument == 0) {
        debugERROR("Error opening the file %s\n", mFileName.c_str());
    }
}

////////////////////////////////////////////////////////////////////////////////
void
XMLHelper::closeXml(void)
{
    delete mDocument;
    mDocument = 0;
}

////////////////////////////////////////////////////////////////////////////////
const TiXmlElement *
XMLHelper::getRootElement(void) const
{
    if (!mDocument)
        return 0;
    return mDocument->RootElement();
}

////////////////////////////////////////////////////////////////////////////
////						Parse Static Functions						////
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
TiXmlDocument*
XMLHelper::loadXmlDocument(const char* filename)
{
    // TODO: use custom allocation instead of new here..
    if (filename == 0) {
        return 0;
    }
    TiXmlDocument* doc = new TiXmlDocument(filename);
    if (doc == 0) {
        debugERROR("Error loading the file %s\n", filename);
        return 0;
    }

    // load
    if (!doc->LoadFile()) {
        debugERROR("Error parsing the file %s\n", filename);
        delete doc;
        return 0;
    }

    // everything goes fine
    return doc;
}

////////////////////////////////////////////////////////////////////////////////
const TiXmlElement *
XMLHelper::findChild(const TiXmlElement *root,
                     const char *name,
                     const char *atName)
{
    // precondition
    ASSERT(root);
    ASSERT(name);
    ASSERT(atName);

    // find it
    const TiXmlElement *first = root->FirstChildElement();
    if (first == 0) {
        debugWARNING("first element couldn't be found for %s root element\n",
            root->Value());
        return 0;
    }

    while (first) {
        const char *value = first->Attribute(atName);
        if (value && std::strcmp(name, value) == 0) {
            return first;
        }
        first = first->NextSiblingElement();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
const TiXmlElement*
XMLHelper::findElement(const TiXmlElement* rootElement,
                       const char *name,
                       const char *attrName)
{
    // precondition
    ASSERT(rootElement);
    ASSERT(name);
    ASSERT(attrName);

    // find it
    const TiXmlElement* first = rootElement->FirstChildElement();
    if (first == 0) {
        debugWARNING("first element couldn't be found for %s root element\n",
            rootElement->Value());
        return 0;
    }

    const char *value = 0;
    while (first) {
        value = first->Attribute(attrName);
        if (value && std::strcmp(name, value) == 0) {
            return first;
        }
        first = first->NextSiblingElement();
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
void
XMLHelper::getFirstElements(const TiXmlElement* rootElement,
                            std::vector<const TiXmlElement *> &elements)
{
    ASSERT(rootElement);

    elements.clear();

    // find it
    const TiXmlElement *first = rootElement->FirstChildElement();
    if (first == 0) {
        debugWARNING("first element couldn't be found for %s root element\n",
            rootElement->Value());
        return;
    }

    while (first) {
        elements.push_back(first);
        first = first->NextSiblingElement();
    }
}


}
