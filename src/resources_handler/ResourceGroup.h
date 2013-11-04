/*
 * ResourceGroup.h
 *
 *  Created on: Oct 28, 2013
 *      Author: agustin
 */

#ifndef RESOURCEGROUP_H_
#define RESOURCEGROUP_H_

#include <string>
#include <set>
#include <limits>
#include <algorithm>

#include <debug/DebugUtil.h>

namespace rrh {

typedef unsigned short ResourceGroupID;
static const ResourceGroupID INVALID_INDEX = std::numeric_limits<ResourceGroupID>::max();

// forward
//
class ResourceHandler;

class ResourceGroup
{
public:
    typedef std::set<std::string> SectionList;
public:
    ResourceGroup() {};
    ~ResourceGroup() {};

    // @brief Get the id of this resource
    //
    inline ResourceGroupID
    ID(void) const;

    // TODO: for now we will only be able to parse a .cfg file and load all the
    //       resources on it.

    // @brief Set the file path to the resource group or name of the resource
    //        path.
    // @param filePath      The .cfg OgreResource file to be used
    // @return true on success | false otherwise
    //
    inline void
    setOgreResourceFile(const std::string& filePath);
    inline const std::string&
    ogreResourceFile(void) const;

    // @brief Add a Ogre section to load if we want. If no sections are set
    //        to this group then all the sections of the group will be used
    // @param section       The section we want to add
    //
    inline void
    addSection(const std::string& section);
    inline const SectionList&
    sections(void) const;

    // @brief Clear the section list from this resource group
    inline void
    clearSections(void);

private:
    std::string mFilePath;
    SectionList mSections;

    // define the id used only by the handler
    friend class ResourceHandler;
    ResourceGroupID _resourceIndex;
};






////////////////////////////////////////////////////////////////////////////////
// Inline methods
//


inline ResourceGroupID
ResourceGroup::ID(void) const
{
    return _resourceIndex;
}
inline void
ResourceGroup::setOgreResourceFile(const std::string& filePath)
{
    mFilePath = filePath;
}
inline const std::string&
ResourceGroup::ogreResourceFile(void) const
{
    return mFilePath;
}

inline void
ResourceGroup::addSection(const std::string& section)
{
    mSections.insert(section);
}
inline const ResourceGroup::SectionList&
ResourceGroup::sections(void) const
{
    return mSections;
}

inline void
ResourceGroup::clearSections(void)
{
	mSections.clear();
}


} /* namespace rrh */
#endif /* RESOURCEGROUP_H_ */
