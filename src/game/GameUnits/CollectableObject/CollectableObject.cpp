/*
 * CollectableObject.cpp
 *
 *  Created on: 29/05/2012
 *      Author: agustin
 */

#include "CollectableObject.h"

#include <OgreMaterialManager.h>
#include <OgreSubEntity.h>

#include <GlobalObjects/GlobalObjects.h>
#include <SelectionSystem/SelectionType.h>
#include <collision/CollisionTypedefs.h>

Ogre::MaterialPtr CollectableObject::sSelMaterial;

CollectableObject::CollectableObject(CollectableType type) :
    mObj(0)
,   mType(type)
{
    // Ugly way to check if the SelectionMaterial is loaded, but this shouldn't
    // affect the game performance since all this objects will be created
    // at the beginning
    if (sSelMaterial.isNull()) {
        // load the material here
        Ogre::MaterialManager& mm = Ogre::MaterialManager::getSingleton();
        debugWARNING("We are using CollObjSelection material for the selected "
            "objects! Hardcoded value!\n");
        sSelMaterial = mm.getByName("CollObjSelection");
    }

    // we need also set the SelectableObject type
    setType(selection::Type::SEL_TYPE_COL_OBJECT);
}

CollectableObject::~CollectableObject()
{

}

void
CollectableObject::objectPicked(void)
{
    // we deactivate the object to avoid collisions against other objects
    setActiveCollision(false);

    // remove from the sceneManager
    if (mNode && mNode->getParentSceneNode()) {
        mNode->getParentSceneNode()->removeChild(mNode);
    }
}

void
CollectableObject::objectThrown(void)
{
    // reactivate collisions again
    setActiveCollision(true);

    // put it again in the scene manager (in the root)
    ASSERT(mNode);
    ASSERT(mNode->getParentSceneNode() == 0);
    if (!mNode || mNode->getParentSceneNode() == 0) {
        return;
    }

    GLOBAL_SCN_MNGR->getRootSceneNode()->addChild(mNode);
}

void
CollectableObject::build(Ogre::Entity *entity, Ogre::SceneNode *node)
{
    // configure the entity and scene node
    ASSERT(entity || mEntity);
    ASSERT(node || mNode);

    if (!mEntity) {
        mEntity = entity;
    }
    if (!mNode) {
        mNode = node;
    }

    // check that the entity is currently attached?
//    if (mNode->getAttachedObject(mEntity->getName()) == 0) {
        mNode->attachObject(mEntity);
//    }

    // config the bounding box
    float w,h;
    getAABBFromEntity(w,h);
    configCollObj(w,h, c_p::COL_FLAG_COLLECTABLE_OBJ, c_p::COL_GRFLAG_COLL_OBJECT);
}

void
CollectableObject::objectSelected(void)
{
    ASSERT(false); // TODO
}

void
CollectableObject::objectUnselected(void)
{
    ASSERT(false); // TODO
}

void
CollectableObject::mouseOverObject(void)
{
    ASSERT(mEntity);

    // we swap the material to use the global one (note that we are assuming that
    // we only have one subentity for this entity, that it should be the case
    // always for this kind of objects).
    ASSERT(mEntity->getNumSubEntities() == 1);
    mMaterial = mEntity->getSubEntity(0)->getMaterial();
    mEntity->setMaterial(sSelMaterial);
}

void
CollectableObject::mouseExitObject(void)
{
    // revert the entity material
    ASSERT(mEntity);
    mEntity->setMaterial(mMaterial);
}

void
CollectableObject::beenHit(const Hit_t &hit)
{
    ASSERT(false && "Probably something is wrong here, the Collectable objects"
        " shouldn't be getting this kind of events for now...\n");
}

