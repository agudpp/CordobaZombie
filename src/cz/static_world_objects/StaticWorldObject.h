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

// @brief This class will be used to load all the static world objects that
//        are static.
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

private:
    // the physical representation
    physics::BulletCollisionObject mPhysicsRep;
};

} /* namespace cz */
#endif /* STATICWORLDOBJECT_H_ */
