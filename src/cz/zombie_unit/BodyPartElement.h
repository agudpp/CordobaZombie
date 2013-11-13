/*
 * BodyPartElement.h
 *
 *  Created on: Oct 31, 2013
 *      Author: agustin
 */

#ifndef BODYPARTELEMENT_H_
#define BODYPARTELEMENT_H_

// forward
//
namespace physics {
class BulletObject;
}

namespace cz {

// define the BodyPart Type enum
enum BodyPartElementType {
    BPE_HEAD = 0,
    BPE_FORE_ARM,
    BPE_UPPER_ARM,
    BPE_UPPER_FORE_ARM,

    BPE_COUNT,
    BPE_INVALID,
};

// Define the BodyPart element containing all the information we need to identify
// each body part uniquely
//
struct BodyPartElement {
    // The bullet object associated to the BodyPart
    physics::BulletObject* bulletObject;
    // The type of the body part
    BodyPartElementType type;
    // The type of the body part ID (each zombie will hold and BodyPartID to
    // be used for the parts).
    unsigned short id;

    BodyPartElement(physics::BulletObject* bp = 0,
                    BodyPartElementType t = BPE_INVALID,
                    unsigned short aID = -1) :
        bulletObject(bp)
    ,   type(t)
    ,   id(aID)
    {}
};

} /* namespace cz */
#endif /* BODYPARTELEMENT_H_ */
