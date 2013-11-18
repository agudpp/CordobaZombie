/*
 * StaticWorldObject.h
 *
 *  Created on: Nov 10, 2013
 *      Author: agustin
 */

#ifndef STATICWORLDOBJECT_H_
#define STATICWORLDOBJECT_H_

#include <world_object/WorldObject.h>
#include <physics/BulletCollisionObject.h>

// @brief This module represents the static world objects of an scene.
//          - The 3D graphic representation.
//          - The 2D collision representation.
//          - The 3D collision representation.
//          - The associated effects when a bullet collide against it.
//

namespace cz {

class StaticWorldObject : public WorldObject
{
public:
    StaticWorldObject();
    virtual
    ~StaticWorldObject();



    ////////////////////////////////////////////////////////////////////////////
    // Physics stuff methods
    //

    // @brief Set the associated shape to this world object
    // @param shape     The bullet shape to be set
    // @note This class will not handle the shape memory
    //
    inline void
    setShape(physics::BulletShape* shape);
    inline bool
    hasShape(void) const;

    // @brief Check for a possible impact. This method will only check if we
    //        impact the current object.
    // @param hitInfo       The hit information structure used to check the
    //                      impact and retrieve the information also. This
    //                      information will be used later if necessary to process
    //                      the ImpactInfo.
    // @return true if we had impact the world object or not.
    //
    virtual bool
    checkImpact(HitInfo& hitInfo) const;

    // @brief The world was hit by a bullet or something (indicated in the
    //        HitInfo structure). Here we will perform all the operations we
    //        when something hits this object.
    // @param hitInfo       The hit information structure. This structure should
    //                      be filled in the checkImpact() method.
    //
    virtual void
    processImpactInfo(const HitInfo& hitInfo);

    ////////////////////////////////////////////////////////////////////////////
    // Effect related methods
    //

    // @brief Set the effect queue to be used when a bullet hits this object
    // @param queue     The queue of effects
    //
    // TODO: issue #255

    // @brief Set the sound we want to play when a bullet hits this object.
    //        This probably could be an effect sound or just an sound reference.
    //        Try to avoid using the name of the sound (save the reference to the
    //        sound instaed since is less memory expensive).
    // @param sound     The sound to be reproduced when the bullet hits the object
    //
    // TODO: issue #256


    ////////////////////////////////////////////////////////////////////////////
    // Configure the static world object
    //

    // @brief Set the associated position to the object. This will update
    //        the graphic, 2D and 3D representation.
    // @param pos       The world position
    //
    void
    configurePosition(const Ogre::Vector3& pos);

    // @brief Set the associated orientation to the object. This will update
    //        the graphic, 2D and 3D representation.
    // @param rot       The world rotation
    //
    void
    configureRotation(const Ogre::Quaternion& rot);

    // @brief Configure the Static world object with all the information we
    //        already have and using the scene node and entity associated to
    //        this instance.
    //        Note that we have to set up also the CollPreciseInfo data before
    //        calling this method!
    // @param node      The scene node to be used
    // @param entity    The entity of the static world object to be used
    //
    void
    configure(Ogre::SceneNode* node, Ogre::Entity* entity);


private:
    // the physical representation
    physics::BulletCollisionObject mPhysicsRep;
};





///////////////////////////////////////////////////////////////////////////////
// Inline methods
//

inline void
StaticWorldObject::setShape(physics::BulletShape* shape)
{
    mPhysicsRep.setShape(shape);
}

inline bool
StaticWorldObject::hasShape(void) const
{
    return mPhysicsRep.shape() != 0;
}

} /* namespace cz */
#endif /* STATICWORLDOBJECT_H_ */
