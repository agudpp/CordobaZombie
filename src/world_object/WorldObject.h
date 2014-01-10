/*
 * WorldObject.h
 *
 *  Created on: Jan 9, 2014
 *      Author: agustin
 */

#ifndef WORLDOBJECT_H_
#define WORLDOBJECT_H_

#include <string.h>

#include "GraphicRepresentation.h"
#include "PhysicsRepresentation.h"
#include "Coll2DRepresentation.h"


// Useful macro to apply to all the representations at once
//
#define WO_CONDITION(rep) rep != 0
#define WO_METHOD_CALL_ARG1(rep, method, arg1) if (WO_CONDITION(rep)) {rep->method(arg1);}
#define WO_ALL_REP_CALL_ARG1(method, arg1)\
    WO_METHOD_CALL_ARG1(mGraphicRep, method, arg1);\
    WO_METHOD_CALL_ARG1(mColl2DRep, method, arg1);\
    WO_METHOD_CALL_ARG1(mPhysicsRep, method, arg1);


namespace w_o {

class WorldObject
{
    // private flags internally used but can be checked / modified externally
    //
    struct Flags {
        unsigned char moveDirty : 1;

        Flags() {memset(this, 0, sizeof(*this));}
    };
public:
    inline
    WorldObject();
    virtual
    ~WorldObject(){};

    // Representations handling
    //

    // @brief Set the different representations and check if we have an specific
    //        representation.
    //        Also return the associated representation.
    // @note We will not handle the memory of any of these representations.
    //
    inline void
    setGraphicRepresentation(GraphicRepresentation* rep);
    inline GraphicRepresentation*
    graphicRepresentation(void);
    inline const GraphicRepresentation*
    graphicRepresentation(void) const;
    inline bool
    hasGraphicRepresentation(void) const;

    inline void
    setColl2DRepresentation(Coll2DRepresentation* rep);
    inline Coll2DRepresentation*
    coll2DRepresentation(void);
    inline const Coll2DRepresentation*
    coll2DRepresentation(void) const;
    inline bool
    hasColl2DRepresentation(void) const;

    inline void
    setPhysicsRepresentation(PhysicsRepresentation* rep);
    inline PhysicsRepresentation*
    physicsRepresentation(void);
    inline const PhysicsRepresentation*
    physicsRepresentation(void) const;
    inline bool
    hasPhysicsRepresentation(void) const;

    // @brief Get/Reset the movement dirty flag. This flag will be set when the
    //        world object is moved / rotated / scaled.
    //        Note that this flag will be not cleared never. The user must
    //        clear this flag if needed.
    //
    inline bool
    isMoveFlagDirty(void) const;
    inline void
    resetMoveFlagDirty(void);


    // Particular methods but probably very used / shortcuts
    //

    // @brief Set this world object invisible or visible (changing the Graphic
    //        representation for example).
    //
    inline void
    setVisible(bool visible);
    inline bool
    isVisible(void) const;

    // @brief Set the 2D collisions enabled / disabled
    //
    inline void
    setCollisions2DEnabled(bool enabled);
    inline bool
    isCollisions2DEnabled(void) const;

    // @brief Enable disable the physics.
    //
    inline void
    setPhysicsEnabled(bool enabled);
    inline bool
    isPhysicsEnabled(void) const;


    // The common Interface for all the representations
    //

    // @brief We can enabled / disable all the representations we have at once
    //        through this methods. If you need to enable disable an specific
    //        representation you can pick the representation and call the
    //        setEnabled() method.
    //        This world object will return true if all the current representations
    //        are enabled.
    //
    inline void
    setEnabled(bool enabled);
    inline bool
    isEnabled(void) const;

    // @brief This method will potitionate all the (active) representation in
    //        the same position (pos).
    // @param pos       The position where we should put it.
    //
    inline void
    setPosition(const Ogre::Vector3& pos);

    // @brief This method will change the orientation of all the representations
    //        (that are currently enabled).
    // @param orientation       The orientation.
    //
    inline void
    setOrientation(const Ogre::Quaternion& orientation);

    // @brief This method will change the scale of all the representations we
    //        currently have enabled.
    // @param scale         The new scale
    //
    inline void
    setScale(const Ogre::Vector3& scale);

    // @brief Set the masks to all the representations.
    // @param masks     The masks information
    //
    inline void
    setMasks(const MaskData& masks);
    inline const MaskData&
    masks(void) const;

    // @brief Set the userDef pointer to all the representations. We will save
    //        this pointer also if you need it later. Note that if you set
    //        different userDef* to different representations we will return the
    //        one you set calling this method in the last call.
    //        If you need to store different userDef* then you can retrieve them
    //        getting first the representation and then getting the userDef.
    // @param userDef       The user def pointer.
    //
    inline void
    setUserDef(void* userDef);
    inline void*
    userDef(void);
    inline const void*
    userDef(void) const;


protected:
    GraphicRepresentation* mGraphicRep;
    Coll2DRepresentation* mColl2DRep;
    PhysicsRepresentation* mPhysicsRep;


private:
    void* mUserDef;
    MaskData mMasks;
    Flags mFlags;

};

















////////////////////////////////////////////////////////////////////////////////
//                              INLINE STUFF                                  //
////////////////////////////////////////////////////////////////////////////////

inline
WorldObject::WorldObject() :
    mGraphicRep(0)
,   mColl2DRep(0)
,   mPhysicsRep(0)
,   mUserDef(0)
{}

// Representations handling
//

inline void
WorldObject::setGraphicRepresentation(GraphicRepresentation* rep)
{
    mGraphicRep = rep;
}
inline GraphicRepresentation*
WorldObject::graphicRepresentation(void)
{
    return mGraphicRep;
}
inline const GraphicRepresentation*
WorldObject::graphicRepresentation(void) const
{
    return mGraphicRep;
}
inline bool
WorldObject::hasGraphicRepresentation(void) const
{
    return mGraphicRep != 0;
}

inline void
WorldObject::setColl2DRepresentation(Coll2DRepresentation* rep)
{
    mColl2DRep = rep;
}
inline Coll2DRepresentation*
WorldObject::coll2DRepresentation(void)
{
    return mColl2DRep;
}
inline const Coll2DRepresentation*
WorldObject::coll2DRepresentation(void) const
{
    return mColl2DRep;
}
inline bool
WorldObject::hasColl2DRepresentation(void) const
{
    return mColl2DRep != 0;
}

inline void
WorldObject::setPhysicsRepresentation(PhysicsRepresentation* rep)
{
    mPhysicsRep = rep;
}
inline PhysicsRepresentation*
WorldObject::physicsRepresentation(void)
{
    return mPhysicsRep;
}
inline const PhysicsRepresentation*
WorldObject::physicsRepresentation(void) const
{
    return mPhysicsRep;
}
inline bool
WorldObject::hasPhysicsRepresentation(void) const
{
    return mPhysicsRep != 0;
}

inline bool
WorldObject::isMoveFlagDirty(void) const
{
    return mFlags.moveDirty;
}
inline void
WorldObject::resetMoveFlagDirty(void)
{
    mFlags.moveDirty = 0;
}


// Particular methods but probably very used / shortcuts
//

inline void
WorldObject::setVisible(bool visible)
{
    if (mGraphicRep) {
        mGraphicRep->setVisible(visible);
    }
}
inline bool
WorldObject::isVisible(void) const
{
    return mGraphicRep && mGraphicRep->isVisible();
}

inline void
WorldObject::setCollisions2DEnabled(bool enabled)
{
    if (mColl2DRep) {
        mColl2DRep->setEnabled(enabled);
    }
}
inline bool
WorldObject::isCollisions2DEnabled(void) const
{
    return mColl2DRep && mColl2DRep->isEnabled();
}

inline void
WorldObject::setPhysicsEnabled(bool enabled)
{
    if (mPhysicsRep) {
        mPhysicsRep->setEnabled(enabled);
    }
}
inline bool
WorldObject::isPhysicsEnabled(void) const
{
    return mPhysicsRep && mPhysicsRep->isEnabled();
}


// The common Interface for all the representations
//

inline void
WorldObject::setEnabled(bool enabled)
{
    // apply to all the representations the same
    WO_ALL_REP_CALL_ARG1(setEnabled, enabled);
}
inline bool
WorldObject::isEnabled(void) const
{
    // check if all the representations are enabled
    return (!mGraphicRep || mGraphicRep->isEnabled()) &&
        (!mColl2DRep || mColl2DRep->isEnabled()) &&
        (!mPhysicsRep || mPhysicsRep->isEnabled());
}

inline void
WorldObject::setPosition(const Ogre::Vector3& pos)
{
    // apply to all the representations the same
    WO_ALL_REP_CALL_ARG1(setPosition, pos);
}

inline void
WorldObject::setOrientation(const Ogre::Quaternion& orientation)
{
    // apply to all the representations the same
    WO_ALL_REP_CALL_ARG1(setOrientation, orientation);
}

inline void
WorldObject::setScale(const Ogre::Vector3& scale)
{
    // apply to all the representations the same
    WO_ALL_REP_CALL_ARG1(setScale, scale);
}

inline void
WorldObject::setMasks(const MaskData& masks)
{
    // apply to all the representations the same
    WO_ALL_REP_CALL_ARG1(setMasks, masks);

    // we will save the masks also
    mMasks = masks;
}
inline const MaskData&
WorldObject::masks(void) const
{
    return mMasks;
}

inline void
WorldObject::setUserDef(void* userDef)
{
    // apply to all the representations the same
    WO_ALL_REP_CALL_ARG1(setUserDef, userDef);

    // we will save the userDef pointer also
    mUserDef = userDef;
}
inline void*
WorldObject::userDef(void)
{
    return mUserDef;
}
inline const void*
WorldObject::userDef(void) const
{
    return mUserDef;
}

} /* namespace w_o */
#endif /* WORLDOBJECT_H_ */
