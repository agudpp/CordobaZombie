/*
 * ZombieUnitBuilder.h
 *
 *  Created on: Oct 31, 2013
 *      Author: agustin
 */

#ifndef ZOMBIEUNITBUILDER_H_
#define ZOMBIEUNITBUILDER_H_

#include <string>

namespace cz {

// forward
//
class BodyPartQueue;
class RagDollQueue;
class ZombieUnit;

class ZombieUnitBuilder
{
public:
    ZombieUnitBuilder();
    ~ZombieUnitBuilder();

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
    // @return true on success | false otherwise
    //
    bool
    fillRagDollQueue(RagDollQueue& queue);

    // @brief Create a zombie unit from a given id and using the current loaded
    //        xml file.
    // @param id        The zombie id / name.
    // @param zu        The zombie unit to be configured.
    // @return true on success | false otherwise
    //
    bool
    loadZombie(const std::string& id, ZombieUnit& zu);
};

} /* namespace cz */
#endif /* ZOMBIEUNITBUILDER_H_ */
