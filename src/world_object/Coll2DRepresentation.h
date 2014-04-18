/*
 * Coll2DRepresentation.h
 *
 *  Created on: Jan 8, 2014
 *      Author: agustin
 */

#ifndef COLL2DREPRESENTATION_H_
#define COLL2DREPRESENTATION_H_

#include <OgreVector3.h>
#include <OgreQuaternion.h>

#include <collisions/CollDefines.h>
#include <collisions/CollisionHandler.h>
#include <collisions/CollObject.h>
#include <math/Vec2.h>
#include <debug/DebugUtil.h>

#include "MaskData.h"

namespace w_o {

class Coll2DRepresentation
{

public:

    enum Coll2DType {
        Coll2D_Static = 0,
        Coll2D_Dynamic,
    };


    // @brief Method used to configure the the Collision handler to be used
    //        for all the world objects.
    // @param collHandler       The handler (should be != 0)
    // @note This method should be called before the construction of any
    //       WorldObject
    //
    static inline void
    setCollisionHandler(coll::CollisionHandler* collHandler);


public:
    Coll2DRepresentation();
    ~Coll2DRepresentation();

    // avoid copy constructor
    //
    Coll2DRepresentation(const Coll2DRepresentation&) = delete;
    Coll2DRepresentation& operator=(const Coll2DRepresentation&) = delete;

    // The Specific interface methods
    //

    // @brief Set the collision information for this object. If no precise
    //        information is given, then the AABB is used.
    // @param pi        The precise information for collisions detection.
    //                  THIS CLASS WILL BE THE OWNER OF THE MEMORY FOR THE pi
    // @param bb        The associated bounding box to be used for this object
    // @note This method will also calculate the squared radius to be used
    //
    void
    setCollisionInformation(coll::CollPreciseInfo* pi,
                            const core::AABB& bb = core::AABB());

    // @brief Set the type of Collision object it is. If it is static or dynamic
    //        to be used by the collision handler.
    // @param collType      The type of the object.
    //
    inline void
    setCollision2DType(Coll2DType collType);
    inline Coll2DType
    collision2DType(void) const;

    // @brief Check if this object is being tracked by the collision world or not
    //
    inline bool
    isInCollisionWorld(void) const;

    // @brief Enable / disable collisions for this object.
    // @param enable    set enable?
    //
    inline void
    enableCollisions(bool enable);

    // @brief Check if this object has the collisions enabled or not
    //
    inline bool
    hasCollisionsEnabled(void) const;

    // @brief Return the associated CollObject and Squared radius.
    //
    inline float
    sqrRadius(void) const;
    inline const coll::CollObject&
    collisionObject(void) const;
    inline coll::CollObject*
    collisionObjectPtr(void);


    // The common Interface for all the representations
    //

    // @brief Set this representation enabled / disabled
    //
    inline void
    setEnabled(bool enabled);
    inline bool
    isEnabled(void) const;

    // @brief Set the position for this representation in the world.
    // @param pos       The position where we should put it.
    //
    inline void
    setPosition(const Ogre::Vector3& pos);

    // @brief Set the Orientation for this representation in the world.
    // @param orientation       The orientation.
    //
    inline void
    setOrientation(const Ogre::Quaternion& orientation);

    // @brief Set the scale associated to the world object for this representation.
    // @param scale         The new scale
    //
    inline void
    setScale(const Ogre::Vector3& scale);

    // @brief Set the Mask data to this representation associated to the WorldObject.
    // @param masks     The masks information
    //
    inline void
    setMasks(const MaskData& masks);

    // @brief Set the user defined pointer used for this world object for this
    //        representation.
    // @param userDef       The user def pointer.
    //
    inline void
    setUserDef(void* userDef);

protected:
    // All the world objects will share the same CollisionHandler object (world)
    //
    static coll::CollisionHandler* sCollHandler;

private:
    Coll2DType mCollType;
    coll::CollObject* mCollObj;
    float mSqrRadius;
};



////////////////////////////////////////////////////////////////////////////////
//                              INLINE STUFF                                  //
////////////////////////////////////////////////////////////////////////////////

inline void
Coll2DRepresentation::setCollisionHandler(coll::CollisionHandler* collHandler)
{
    ASSERT(collHandler);
    sCollHandler = collHandler;
}

inline void
Coll2DRepresentation::setCollision2DType(Coll2DType collType)
{
    ASSERT(!isInCollisionWorld() && "We cannot change the type of the object when"
        " the object is already in the collision world.");
    mCollType = collType;
}
inline Coll2DRepresentation::Coll2DType
Coll2DRepresentation::collision2DType(void) const
{
    return mCollType;
}

inline bool
Coll2DRepresentation::isInCollisionWorld(void) const
{
    return mCollObj->isInCollisionWorld();
}

inline void
Coll2DRepresentation::enableCollisions(bool enable)
{
    mCollObj->setCollisionsEnabled(enable);
}

inline bool
Coll2DRepresentation::hasCollisionsEnabled(void) const
{
    return mCollObj->isCollisionsEnabled();
}

inline float
Coll2DRepresentation::sqrRadius(void) const
{
    return mSqrRadius;
}
inline const coll::CollObject&
Coll2DRepresentation::collisionObject(void) const
{
    return *mCollObj;
}
inline coll::CollObject*
Coll2DRepresentation::collisionObjectPtr(void)
{
    return mCollObj;
}



// The common Interface for all the representations
//

inline void
Coll2DRepresentation::setEnabled(bool enabled)
{
    if (isInCollisionWorld() == enabled) {
        // we have nothing to do here
        return;
    }

    // we need to add this collision object to the collisions world
    if (mCollType == Coll2DType::Coll2D_Dynamic) {
        if (enabled) {
            sCollHandler->coAddDynamic(mCollObj);
        } else {
            sCollHandler->coRemoveDynamic(mCollObj);
        }
    } else {
        // static
        if (enabled) {
            sCollHandler->coAddStatic(mCollObj);
        } else {
            sCollHandler->coRemoveStatic(mCollObj);
        }
    }
}

inline bool
Coll2DRepresentation::isEnabled(void) const
{
    return isInCollisionWorld();
}

inline void
Coll2DRepresentation::setPosition(const Ogre::Vector3& pos)
{
    mCollObj->setPosition(core::Vector2(pos.x, pos.y));
}

inline void
Coll2DRepresentation::setOrientation(const Ogre::Quaternion& orientation)
{
    mCollObj->setAngle(orientation.getRoll().valueRadians());
}

inline void
Coll2DRepresentation::setScale(const Ogre::Vector3& scale)
{
    ASSERT(false && "This feature is not implemented right now and I think that"
        " Box2D is not supporting rescale the objects. So to be able to do this"
        " we will need to implement this by hand. Note that scaling in runtime"
        " is the problem, and not scaling the element when constructing it.");
}

inline void
Coll2DRepresentation::setMasks(const MaskData& masks)
{
    mCollObj->setMask(masks.filterMask);
}

inline void
Coll2DRepresentation::setUserDef(void* userDef)
{
    mCollObj->setUserDef(userDef);
}

} /* namespace w_o */
#endif /* COLL2DREPRESENTATION_H_ */
