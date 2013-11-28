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
#include <fx/effects/EffectQueueDefs.h>

// @brief This module represents the static world objects of an scene.
//          - The 3D graphic representation.
//          - The 2D collision representation.
//          - The 3D collision representation.
//          - The associated effects when a bullet collide against it.
//

// forward
//
namespace effect {
class EffectHandler;
}


namespace cz {

class StaticWorldObject : public WorldObject
{
public:
    StaticWorldObject();
    virtual
    ~StaticWorldObject();

    // @brief We need to set the global EffectHandler to be used for all the
    //        effects of the zombies.
    // @param eh        The global effect handler
    //
    static inline void
    setEffectHandler(effect::EffectHandler* eh);
    static inline effect::EffectHandler*
    effectHandler(void);

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
    //
    inline void
    setBulletImpactEffectQueue(BulletImpactEffectQueue* queue);
    inline BulletImpactEffectQueue*
    bulletImpactEffectQueue(void);



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
    BulletImpactEffectQueue* mBulletEffectQueue;

    // static data
    static effect::EffectHandler* sEffectHandler;
};





///////////////////////////////////////////////////////////////////////////////
// Inline methods
//
inline void
StaticWorldObject::setEffectHandler(effect::EffectHandler* eh)
{
    ASSERT(eh);
    sEffectHandler = eh;
}
inline effect::EffectHandler*
StaticWorldObject::effectHandler(void)
{
    return sEffectHandler;
}

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

inline void
StaticWorldObject::setBulletImpactEffectQueue(BulletImpactEffectQueue* queue)
{
    mBulletEffectQueue = queue;
}
inline BulletImpactEffectQueue*
StaticWorldObject::bulletImpactEffectQueue(void)
{
    return mBulletEffectQueue;
}

} /* namespace cz */
#endif /* STATICWORLDOBJECT_H_ */
