/*
 * CollObject.h
 *
 *  Created on: Feb 17, 2013
 *      Author: agustin
 */

#ifndef COLLOBJECT_H_
#define COLLOBJECT_H_

#include <math/Vec2.h>
#include <math/AABB.h>
#include <types/basics.h>
#include <debug/DebugUtil.h>

#include "CollDefines.h"
#include "CollPreciseInfo.h"

namespace coll {

// Forward
//
class CollisionHandler;
class CollCell;

class CollObject {
public:

    // @brief Set/get the bounding box of the object. This bounding box will
    //        contain the position of the object centered in the middle, so
    //        if call this method with a bounding box already translated the
    //        position of the object will be that "translated vector" = AABB.center().
    // @param bb        The bounding box to be used by this object.
    // @note BE CAREFUL calling this method if you have a CollPreciseInfo structure
    //       associated, since it could produce wrong behaviors. Only use this
    //       method when you are using only simple BB objects.
    //       THIS METHOD WILL ASSERT(false) if you have CollPreciseInfo
    //
    inline void
    setBoundingBox(const core::AABB& bb);
    inline const core::AABB&
    boundingBox(void) const;

    // @brief Configure the bounding box of the object given the sizes and the
    //        position of the element (the center).
    // @param width     The width of the bb.
    // @param height    The height of the bb.
    // @param pos       The translation position of the element
    //
    inline void
    configureBB(float width, float height,
                const core::Vector2& pos = core::Vector2(0,0));

    // @brief Set/get the user defined object
    // @param userDef   The user def object
    //
    inline void
    setUserDef(void* userDef);
    inline const void*
    userDef(void) const;
    inline void*
    userDef(void);

    // @brief Set/get the mask for this object
    // @param mask      The mask to be used for collisions
    //
    inline void
    setMask(mask_t mask);
    inline mask_t
    mask(void) const;

    // @brief Return the more detailed information used by this class.
    //
    inline const CollPreciseInfo*
    preciseInfo(void) const;

    // @brief Configure the more precise information for this particular
    //        object. This method will update the AABB automatically but
    //        it will not change the position.
    //        Once you set the CollPreciseInfo you should not free its memory anymore
    //        this class will be the owner of the memory.
    // @param pi        The precise information to be used by this class.
    //
    inline void
    setPreciseInfo(CollPreciseInfo* pi);


    ////////////////////////////////////////////////////////////////////////////
    // Movement functions
    //

    // @brief Set the position of this object (using the center of the bb)
    // @param pos       The position we want to set.
    //
    inline void
    setPosition(const core::Vector2& pos);
    inline core::Vector2
    position(void) const;
    // alias for position() method
    inline core::Vector2
    center(void) const;

    // @brief Translate this object
    // @param tvec      The translation vector
    //
    inline void
    translate(const core::Vector2& tvec);

    // @brief Set the current angle of the object. This should be used if and
    //        only if we have a CollPreciseInfo associated, if not, this value will
    //        not be used.
    // @param angle     The rotation angle of this object in radians
    // @note if we have no CollPreciseInfo associated.
    //       Calling this method will not update the current BoundingBox since
    //       the bounding box associated to this object is always the biggest
    //       where the object (CollPreciseInfo) can fit whenever the rotation it has.
    //
    inline void
    setAngle(const float angle);

    // @brief Enable / disable collisions for this object
    // @param enable    Set the collisions enabled? or disabled?
    //
    inline void
    setCollisionsEnabled(bool enable);
    inline bool
    isCollisionsEnabled(void) const;

    ////////////////////////////////////////////////////////////////////////////
    // Collision helper functions
    //

    // @brief Check if two objects overlaps its bounding box.
    // @param other     The other collision object we want to check
    // @return true if they overlaps theirs bounding box. False otherwise
    //
    inline bool
    collideBB(const CollObject& other) const;

    // @brief Check if two CollObject collides (using the precise info).
    //        This method WILL NOT CHECK the current BB of the elements, so
    //        if both elements not contain precise info this method will return
    //        true (since we are assuming that they collide with theirs BB).
    // @param other     The other obj we want to test against this.
    // @return true if they collide, false otherwise
    //
    inline bool
    collidePrecise(const CollObject& other) const;

    // @brief Get the collisions points for two given elements.
    // TODO:

private:
    // Here we will define some data used by the Handler and the CollCell only
    friend class CollisionHandler;
    friend class CollCell;

    struct Flags {
        unsigned char enabled : 1;
        unsigned char dirty : 1;
    };

    // constructor / destructor
    inline CollObject(mask_t mask = 0,
                      const core::AABB& aabb = core::AABB(),
                      void* userDef = 0,
                      CollPreciseInfo* pi = 0) :
        mMask(mask)
    ,   mAABB(aabb)
    ,   mUserDef(userDef)
    {
        if (pi) {
            setPreciseInfo(pi);
        } else {
            mPinfo = 0;
        }
    }
    inline ~CollObject()
    {
        delete mPinfo;
    }
    // reset the object freeing the Precise info if needed
    inline void reset(void)
    {
        mUserDef = 0;
        delete mPinfo; mPinfo = 0;
        flags = {0};
    }

    // avoid copying
    CollObject(const CollObject&);
    CollObject& operator=(const CollObject&);

    // the ID used by the Handler
    Flags flags;
    index_t id;

private:
    // The members of the object

    // mask object (for collision groups)
    mask_t mMask;
    // the associated AABB and the center of the object
    core::AABB mAABB;
    // user defined type
    void *mUserDef;
    // more precise information for collision detection
    CollPreciseInfo* mPinfo;
};




////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
CollObject::setBoundingBox(const core::AABB& bb)
{
    ASSERT(mPinfo == 0);
    mAABB = bb;
    flags.dirty = true;
}
inline const core::AABB&
CollObject::boundingBox(void) const
{
    return mAABB;
}

inline void
CollObject::configureBB(float width, float height, const core::Vector2& pos)
{
    ASSERT(mPinfo == 0);
    const float halfW = width * 0.5;
    const float halfH = height * 0.5;

    // we need to update the position of the bb taking into account its size
    mAABB.tl.x = pos.x - halfW;
    mAABB.br.x = pos.x + halfW;
    mAABB.tl.y = pos.y + halfH;
    mAABB.br.y = pos.y - halfH;
    flags.dirty = true;
}

inline void
CollObject::setUserDef(void* userDef)
{
    mUserDef = userDef;
}
inline const void*
CollObject::userDef(void) const
{
    return mUserDef;
}
inline void*
CollObject::userDef(void)
{
    return mUserDef;
}

inline void
CollObject::setMask(mask_t mask)
{
    mMask = mask;
}
inline mask_t
CollObject::mask(void) const
{
    return mMask;
}

inline const CollPreciseInfo*
CollObject::preciseInfo(void) const
{
    return mPinfo;
}

inline void
CollObject::setPreciseInfo(CollPreciseInfo* pi)
{
    ASSERT(pi);
    // remove the old one
    delete mPinfo; mPinfo = 0;

    // we need to calculate the size of the new bounding box
    core::AABB bb;
    pi->getMaximumBB(bb);
    mAABB = bb;
    mPinfo = pi;
    flags.dirty = true;
}

////////////////////////////////////////////////////////////////////////////
// Movement functions
//
inline void
CollObject::setPosition(const core::Vector2& pos)
{
    // use translate (this will dirty the flag)
    translate(pos - position());
}
inline core::Vector2
CollObject::position(void) const
{
    return mAABB.center();
}
// alias for position() method
inline core::Vector2
CollObject::center(void) const
{
    return mAABB.center();
}

inline void
CollObject::translate(const core::Vector2& tvec)
{
    // we can translate directly since our invariant is that the AABB is
    // already translated to be in the center position (contains the offset).
    //
    mAABB.translate(tvec);
    flags.dirty = true;
}

inline void
CollObject::setAngle(const float angle)
{
    if (mPinfo) {
        mPinfo->setAngle(angle);
    }
}

inline void
CollObject::setCollisionsEnabled(bool enable)
{
    flags.enabled = enable;
}
inline bool
CollObject::isCollisionsEnabled(void) const
{
    return flags.enabled;
}

////////////////////////////////////////////////////////////////////////////
// Collision helper functions
//

inline bool
CollObject::collideBB(const CollObject& other) const
{
    return mAABB.collide(other.boundingBox());
}

inline bool
CollObject::collidePrecise(const CollObject& other) const
{
    if (mPinfo) {
        if (other.mPinfo) {
            return mPinfo->checkOverlap(*(other.mPinfo));
        } else {
            return mPinfo->checkOverlap(other.mAABB);
        }
    } else if (other.mPinfo) {
        return other.mPinfo->checkOverlap(mAABB);
    }
    // else true
    return true;
}

}

#endif /* COLLOBJECT_H_ */
