/*
 * XMLHelper.h
 *
 *  Created on: 15/06/2013
 *      Author: agustin
 */

#ifndef XMLHelper_H_
#define XMLHelper_H_

#include <vector>
#include <string>

#include <OgreStringConverter.h>
#include <tinyxml/tinyxml.h>

#include <debug/DebugUtil.h>



// Forward declaration
//
class TiXmlElement;
class TiXmlDocument;

namespace core {

class XMLHelper
{
public:
    XMLHelper();
    ~XMLHelper();

    /**
     * Set the xml filename
     */
    void
    setFilename(const std::string& name);

    /**
     * @brief	View the currently set XML filename.
     * @return	The currently set XML filename ("" if nothing set)
     */
    inline const std::string&
    getFilename() const;

    /**
     * @brief	Does the instance have a currently open XML file?
     * @return	true if there's an open file attached to the instance
     *			false otherwise
     */
    inline bool
    hasOpenFile() const;

    /**
     * Open/Close the filename
     */
    void
    openXml(void);
    void
    closeXml(void);

    /**
     * Returns the root element of the xml
     */
    const TiXmlElement*
    getRootElement(void) const;

    /**
     * @brief Returns the TiXmlDocument associated, or 0 if we haven't one
     */
    inline TiXmlDocument*
    getDocument(void) const;

    ////////////////////////////////////////////////////////////////////////////
    ////                        Static Functions                            ////
    ////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Load and xml document from a file
     * @param filename  The filename of the xml file (not the full path)
     * @return The TiXmlDocument on success or 0 on error
     */
    static TiXmlDocument*
    loadXmlDocument(const char* filename);

    /**
     * Parse functions using the Ogre framework.
     * TODO: remove ogre dependencies.
     */
    static inline void
    parseInt(const TiXmlElement *elem, const char *attrName, int &result);
    static inline void
    parseUnsignedInt(const TiXmlElement *elem, const char *attrName, unsigned int &result);
    static inline void
    parseFloat(const TiXmlElement *elem, const char *attrName, float &result);
    static inline void
    parseBool(const TiXmlElement *elem, const char *attrName, bool &result);

    /**
     * Find a child element from a top level element
     * @param	root	The root element where we will find the child element
     * @param	name	The name of the element to find (value of the attr)
     * @param	atName	The attribute name ("name" as default)
     * @return	element	The founded element, or 0 if we cannot find it
     */
    static const TiXmlElement *
    findChild(const TiXmlElement *root,
              const char *name,
              const char *atName = "name");

    /**
     * Find an element (with attribute name) in the first nested elements
     * (starting from rootElement->getFirstChildElement())
     * @param   rootElement The root element where we will look for
     * @param   name        The element's name we're looking for
     * @param   attrName    The attribute name (usually "name")
     * @return  0           On error, or the element if we find it
     */
    static const TiXmlElement*
    findElement(const TiXmlElement* rootElement,
                const char *name,
                const char *attrName = "name");

    /**
     * @brief Returns all the first childs element from a root element
     * @param rootElement   The root element from which we want to retrieve the
     *                      elements
     * @param elements      The resulting vector
     */
    static void
    getFirstElements(const TiXmlElement* rootElement,
                     std::vector<const TiXmlElement *> &elements);

private:
    std::string mFileName;
    TiXmlDocument* mDocument;

};

/******************************************************************************/
/****************************     INLINES     *********************************/

////////////////////////////////////////////////////////////////////////////////
inline TiXmlDocument*
XMLHelper::getDocument(void) const
{
    return mDocument;
}

////////////////////////////////////////////////////////////////////////////////
inline const std::string&
XMLHelper::getFilename() const
{
    return mFileName;
}

////////////////////////////////////////////////////////////////////////////////
inline bool
XMLHelper::hasOpenFile() const
{
    return (mDocument != 0);
}

////////////////////////////////////////////////////////////////////////////////
inline void
XMLHelper::parseInt(const TiXmlElement *elem, const char *attrName, int &result)
{
    ASSERT(elem); ASSERT(attrName);
#ifdef DEBUG
    if(!elem->Attribute(attrName)) {
        debugWARNING("Attribute %s not exists in %s\n", attrName, elem->Value());
        ASSERT(false);
    }
#endif
    result = Ogre::StringConverter::parseInt(elem->Attribute(attrName));
}
inline void
XMLHelper::parseUnsignedInt(const TiXmlElement *elem, const char *attrName, unsigned int &result)
{
    ASSERT(elem); ASSERT(attrName);
#ifdef DEBUG
    if(!elem->Attribute(attrName)) {
        debugWARNING("Attribute %s not exists in %s\n", attrName, elem->Value());
        ASSERT(false);
    }
#endif
    result = Ogre::StringConverter::parseUnsignedInt(elem->Attribute(attrName));
}

inline void
XMLHelper::parseFloat(const TiXmlElement *elem, const char *attrName,
    float &result)
{
    ASSERT(elem); ASSERT(attrName);
#ifdef DEBUG
    if(!elem->Attribute(attrName)) {
        debugWARNING("Attribute %s not exists in %s\n", attrName, elem->Value());
        ASSERT(false);
    }
#endif
    result = Ogre::StringConverter::parseReal(elem->Attribute(attrName));
}

inline void
XMLHelper::parseBool(const TiXmlElement *elem, const char *attrName,
    bool &result)
{
    ASSERT(elem); ASSERT(attrName);
#ifdef DEBUG
    if(!elem->Attribute(attrName)) {
        debugWARNING("Attribute %s not exists in %s\n", attrName, elem->Value());
        ASSERT(false);
    }
#endif
    result = Ogre::StringConverter::parseBool(elem->Attribute(attrName));
}

}
#endif /* XMLHelper_H_ */
