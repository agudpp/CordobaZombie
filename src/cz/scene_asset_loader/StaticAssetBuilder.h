/*
 * StaticAssetBuilder.h
 *
 *  Created on: Nov 18, 2013
 *      Author: agustin
 */

#ifndef STATICASSETBUILDER_H_
#define STATICASSETBUILDER_H_

#include <hash_map>
#include <memory>

#include <types/DataHolder.h>
#include <physics/BulletShape.h>
#include <static_world_objects/StaticWorldObject.h>

#include "SceneAssetBuilder.h"


namespace cz {



class StaticAssetBuilder : public SceneAssetBuilder
{
public:
    StaticAssetBuilder();
    virtual
    ~StaticAssetBuilder();

    // @brief Set the Data holder to hold the BulletShape used by the scene.
    //        This should be destroyed after we destroy all the StaticWorldObjects
    //        from the scene since they are probably in the dynamicWorldInstance
    // @param shapeHolder       The Data holder for shapes to be filled with the
    //                          allocated shapes (The caller is the responsible
    //                          for deallocate the memory)
    //
    inline void
    setShapeHolder(core::DataHolder<physics::BulletShape*>* shapeHolder);
    inline core::DataHolder<physics::BulletShape*>*
    setShapeHolder(void);

    // @brief Set the WorldStaticObject vector to be filled with the new allocated
    //        and built assets.
    //        Each StaticWorldObject built from the assets of the scene will
    //        be put in this vector and this instance WILL NOT FREE THE MEMORY
    //        The one who call this is responsible for free the memory
    // @param wsoHolder     The WorldStaticObjectHolder
    //
    inline void
    setWorldStaticObjectHolder(core::DataHolder<StaticWorldObject*>* wsoHolder);
    inline core::DataHolder<StaticWorldObject*>*
    worldStaticObjectHolder(void);



    ////////////////////////////////////////////////////////////////////////////
    // Inherited interface
    //

    // @brief Reset all the information you are currently using to start a new
    //        load.
    //        This method will be called before starting build the assets
    //
    virtual void
    reset(void);

    // @brief Check if this builder can process a particular asset
    // @param asset     The asset to be checked
    // @return true if it can process this asset (build) | false otherwise
    // @note if this instance can process the asset then the method process
    //       will be called
    //
    virtual bool
    canProcess(const core::Asset& asset) const;

    // @brief Build a new asset using the asset information and the associated
    //        scene node and entity to be loaded.
    // @param asset     The asset information
    // @param node      The associated scene node
    // @param entity    The associated entity
    // @return true on success | false otherwise
    //
    virtual bool
    build(const core::Asset& asset, Ogre::SceneNode* node, Ogre::Entity* entity);

    // @brief This method will be called after all the asset of the scene
    //        were parsed.
    //
    virtual void
    finish(void);


private:
    core::DataHolder<physics::BulletShape*>* mShapeHolder;
    core::DataHolder<StaticWorldObject*>* mWorldObjects;
    std::hash_map<std::string, physics::BulletShape*> mShapeMap;

};







////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
StaticAssetBuilder::setShapeHolder(core::DataHolder<physics::BulletShape*>* shapeHolder)
{
    mShapeHolder = shapeHolder;
}
inline core::DataHolder<physics::BulletShape*>*
StaticAssetBuilder::setShapeHolder(void)
{
    return mShapeHolder;
}

inline void
StaticAssetBuilder::setWorldStaticObjectHolder(core::DataHolder<StaticWorldObject*>* wsoHolder)
{
    mWorldObjects = wsoHolder;
}

inline core::DataHolder<StaticWorldObject*>*
StaticAssetBuilder::worldStaticObjectHolder(void)
{
    return mWorldObjects;
}

} /* namespace cz */
#endif /* STATICASSETBUILDER_H_ */
