/*
 * HitInfo.h
 *
 *  Created on: Nov 4, 2013
 *      Author: agustin
 */

#ifndef HITINFO_H_
#define HITINFO_H_

#include <OgreVector3.h>
#include <OgreRay.h>

#include <firing_system/Bullet.h>

namespace cz {


// Here we will put all the information needed for the hitting information
// (this is, when we fire a bullet / any weapon, we want to check if hit against
// a zombie, and if that is the case, then perform any operation we need).
//


struct HitInfo {
    Ogre::Ray ray;
    Ogre::Vector3 intersectionPoint;
    bool hasImpact;
    float power;
    ZombieBody::BodyPart bodyPart;

    // @brief Configure the HitInfo from a Bullet.
    //        This will reset all the values to the default ones
    // @param bullet        The bullet to be used to configure this class
    //
    inline void
    reset(const Bullet& bullet);

    // @brief Configure the HitInfo from a origin and direction of the bullet
    //        and the associated power.
    // @param origin        The origin
    // @param dir           The direction
    // @param power         The power of the bullet / XXX.
    //
    inline void
    reset(const Ogre::Vector3& origin, const Ogre::Vector3& dir, float power);
};







////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
HitInfo::reset(const Bullet& bullet)
{
    ray.setDirection(bullet.direction());
    ray.setOrigin(bullet.currentPosition());
    power = bullet.power();
    hasImpact = false;
}
inline void
HitInfo::reset(const Ogre::Vector3& origin, const Ogre::Vector3& dir, float p)
{
    ray.setDirection(dir);
    ray.setOrigin(origin);
    power = p;
    hasImpact = false;
}


} /* namespace cz */
#endif /* HITINFO_H_ */
