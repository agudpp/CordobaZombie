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

namespace coll {

// Forward
//
class CollisionHandler;
class CollCell;

class CollObject {
public:

    // @brief Set/get the bounding box of the object.
    // @param bb        The bounding box to be used by this object.
    // @note BE CAREFUL calling this method if you have a PreciseInfo structure
    //       associated, since it could produce wrong behaviors. Only use this
    //       method when you are using only simple BB objects.
    //
    void
    setBoundingBox(const core::AABB& bb);
    inline const core::AABB&
    boundingBox(void) const;

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
    inline const PreciseInfo*
    preciseInfo(void) const;

    // @brief Configure the more precise information for this particular
    //        object. This method will update the AABB automatically but
    //        not the position.
    //        Once you set the PreciseInfo you should not free its memory anymore
    //        this class will be the owner of the memory.
    // @param pi        The precise information to be used by this class.
    //
    void
    setPreciseInfo(PreciseInfo* pi);


    ////////////////////////////////////////////////////////////////////////////
    // Movement functions
    //

    // @brief Set the position of this object (using the center)
    // @param pos       The position we want to set.
    //
    void
    setPosition(const core::Vector2& pos);
    inline const core::Vector2&
    position(void) const;
    // alias for position() method
    inline const core::Vector2&
    center(void) const;

    // @brief Translate this object
    // @param tvec      The translation vector
    //
    void
    translate(const core::Vector2& tvec);

    // @brief Enable / disable collisions for this object
    // @param enable    Set the collisions enabled? or disabled?
    //
    void
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

    // @brief Get the collisions points for two given elements.
    // TODO:

private:
    // Here we will define some data used by the Handler and the CollCell only
    friend class CollisionHandler;
    friend class CollCell;

    struct Flags {
        unsigned char enabled : 1;
        unsigned char beingTracked : 1;
    };

    // constructor / destructor
    inline CollObject(mask_t mask = 0,
                      const core::AABB& aabb = core::AABB(),
                      void* userDef = 0,
                      PreciseInfo* pi = 0) :
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
        // do nothing
    }
    // reset the object freeing the Precise info if needed
    inline void reset(void)
    {
        mUserDef = 0;
        delete mPinfo; mPinfo = 0;
        flags = {0};
    }

    // @brief Set the collision handler to be used. This function should
    //        be called before any of the other functions of this class.
    //        It will be called automatically by the last instance of the Collision
    //        Hanlder created.
    //
    static void
    setCollisionHandler(CollisionHandler* ch)
    {
        sCollHandler = ch;
    }

    // avoid copying
    CollObject(const CollObject&);
    CollObject& operator=(const CollObject&);

    // the ID used by the Handler
    Flags flags;
    index_t id;

private:
    // The members of the object

    // the pointer to the CollisionHandler
    static CollisionHandler* sCollHandler;

    // mask object (for collision groups)
    mask_t mMask;
    // the associated AABB and the center of the object (not necessary the
    // center of the AABB).
    core::AABB mAABB;
    core::Vector2 mCenter;

    // user defined type
    void *mUserDef;
    // more precise information for collision detection
    PreciseInfo* mPinfo;
};




////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline const core::AABB&
CollObject::boundingBox(void) const
{
    return mAABB;
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

inline const PreciseInfo*
CollObject::preciseInfo(void) const
{
    return mPinfo;
}


////////////////////////////////////////////////////////////////////////////
// Movement functions
//

inline const core::Vector2&
CollObject::position(void) const
{
    return mCenter;
}
// alias for position() method
inline const core::Vector2&
CollObject::center(void) const
{
    return mCenter;
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

}

#endif /* COLLOBJECT_H_ */
