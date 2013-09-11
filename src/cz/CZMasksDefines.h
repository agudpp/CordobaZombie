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

enum CZRayMask {
    CZRM_None               = 0,
    CZRM_ZOMBIE             = (1 << 0),
    CZRM_WORLD              = (1 << 1),

};

}

#endif /* CZMASKSDEFINES_H_ */
