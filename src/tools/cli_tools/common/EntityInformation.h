/*
 * EntityInformation.h
 *
 *  Created on: Nov 10, 2013
 *      Author: agustin
 */

#ifndef ENTITYINFORMATION_H_
#define ENTITYINFORMATION_H_

#include <OgreString.h>
#include <OgreStringVector.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>

#include <sstream>

namespace tool {

class EntityInformation
{
public:
    EntityInformation();
    ~EntityInformation();

    // @brief Parse all the entity information we want from a given mesh name.
    // @param entName      The entity name we want to parse
    //
    bool
    parseResource(const Ogre::String& entName, Ogre::SceneManager* sceneMngr);


    // @brief Get all the information from the entity into a stream.
    // @param stream        The stream where we will put the information
    //
    void
    getInformation(std::stringstream& stream) const;

private:
    // @brief Parse the entity bone information (skeleton info)
    //
    void
    parseSkeletonInformation(void);

    // @brief Parse the animation information
    //
    void
    parseAnimInformation(void);

    // @brief Add a string vector information into the stream with a title
    //
    void
    addToStream(const Ogre::String& title,
                const Ogre::StringVector& vec,
                std::stringstream& stream) const;

private:
    Ogre::StringVector mAnimNames;
    Ogre::StringVector mBoneNames;
    Ogre::String mName;
    Ogre::String mResourcePath;
    Ogre::Entity* mEntity;
};

} /* namespace tool */
#endif /* ENTITYINFORMATION_H_ */
