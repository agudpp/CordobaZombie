/*
 * BulletImporter.h
 *
 *  Created on: Sep 25, 2013
 *      Author: agustin
 */

#ifndef BULLETIMPORTER_H_
#define BULLETIMPORTER_H_

namespace physics {

// @brief This class will be used to create the btRigidBodies from the .mesh
//        Ogre files.
//

class BulletImporter
{
public:
    BulletImporter();
    virtual
    ~BulletImporter();
};

} /* namespace physics */
#endif /* BULLETIMPORTER_H_ */
