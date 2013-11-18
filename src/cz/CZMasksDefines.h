/*
 * CZMasksDefines.h
 *
 *  Created on: Sep 9, 2013
 *      Author: agustin
 */

#ifndef CZMASKSDEFINES_H_
#define CZMASKSDEFINES_H_

namespace cz {

// @brief Here we will define all the masks used for the collisions / raycasts /
//        etc..
#ifndef BIT_SET
#define BIT_SET(x) (1 << x)
#endif
enum CZRayMask {
    CZRM_NONE               = 0,
    // this mask is also defined in RagDoll.h (physics::RagDoll) and check
    // the mask collision "agains" also. In this case it will be only the
    // world objects (id = 4)
    CZRM_RAGDOLL            = (1 << 0),
    CZRM_ZOMBIE             = (1 << 1),
    CZRM_WORLD_STATIC       = (1 << 2),
    CZRM_WORLD_DYNAMIC      = (1 << 3),

    CZRM_ZOMBIE_BB          = (1 << 4),
    CZRM_ZOMBIE_BODY_PART   = (1 << 5),

    CZRM_RAYCASTABLE        = (1 << 6),

    // collide against everything
    CZRM_WORLD              = (CZRM_WORLD_STATIC | CZRM_WORLD_DYNAMIC),
    CZRM_ALL                = (CZRM_ZOMBIE | CZRM_WORLD),
};


}

#endif /* CZMASKSDEFINES_H_ */
