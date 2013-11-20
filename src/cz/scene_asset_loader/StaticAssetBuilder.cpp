/*
 * StaticAssetBuilder.cpp
 *
 *  Created on: Nov 18, 2013
 *      Author: agustin
 */

#include "StaticAssetBuilder.h"

#include <memory>

#include <debug/DebugUtil.h>
#include <math/FloatComp.h>
#include <os_utils/OSHelper.h>
#include <collisions/helpers/CollObjectExporter.h>
#include <physics/helpers/BulletExporter.h>
#include <physics/BulletImporter.h>



namespace cz {

bool
StaticAssetBuilder::configureStaticWorldObject(const core::Asset& asset,
                                               Ogre::SceneNode* node,
                                               Ogre::Entity* entity,
                                               StaticWorldObject& swo)
{
    ASSERT(node);
    ASSERT(entity);

    // set all the information we need here
    coll::CollPreciseInfo* coll2D = 0;
    core::AABB aabb;

    // we will check if we have 2D and 3D representation
    if (!asset.coll2DRepFile.empty()) {
        // parse that file
        if (!core::OSHelper::fileExists(asset.coll2DRepFile.c_str())) {
            debugERROR("Error, invalid asset: file %s doesn't exists. If this"
                " asset has no 2D representation then the filepath should be"
                " empty\n", asset.coll2DRepFile.c_str());
            return false;
        }
        // else parse the information here
        if (!coll::CollObjectExporter::buildFromFile(asset.coll2DRepFile,
                                                     coll2D,
                                                     aabb)) {
            debugERROR("Cannot build 2D rep\n");
            return false;
        }
    } else {
        debugRED("No collision 2D representation found for asset %s\n",
                 asset.name.c_str());
    }

    // set the information
    swo.setCollisionInformation(coll2D, aabb);

    // now try 3D, for this one we will use the shape map to check if the shape
    // was already created
    physics::BulletShape* shape = 0;
    auto shapeIt = mShapeMap.find(asset.meshName);
    if (shapeIt != mShapeMap.end()) {
        // we found a shape representation
        shape = shapeIt->second;
    } else {
        // we need to create a new one
        if (!asset.coll3DRepFile.empty()) {
            if (!core::OSHelper::fileExists(asset.coll3DRepFile.c_str())) {
                debugERROR("Error, invalid asset: file %s doesn't exists. If this"
                    " asset has no 3D representation then the filepath should be"
                    " empty\n", asset.coll3DRepFile.c_str());
                return false;
            }
            // create the new one
            shape = physics::BulletExporter::buildShapeFromFile(asset.coll3DRepFile);
            if (shape == 0) {
                debugERROR("Error building the 3D shape\n");
                return false;
            }
            // add it to the shape holder and the map
            mShapeHolder->add(shape);
            mShapeMap[asset.meshName] = shape;
        }
    }
    // set the shape to the object
    if (shape) {
        swo.setShape(shape);
    }

    // TODO: complete here with the material type to use specific effects.
    debugERROR("TODO: check issue #250, #260, #261\n");

    // now configure the static world object
    swo.configure(node, entity);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
StaticAssetBuilder::configureStaticWorldFloor(const core::Asset& asset,
                                              Ogre::SceneNode* node,
                                              Ogre::Entity* entity,
                                              StaticWorldObject& swo)
{
    ASSERT(entity);
    ASSERT(node);

    // for this particular case we don't need any collision precise information
    // We will construct the plane associated to the floor
    Ogre::AxisAlignedBox bb = entity->getBoundingBox();
    debugERROR("Check here if we will have one or more parts of the floor. If we"
        " will only have one then we can create a PlaneShape instead of a "
        "bounding box shape. If not, then we need to construct the shape from the"
        " bounding box of the entity (and we will also need to set the size a little"
        " bigger to avoid problems with the fast objects collisions. To do that"
        " we cannot just increase the z axis value because the shape will be "
        "moved down or up to be centered with the graphical representation."
        " We have to investigate how to set an offset!");

    // check if the z axis
    physics::BulletShape* shape = physics::BulletImporter::createBoxShape(bb);

    // set the shape to the object
    if (shape) {
        swo.setShape(shape);
        mShapeHolder->add(shape);
    } else {
        debugERROR("Error creating the shape for the floor!! this is not good\n");
    }

    // TODO: complete here with the material type to use specific effects.
    debugERROR("TODO: check issue #250, #260, #261\n");

    // now configure the static world object
    swo.configure(node, entity);

    // we have to remove the collision 2D object from the world since we don't
    // want it
    swo.removeFromCollWorld();
    swo.setCollMask(0);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
BulletImpactEffectQueue*
StaticAssetBuilder::getBulletImpactQueue(const core::Asset& asset)
{
    ASSERT(mBulletImpactHandler);

    BulletImpactQueueHandler::Type type = BulletImpactQueueHandler::Type::BIQT_DEFAULT;
    switch (asset.materialType) {
    case core::AssetMaterialType::ASSET_MAT_NONE:
        break;
    case core::AssetMaterialType::ASSET_MAT_METAL:
        type = BulletImpactQueueHandler::Type::BIQT_METAL;
        break;
    case core::AssetMaterialType::ASSET_MAT_WOOD:
        type = BulletImpactQueueHandler::Type::BIQT_WOOD;
        break;
    case core::AssetMaterialType::ASSET_MAT_ROCK:
        type = BulletImpactQueueHandler::Type::BIQT_ROCK;
        break;
    }

    return mBulletImpactHandler->queue(type);
}

////////////////////////////////////////////////////////////////////////////////
StaticAssetBuilder::StaticAssetBuilder() :
    mShapeHolder(0)
,   mWorldObjects(0)
,   mBulletImpactHandler(0)
{
    debugOPTIMIZATION("We are using a hash_map here, we can optmize this probably\n");
}

////////////////////////////////////////////////////////////////////////////////
StaticAssetBuilder::~StaticAssetBuilder()
{

}


////////////////////////////////////////////////////////////////////////////////
void
StaticAssetBuilder::reset(void)
{
    ASSERT(mShapeHolder);
    ASSERT(mWorldObjects);
    ASSERT(mBulletImpactHandler);

    mShapeMap.clear();
    if (!mShapeHolder->elements().empty()) {
        debugWARNING("The shape holder is not empty\n");
    }
    if (!mWorldObjects->elements().empty()) {
        debugWARNING("World object is not empty!\n");
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
StaticAssetBuilder::canProcess(const core::Asset& asset) const
{
    // we will process the asset if the type is the static one
    return (asset.type == core::AssetType::ASSET_STATIC_WORLD_ELEMENT) ||
        (asset.type == core::AssetType::ASSET_STATIC_WORLD_FLOOR);
}

////////////////////////////////////////////////////////////////////////////////
bool
StaticAssetBuilder::build(const core::Asset& asset,
                          Ogre::SceneNode* node,
                          Ogre::Entity* entity)
{
    ASSERT(node);
    ASSERT(entity);

    // we will build the static object here
    // - construct the new world object
    std::unique_ptr<StaticWorldObject> swo(new StaticWorldObject);

    // - take into account the node scale (print a warning if different from 1)
    const Ogre::Vector3& scale = node->getScale();
    if (!core::fcomp_equal(scale.x, 1.f) ||
        !core::fcomp_equal(scale.y, 1.f) ||
        !core::fcomp_equal(scale.z, 1.f)) {
        debugWARNING("We are not currently supporting scales in the assets... "
            "Please verify that the scene contains only elements that are not "
            "scaled! Current scale found: [%f, %f, %f] for node %s and entity %s\n",
            scale.x, scale.y, scale.z,
            node->getName().c_str(), entity->getName().c_str());
        return false;
    }
    // now parse the asset, check if it is the floor or a normal object
    bool result = false;
    if (asset.type == core::AssetType::ASSET_STATIC_WORLD_ELEMENT) {
        result = configureStaticWorldObject(asset, node, entity, *(swo.get()));
    } else if (asset.type == core::AssetType::ASSET_STATIC_WORLD_FLOOR) {
        result = configureStaticWorldFloor(asset, node, entity, *(swo.get()));
    }
    if (!result) {
        debugERROR("Error configuring asset\n");
        return false;
    }

    // set the queue to be used by this static object
    swo->setBulletImpactEffectQueue(getBulletImpactQueue(asset));

    // everything fine
    // release the scope pointer and put it in the vector
    mWorldObjects->add(swo.release());

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
StaticAssetBuilder::finish(void)
{
    // clear the map memory
    mShapeMap.clear();

    // we probably do nothing else here...
}

} /* namespace cz */
