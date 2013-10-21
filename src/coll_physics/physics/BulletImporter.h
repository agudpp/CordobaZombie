/*
 * BulletImporter.h
 *
 *  Created on: Sep 25, 2013
 *      Author: agustin
 */

#ifndef BULLETIMPORTER_H_
#define BULLETIMPORTER_H_

#include <OgreAxisAlignedBox.h>

#include "BulletObject.h"

namespace physics {

// @brief This class will be used to create the btRigidBodies from the .mesh
//        Ogre files.
//

class BulletImporter
{
public:
    BulletImporter();
    ~BulletImporter();

    // @brief Helper method to construct a simple box.
    // @param bb        The bounding box
    // @param mass      The mass of the bb
    // @return BulletObject | 0 on error
    // This method will set the rigidBody->userDefinedPointer(this).
    //
    static BulletObject*
    createBox(const Ogre::AxisAlignedBox& bb, float mass);
};

} /* namespace physics */
#endif /* BULLETIMPORTER_H_ */
