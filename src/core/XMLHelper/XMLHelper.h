/*
 * XMLHelper.h
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */

#ifndef XMLHelper_H_
#define XMLHelper_H_

#include <OgreString.h>
#include <OgreStringConverter.h>

#include <vector>

#include "DebugUtil.h"
#include "Util.h"
#include "tinyxml.h"

class TiXmlElement;
class TiXmlDocument;

class XMLHelper
{
public:
    XMLHelper();
    ~XMLHelper();

    /**
     * Set the xml filename
     */
    void
    setFilename(const Ogre::String &name);

    /**
     * @brief	View the currently set XML filename.
     * @return	The currently set XML filename ("" if nothing set)
     */
    inline Ogre::String
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
     * Find an element (with attribute name) in the first nested elements
     * (starting from rootElement->getFirstChildElement())
     * @param	name		The element's name we're looking for
     * @param	attrName	The attribute name (usually "name")
     * @return	0			On error, or the element if we find it
     */
    const TiXmlElement *
    findElement(const char *name, const char *attrName = "name") const;

    /**
     * Returns all the first childs element of the root element
     */
    void
    getFirstElements(std::vector<const TiXmlElement *> &elements) const;

    /**
     * Returns the root element of the xml
     */
    const TiXmlElement *
    getRootElement(void) const;

    ////////////////////////////////////////////////////////////////////////////
    ////						Parse Static Functions						////
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Parse functions
     */
    static inline void
    parseInt(const TiXmlElement *elem, const char *attrName, int &result);
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
    findChild(const TiXmlElement *root, const char *name, const char *atName =
        "name");

    /* Loads an XmlDocument from a filename.
     * Requires:
     *  @filename       The xml filename
     * Returns:
     *  @doc            The xmlDocument or 0 if error
     * Note: This function allocates a xmlDocument. you have to remove after
     * using it
     */
    static TiXmlDocument*
    loadXmlDocument(const char *fname);

private:
    Ogre::String mFileName;
    TiXmlDocument *mDocument;

};

/******************************************************************************/
/****************************     INLINES     *********************************/

////////////////////////////////////////////////////////////////////////////////
inline Ogre::String
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
    ASSERT(elem);
    ASSERT(attrName);
#ifdef DEBUG
    if (!elem->Attribute(attrName)) {
        debugWARNING("Attribute %s not exists in %s\n",
            attrName, elem->Value());
        ASSERT(false);
    }
#endif
    result = Ogre::StringConverter::parseInt(elem->Attribute(attrName));
}

inline void
XMLHelper::parseFloat(const TiXmlElement *elem, const char *attrName,
    float &result)
{
    ASSERT(elem);
    ASSERT(attrName);
#ifdef DEBUG
    if (!elem->Attribute(attrName)) {
        debugWARNING("Attribute %s not exists in %s\n",
            attrName, elem->Value());
        ASSERT(false);
    }
#endif
    result = Ogre::StringConverter::parseReal(elem->Attribute(attrName));
}

inline void
XMLHelper::parseBool(const TiXmlElement *elem, const char *attrName,
    bool &result)
{
    ASSERT(elem);
    ASSERT(attrName);
#ifdef DEBUG
    if (!elem->Attribute(attrName)) {
        debugWARNING("Attribute %s not exists in %s\n",
            attrName, elem->Value());
        ASSERT(false);
    }
#endif
    result = Ogre::StringConverter::parseBool(elem->Attribute(attrName));
}

#endif /* XMLHelper_H_ */
