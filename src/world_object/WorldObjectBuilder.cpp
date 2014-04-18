/*
 * WorldObjectBuilder.cpp
 *
 *  Created on: Feb 5, 2014
 *      Author: agustin
 */

#include "WorldObjectBuilder.h"

#include <OgreSceneNode.h>
#include <OgreEntity.h>

#include <debug/DebugUtil.h>
#include <asset/Asset.h>
#include <collisions/helpers/CollObjectExporter.h>
#include <physics/DynamicWorld.h>
#include <physics/CollisionObject.h>
#include <physics/PhysicsBuilder.h>

#include "WorldObject.h"
#include "GraphicRepresentation.h"
#include "Coll2DRepresentation.h"
#include "PhysicsRepresentation.h"

namespace w_o {

////////////////////////////////////////////////////////////////////////////////
bool
WorldObjectBuilder::buildFromAsset(const core::Asset& asset,
                                   Ogre::SceneManager* scnMngr,
                                   WorldObject& wo)
{
    ASSERT(scnMngr);

    // create the graphic representation
    if (asset.meshName.empty()) {
        debugERROR("No graphic representation for asset %s\n", asset.fullAssetPath.c_str());
        return false;
    }

    Ogre::Entity* ent = scnMngr->createEntity(asset.meshName);
    if (ent == 0) {
        debugERROR("Error creating entity %s, not found\n", asset.meshName.c_str());
        return false;
    }
    Ogre::SceneNode* node = scnMngr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(ent);
    GraphicRepresentation* grep = new GraphicRepresentation;
    grep->setSceneNode(node);
    grep->setEntity(ent);
    wo.setGraphicRepresentation(grep);

    // check if the asset is a static element or dynamic one
    const bool isStatic = asset.type == core::AssetType::ASSET_STATIC_WORLD_ELEMENT ||
        asset.type == core::AssetType::ASSET_STATIC_WORLD_FLOOR;

    // create the Collision2D rep if we have
    if (!asset.coll2DRepFile.empty()) {
        // we have it, create it
        core::AABB bb;
        coll::CollPreciseInfo* pinfo = 0;
        if (!coll::CollObjectExporter::buildFromFile(asset.coll2DRepFile, pinfo, bb)) {
            debugERROR("Error building the collision precise object from %s\n",
                       asset.fullAssetPath.c_str());
            ASSERT(false && "we have a leak here! we need to remove all the other "
                "stuff before going out");
            return false;
        }

        Coll2DRepresentation* collRep = new Coll2DRepresentation;
        collRep->setCollisionInformation(pinfo, bb);
        // set the type of the coll object
        if (isStatic) {
            collRep->setCollision2DType(Coll2DRepresentation::Coll2DType::Coll2D_Static);
        } else {
            collRep->setCollision2DType(Coll2DRepresentation::Coll2DType::Coll2D_Dynamic);
        }
        wo.setColl2DRepresentation(collRep);
    }

    // create the Physics representation if we have
    if (!asset.coll3DRepFile.empty()) {
        // we have to create the shape and the object, for now we will assume that
        // all the elemnts will be static
        physics::CollisionObject* collObj = 0;
        if (isStatic) {
            collObj = new physics::CollisionObject;

            // construct the shape
            if (!physics::PhysicsBuilder::buildShapeFromFile(collObj->shape(),
                                                             asset.coll3DRepFile)) {
                debugERROR("Error loading the 3D Representation from %s\n",
                           asset.fullAssetPath.c_str());
                ASSERT(false && "we have a leak here! we need to remove all the other "
                        "stuff before going out");
                return false;
            }

            // construct the collision object from this shape
            if (!physics::PhysicsBuilder::createCollObjectFromShape(*collObj,
                                                                    collObj->shape())) {
                debugERROR("Error building the collision object for %s\n",
                           asset.fullAssetPath.c_str());
                ASSERT(false && "we have a leak here! we need to remove all the other "
                        "stuff before going out");
                return false;
            }

            // now we set the coll object to the PhysicsRepresentation
            PhysicsRepresentation* physicRep = new PhysicsRepresentation;
            physicRep->setCollisionObjectElement(collObj);
            wo.setPhysicsRepresentation(physicRep);

        } else {
            ASSERT(false && "Not implemented, here we have to construct a rigid"
                " body instead of a collision object");
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
WorldObjectBuilder::destroyPlainWorldObject(Ogre::SceneManager* scnMngr,
                                            WorldObject& wo)
{
    ASSERT(scnMngr);

    // destroy the graphic representation
    if (wo.hasGraphicRepresentation()) {
        GraphicRepresentation* grep = wo.graphicRepresentation();
        if (grep->sceneNode()) {
            scnMngr->destroySceneNode(grep->sceneNode());
        }
        if (grep->entity()) {
            scnMngr->destroyEntity(grep->entity());
        }
        delete grep;
        wo.setGraphicRepresentation(0);
    }

    // destroy the collision 2D rep
    if (wo.hasColl2DRepresentation()) {
        Coll2DRepresentation* collrep = wo.coll2DRepresentation();
        delete collrep;
        wo.setColl2DRepresentation(0);
    }

    // destroy the Physics representation
    if (wo.hasPhysicsRepresentation()) {
        PhysicsRepresentation* rep = wo.physicsRepresentation();
        // remove it from the world
        rep->setEnabled(false);

        // now delete the shape and collision object
        physics::CollisionObject* coll = rep->collisionObject();
        physics::PhysicsBuilder::destroyCollObjectAndShape(*coll);
        delete rep;
        wo.setPhysicsRepresentation(0);
    }
}

}
