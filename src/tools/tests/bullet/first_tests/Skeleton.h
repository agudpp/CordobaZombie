/*
 * Skeleton.h
 *
 *  Created on: Sep 12, 2013
 *      Author: agustin
 */

#ifndef SKELETON_H_
#define SKELETON_H_


#include <OgreSceneNode.h>
#include <OgreSkeletonInstance.h>
#include <OgreBone.h>

#include <bullet/btBulletDynamicsCommon.h>

#include <types/StackVector.h>

namespace physics {

// @brief This class will be used to handle the skeletons parts and for ogre also.
//        We require the number of parts of the body we will handle per skeleton
//        and the sizes of the different parts (we will represent each part
//        with a box so we need the dimensions of the box).
//        JUST FOR NOW WE WILL HARDCODE THE NAME OF THE BODY PARTS TO CONFIGURE
//        THEM IN A HARDCODED WAY (CONSTRAINTS). BUT THIS CAN BE CHANGED LATER
//


class Skeleton
{
public:
    Skeleton();
    ~Skeleton();

    // @brief Set / get the Dynamic world used to construct the ragdoll
    // @param bdw       The bullet dynamic world
    //
    inline void
    setDynamicWorld(btDynamicsWorld* bdw);
    inline btDynamicsWorld*
    dynamicWorld() const;

    // @brief Construct this skeleton using an ogre skeleton instance and the
    //        body part information.
    //        Note that the order in which the body parts are given is the id
    //        of the body part used later for collisions checkings and everything
    //        else.
    //        This method will create the ragdoll structure for this skeleton also
    //        but will not be enabled until you want to.
    // @param ogreSkeleton      The ogre skeleton instance.
    // @param bodyParts         The list of bodyparts.
    // @return true on success | false otherwise
    //
    bool
    buildFromSkeleton(const Ogre::SkeletonInstance* ogreSkeleton,
                      const BodyPartInfoVec& bodyParts);

    // @brief Update the position/orientation of each body part to be in world
    //        space.


private:
    btDynamicsWorld* mDynamicWorld;

};

} /* namespace physics */
#endif /* SKELETON_H_ */
