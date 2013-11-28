/*
 * BodyPartElement.h
 *
 *  Created on: Oct 31, 2013
 *      Author: agustin
 */

#ifndef BODYPARTELEMENT_H_
#define BODYPARTELEMENT_H_

#include <physic_game_object/PhysicGameObject.h>
#include <physics/BulletObject.h>
#include <debug/DebugUtil.h>


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
class BodyPartElement : public PhysicGameObject {
public:

    BodyPartElement(physics::BulletObject* bp = 0,
                    BodyPartElementType t = BPE_INVALID,
                    unsigned short aID = -1) :
        mBulletObject(bp)
    ,   mType(t)
    ,   mID(aID)
    {}
    virtual ~BodyPartElement()
    {}

    // @brief Set the bullet object to use. This will configure the userPointer
    //        to point to this class.
    // @param bulletObject      The bullet object
    //
    inline void
    setBulletObject(physics::BulletObject* bulletObject);
    inline physics::BulletObject*
    bulletObject(void);
    inline const physics::BulletObject*
    bulletObject(void) const;

    // @brief The body part element type related to this instance
    //
    inline void
    setBodyPartElementType(BodyPartElementType type);
    inline BodyPartElementType
    bodyPartElementType(void) const;

    // @brief Set / get the BodyPart ID (each zombie will hold and BodyPartID to
    //        be used for the parts)
    //
    inline void
    setID(unsigned short id);
    inline unsigned short
    ID(void) const;

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

    // @brief The object was hit by a bullet or something (indicated in the
    //        HitInfo structure). Here we will perform all the operations we
    //        when something hits this object.
    // @param hitInfo       The hit information structure. This structure should
    //                      be filled in the checkImpact() method.
    //
    virtual void
    processImpactInfo(const HitInfo& hitInfo);

private:
    // avoid copying
    BodyPartElement(const BodyPartElement&);
    BodyPartElement& operator=(const BodyPartElement&);

private:
    // The bullet object associated to the BodyPart
    physics::BulletObject* mBulletObject;
    // The type of the body part
    BodyPartElementType mType;
    // The type of the body part ID (each zombie will hold and BodyPartID to
    // be used for the parts).
    unsigned short mID;
};





////////////////////////////////////////////////////////////////////////////////
inline void
BodyPartElement::setBulletObject(physics::BulletObject* bulletObject)
{
    ASSERT(bulletObject);
    mBulletObject = bulletObject;
    // configure the user pointer
    bulletObject->setUserPointer(this);
}
inline physics::BulletObject*
BodyPartElement::bulletObject(void)
{
    return mBulletObject;
}
inline const physics::BulletObject*
BodyPartElement::bulletObject(void) const
{
    return mBulletObject;
}

inline void
BodyPartElement::setBodyPartElementType(BodyPartElementType type)
{
    mType = type;
}
inline BodyPartElementType
BodyPartElement::bodyPartElementType(void) const
{
    return mType;
}

inline void
BodyPartElement::setID(unsigned short id)
{
    mID = id;
}
inline unsigned short
BodyPartElement::ID(void) const
{
    return mID;
}

} /* namespace cz */
#endif /* BODYPARTELEMENT_H_ */
