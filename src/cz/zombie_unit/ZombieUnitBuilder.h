/*
 * ZombieUnitBuilder.h
 *
 *  Created on: Oct 31, 2013
 *      Author: agustin
 */

#ifndef ZOMBIEUNITBUILDER_H_
#define ZOMBIEUNITBUILDER_H_

#include <string>
#include <memory>

#include <tinyxml/tinyxml.h>

#include "RagDollQueue.h"


// forward
//
namespace physics {
class DynamicWorld;
}

namespace cz {

// forward
//
struct BodyPartElement;
class BodyPartQueue;
class ZombieUnit;

class ZombieUnitBuilder
{
public:
    ZombieUnitBuilder();
    ~ZombieUnitBuilder();

    // @brief Set the Bullet world to be used.
    // @param bdw           The bullet dynamic world instance
    //
    inline void
    setDynamicWorld(physics::DynamicWorld* bdw);

    // @brief Load and parse an xml to be used to load all the data.
    // @param xmlPath       The xml pah
    // @return true on success | false otherwise
    //
    bool
    parseXml(const std::string& xmlPath);

    // @brief Configure a BodyPartQueue using the already set xml. This method
    //        will fill the queue with all the body parts we have in the xml.
    // @param queue         The queue to be filled
    // @return true on success | false otherwise
    //
    bool
    fillBodyPartQueue(BodyPartQueue& queue);

    // @brief Fill a Ragdoll queue using the already loaded xml.
    // @param queue         The RagDollQueue to be filled
    // @param zombieModel   The zombie model to be used for all the ragdolls.
    // @return true on success | false otherwise
    //
    bool
    fillRagDollQueue(RagDollQueue<>& queue,
                     const Ogre::Entity* zombieModel);

    // @brief Create a zombie unit from a given id and using the current loaded
    //        xml file.
    // @param id        The zombie id / name.
    // @param zu        The zombie unit to be configured.
    // @return true on success | false otherwise
    // @note that this will not set the queues used by the zombie (RagDollQueue
    //       nor BodyPartQueue)
    //
    bool
    loadZombie(const std::string& id, ZombieUnit& zu);

private:
    // @brief Construct a BodyPartElement from an xml element
    //        This method will also build a BulletObject (bodyPart) from a mesh,
    //        we will use only boxes
    //        to represent this bodyparts.
    // @param xmlElement        The BodyPart xml element
    // @param bpe               The body part element
    // @return true on success | false otherwise
    //
    bool
    buildBodyPartElement(const TiXmlElement* xmlElement,
                         BodyPartElement& bpe) const;

    // @brief Parse and configure a zombie unit from an xml element
    // @param xmlElement        The xml element.
    // @param zu                The zombie unit to be configured
    // @return true on success | false otherwise
    //
    bool
    configureZombieUnit(const TiXmlElement* xmlElement, ZombieUnit& zu);


private:
    std::unique_ptr<TiXmlDocument> mDocument;
    physics::DynamicWorld* mDynamicWorld;
};





////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
ZombieUnitBuilder::setDynamicWorld(physics::DynamicWorld* bdw)
{
    mDynamicWorld = bdw;
}

} /* namespace cz */
#endif /* ZOMBIEUNITBUILDER_H_ */
