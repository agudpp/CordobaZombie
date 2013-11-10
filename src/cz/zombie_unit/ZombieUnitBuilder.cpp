/*
 * ZombieUnitBuilder.cpp
 *
 *  Created on: Oct 31, 2013
 *      Author: agustin
 */

#include "ZombieUnitBuilder.h"

#include <OgreSceneManager.h>

#include <debug/DebugUtil.h>
#include <global_data/GlobalData.h>
#include <xml/XMLHelper.h>
#include <physics/BulletImporter.h>
#include <physics/DynamicWorld.h>
#include <CZMasksDefines.h>

#include "ZombieUnit.h"
#include "BodyPartElement.h"
#include "BodyPartQueue.h"



// Helper stuff
namespace {

}


namespace cz {

////////////////////////////////////////////////////////////////////////////////
bool
ZombieUnitBuilder::buildBodyPartElement(const TiXmlElement* xmlElement,
                                        BodyPartElement& bpe) const
{
    ASSERT(xmlElement);
    ASSERT(xmlElement->Attribute("meshName"));
    ASSERT(xmlElement->Attribute("type"));
    ASSERT(xmlElement->Attribute("id"));
    ASSERT(xmlElement->Attribute("mass"));

    // get the scene manager
    Ogre::SceneManager* sceneMngr = GlobalData::sceneMngr;
    ASSERT(sceneMngr);

    // get the entity
    Ogre::Entity* ent = sceneMngr->createEntity(xmlElement->Attribute("meshName"));
    ASSERT(ent);

    // now we have to create the bullet object, we can assume that all the
    // elements will be at 0,0,0 position (centered).
    //
    float mass;
    core::XMLHelper::parseFloat(xmlElement, "mass", mass);
    bpe.setBulletObject(physics::BulletImporter::createBox(ent->getBoundingBox(),
                                                           mass,
                                                           false));
    ASSERT(bpe.bulletObject());

    // create the scene node for the entity and attach the object to it
    //
    Ogre::SceneNode* node = sceneMngr->createSceneNode();
    node->attachObject(ent);
    bpe.bulletObject()->motionState.setNode(node);
    bpe.bulletObject()->entity = ent;

    // get the id and the type
    unsigned int aux;
    core::XMLHelper::parseUnsignedInt(xmlElement, "type", aux);
    ASSERT(aux < BodyPartElementType::BPE_COUNT);
    bpe.setBodyPartElementType(BodyPartElementType(aux));
    core::XMLHelper::parseUnsignedInt(xmlElement, "id", aux);
    bpe.setID(aux);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
ZombieUnitBuilder::configureZombieUnit(const TiXmlElement* xmlElement,
                                       ZombieUnit& zu)
{
    ASSERT(xmlElement);
    ASSERT(xmlElement->Attribute("mesh"));
    ASSERT(xmlElement->Attribute("material"));
    ASSERT(xmlElement->Attribute("vel"));
    ASSERT(xmlElement->Attribute("initialLife"));
    ASSERT(xmlElement->Attribute("bodyPartID"));

    // get the scene manager
    Ogre::SceneManager* sceneMngr = GlobalData::sceneMngr;
    ASSERT(sceneMngr);

    // construct the zombie unit
    Ogre::Entity* ent = sceneMngr->createEntity(xmlElement->Attribute("mesh"));
    ASSERT(ent);

    // set the material for this entity
    //
    ent->setMaterialName(xmlElement->Attribute("material"));

    debugWARNING("Here we are configuring the zombie taking into account that it"
            " was bad exported pointing the Y as the \"Up Vector\"\n");
    // we have to rotate the zombie in the X axis to match with the wrong
    // exported axis, we will create an additional scene node and rotate it
    // create the scene node and attach the entity to it
    Ogre::SceneNode* node = sceneMngr->createSceneNode();
//    Ogre::SceneNode* child = node->createChildSceneNode();
//    child->pitch(Ogre::Radian(Ogre::Math::PI/2.));
//    child->attachObject(ent);
    node->attachObject(ent);

    // we will set its height taking into account also the problem mentioned above
    const Ogre::Vector3 halfSizeBB = ent->getBoundingBox().getHalfSize();
    const float zombieHeight = std::max(halfSizeBB.x,
                                        std::max(halfSizeBB.y, halfSizeBB.z));
    zu.setObjectHeight(zombieHeight);

    // vel
    float vel;
    core::XMLHelper::parseFloat(xmlElement, "vel", vel);
    zu.setVelocity(vel);

    // initial life
    unsigned int aux;
    core::XMLHelper::parseUnsignedInt(xmlElement, "initialLife", aux);
    zu.setLife(aux);
    zu.setInitialLife(aux);

    // bodyPartID
    core::XMLHelper::parseUnsignedInt(xmlElement, "bodyPartID", aux);
    zu.setBodyPartID(aux);

    // configure the zombie
    ASSERT(mRagDollQueue);
    ASSERT(mBodyPartQueue);
    zu.setQueues(mRagDollQueue, mBodyPartQueue);
    zu.configure(node, ent);

    return true;
}


////////////////////////////////////////////////////////////////////////////////
ZombieUnitBuilder::ZombieUnitBuilder() :
    mDynamicWorld(0)
,   mRagDollQueue(0)
,   mBodyPartQueue(0)
{
}

////////////////////////////////////////////////////////////////////////////////
ZombieUnitBuilder::~ZombieUnitBuilder()
{
}

////////////////////////////////////////////////////////////////////////////////
bool
ZombieUnitBuilder::parseXml(const std::string& xmlPath)
{
    mDocument.reset(core::XMLHelper::loadXmlDocument(xmlPath.c_str()));
    if (mDocument.get() == 0) {
        debugERROR("Error opening xml: %s\n", xmlPath.c_str());
        return false;
    }

    TiXmlElement* rootElement = mDocument->RootElement();
    if (std::string(rootElement->Value()) != "ZombieConfig") {
        debugERROR("Root element has wrong name: %s\n", rootElement->Value());
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
ZombieUnitBuilder::fillBodyPartQueue(BodyPartQueue& queue)
{
    ASSERT(mDocument.get());

    queue.clear();
    const TiXmlElement* rootElement = mDocument->RootElement();

    // <ZombieBodyParts>
    //      <BodyPart meshName="head.mesh" type="0" id="3" />
    //

    // get the root element of the ZombieBodyParts
    const TiXmlElement* bodyParts = rootElement->FirstChildElement("ZombieBodyParts");
    if (bodyParts == 0) {
        debugERROR("No ZombieBodyParts found\n");
        return false;
    }

    // iterate over each BodyPart
    bodyParts = bodyParts->FirstChildElement("BodyPart");
    if (bodyParts == 0) {
        debugERROR("No BodyPart elements found\n");
        return false;
    }

    // parse all the elements and use the queue vector
    BodyPartQueue::BodyPartElementVec& bpeVec = queue.bodyPartElements();
    while (bodyParts != 0) {
        // we have to increase the size in one of the queue
        bpeVec.resize(bpeVec.size() + 1);
        // get the element
        BodyPartElement& element = bpeVec.back();

        // parse the element here
        if (!buildBodyPartElement(bodyParts, element)) {
            debugERROR("Some error occur when building the body part element\n");
            return false;
        }
        bodyParts = bodyParts->NextSiblingElement("BodyPart");
    }
    // build the queue
    queue.build();

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
ZombieUnitBuilder::fillRagDollQueue(RagDollQueue<>& queue,
                                    const Ogre::Entity* zombieModel)
{
    ASSERT(mDynamicWorld);
    ASSERT(zombieModel);

    // configure the queue
    return queue.configure(&(mDynamicWorld->bulletDynamicWorld()),
                           zombieModel->getSkeleton(),
                           0);
}

////////////////////////////////////////////////////////////////////////////////
bool
ZombieUnitBuilder::loadZombie(const std::string& id, ZombieUnit& zu)
{
//    <ZombieUnits>
//        <Zombie mesh="zombie.mesh"
//                material="Zombie/Mat1"
//                vel="10"
//                initialLife="100"
//                bodyPartID="1" />//
//    </ZombieUnits>

    const TiXmlElement* rootElement = mDocument->RootElement();

    // get the root element of the ZombieUnits
    const TiXmlElement* zombies = rootElement->FirstChildElement("ZombieUnits");
    if (zombies == 0) {
        debugERROR("No ZombieUnits found\n");
        return false;
    }

    // iterate over each zombie
    zombies = zombies->FirstChildElement("Zombie");
    if (zombies == 0) {
        debugERROR("No Zombie elements found\n");
        return false;
    }

    // search for the zombie that matches with the associated id
    while (zombies != 0) {
        ASSERT(zombies->Attribute("name"));
        if (std::strcmp(zombies->Attribute("name"), id.c_str()) == 0) {
            // this is the one we want
            return configureZombieUnit(zombies, zu);
        }
        zombies = zombies->NextSiblingElement("Zombie");
    }

    // we couldn't found the element we want
    return false;
}

} /* namespace cz */
