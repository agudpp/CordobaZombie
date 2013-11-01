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

#include "ZombieUnit.h"
#include "BodyPartElement.h"
#include "BodyPartQueue.h"
#include "RagDollQueue.h"



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
    bpe.bulletObject = physics::BulletImporter::createBox(ent->getBoundingBox(),
                                                          mass,
                                                          false);
    ASSERT(bpe.bulletObject);

    // create the scene node for the entity and attach the object to it
    //
    Ogre::SceneNode* node = sceneMngr->createSceneNode();
    node->attachObject(ent);
    bpe.bulletObject->motionState.setNode(node);

    // get the id and the type
    unsigned int aux;
    core::XMLHelper::parseUnsignedInt(xmlElement, "type", aux);
    bpe.type = aux;
    core::XMLHelper::parseUnsignedInt(xmlElement, "id", aux);
    bpe.id = aux;

    return true;
}


////////////////////////////////////////////////////////////////////////////////
ZombieUnitBuilder::ZombieUnitBuilder()
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
    mDocument = core::XMLHelper::loadXmlDocument(xmlPath.c_str());
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

    // parse all the elements
    BodyPartElement element;
    while (bodyParts != 0) {
        // parse the element here
        if (!buildBodyPartElement(bodyParts, element)) {
            debugERROR("Some error occur when building the body part element\n");
            return false;
        }
        queue.addBodyPartElement(element);
        bodyParts = bodyParts->NextSiblingElement("BodyPart");
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
ZombieUnitBuilder::fillRagDollQueue(RagDollQueue& queue);

////////////////////////////////////////////////////////////////////////////////
bool
ZombieUnitBuilder::loadZombie(const std::string& id, ZombieUnit& zu);

} /* namespace cz */
