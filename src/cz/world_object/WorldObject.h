/*
 * WorldObject.h
 *
 *  Created on: Sep 7, 2013
 *      Author: agustin
 */

#ifndef WORLDOBJECT_H_
#define WORLDOBJECT_H_


#include <OgreEntity.h>
#include <OgreSceneNode.h>

#include <debug/DebugUtil.h>
#include <collisions/CollisionHandler.h>
#include <collisions/CollObject.h>
#include <collisions/CollDefines.h>
#include <math/Vec2.h>


// Forward
//
namespace coll {
class CollPreciseInfo;
}

namespace cz {


// @brief This will represent all the objects in the scene but using only the
//        2D logic (not 3D logic for example for physics).
//        Since this project (CZ0.1) is little and the physics will be handled
//        outside this as other thing then we will have no problems..
//
// @note  That we are not using a virtual destructor for this objects, so if
//        you allocate someone that inherits and you will handle it at this level
//        then some memory leaks could occur.
//


class WorldObject
{
public:

    // @brief Method used to configure the the Collision handler to be used
    //        for all the world objects.
    // @param collHandler       The handler (should be != 0)
    // @note This method should be called before the construction of any
    //       WorldObject
    //
    static inline void
    setCollisionHandler(coll::CollisionHandler* collHandler);
public:
    // @brief The constructor of an object will automatically create a collision
    //        object (using the already set CollisionHandler).
    //        Note that the CollObject will be not inserted in the system until
    //        you ask for that.
    //        We will also automatically set the userDef of the collObject to point
    //        this instance
    //
    WorldObject();

    // @brief Will automatically destroy the associated collision object.
    //
    ~WorldObject();

    ////////////////////////////////////////////////////////////////////////////
    // Ogre stuff methods
    //

    // @brief Configure the object with the ogre stuff (SceneNode and Entity).
    //        The node should be already added to the scene and the entity
    //        should be attached to the scene node.
    //        This method will do nothing to the node nor entity.
    // @param node      The ogre scene node
    // @param entity    The ogre entity associated to this object.
    // @note this method will change the position of the elements in the world
    //
    inline void
    setOgreStuff(Ogre::SceneNode* node, Ogre::Entity* entity);
    inline Ogre::SceneNode*
    sceneNode(void);
    inline const Ogre::SceneNode*
    sceneNode(void) const;
    inline Ogre::Entity*
    entity(void);
    inline const Ogre::Entity*
    entity(void) const;

    ////////////////////////////////////////////////////////////////////////////
    // Collision stuff methods
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

    // @brief Make this object static / dynamic object.
    //        This method will add the already configured object to the
    //        collision world with as static / dynamic element.
    // @param isStatic  if true, the element will be treated as static (and note
    //                  that you cannot move this object anymore).
    //                  if false, the object will be treated as dynamic one.
    // @note YOU SHOULDN't call this method more than once
    //
    void
    setCollisionType(bool isStatic);

    // @brief Remove the element completely from the collision world. After
    //        calling this method you will be able to add it again as dynamic /
    //        static one.
    //
    void
    removeFromCollWorld(void);

    // @brief Check if this object is being tracked by the collision world or not
    //
    inline bool
    isInCollisionWorld(void) const;

    // @brief Check if is an static object or not
    //
    inline bool
    isStatic(void) const;

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

    // @brief Set / get the masks to be used for this object in the 2D world
    // @param mask      The mask to be used
    //
    inline void
    setCollMask(coll::mask_t mask);
    inline coll::mask_t
    collMask(void) const;


    ////////////////////////////////////////////////////////////////////////////
    // Movement stuff methods
    //

    // @brief Configure the height of this object. This height will be used
    //        to potitionate the entity height units above the floor.
    // @param height        The height of the object
    //
    inline void
    setObjectHeight(float height);
    inline float
    objectHeight(void) const;

    // @brief Method used to get the height of the floor in the place where this
    //        object is.
    //
    inline float
    floorHeight(void) const;

    // @brief Set / get the position in the world (2D world).
    // @param pos   The position
    //
    inline void
    setPosition(const core::Vector2& pos);
    inline void
    setPosition(const Ogre::Vector3& pos);
    inline void
    setPosition(float x, float y);
    inline core::Vector2
    position(void) const;

    // @brief Set / get the 3D position.
    // @parma pos   The position
    //
    inline void
    set3DPosition(const Ogre::Vector3& pos);
    inline const Ogre::Vector3&
    position3D(void) const;

    // @brief Translations for 2D and 3D
    // @param tvec  The translation vector
    //
    inline void
    translate(const core::Vector2& tvec);
    inline void
    translate(float x, float y);
    inline void
    translate(const Ogre::Vector3& tvec);
    inline void
    translate3D(const Ogre::Vector3& tvec);

    // @brief Advance the object taking into account the direction that is
    //        pointing to. This will only be used for 2D transformation.
    //        This is the same than translate but using local coordinates axis
    // @param avec      The "advance" vector
    //
    inline void
    advance(const core::Vector2& avec);
    inline void
    advance(float x, float y);
    inline void
    advance(const Ogre::Vector3& avec);

    // @brief Rotate the object in the 2D world (this is the xy plane for ogre)
    // @param r     Radians angle to rotate the object
    //
    inline void
    rotate(float r);
    inline void
    rotate(const Ogre::Quaternion& q);

    // @brief Set a particular direction for this object. Only valid for 2D
    // @param d     The new direction of the object.
    //
    inline void
    setDirection(const core::Vector2& d);
    inline void
    setDirection(const Ogre::Vector3& d);
    inline const core::Vector2&
    getNormalizedDir(void) const;

    // @brief Make the object look at an specific position.
    // @param pos   The position that we want to see.
    //
    inline void
    lookAt(const core::Vector2& pos);
    inline void
    lookAt(float x, float y);
    inline void
    lookAt(const Ogre::Vector3& pos);


protected:
    // All the world objects will share the same CollisionHandler object (world)
    //
    static coll::CollisionHandler* sCollHandler;

protected:
    // Ogre particular stuff
    Ogre::Entity* mEntity;
    Ogre::SceneNode* mNode;

    // 2D collision system stuff
    coll::CollObject* mCollObj;
    float mSqrRadius;

    // movement stuff
    core::Vector2 mNormalizedDir;
    float mHeight;

private:
    // Avoid copying this object
    //
    WorldObject(const WorldObject&);
    WorldObject& operator=(const WorldObject&);

    // @brief Update the normalized direction from the current direction of
    //        the ogre node
    //
    inline void
    updateDirectionFromNode(void);
};







////////////////////////////////////////////////////////////////////////////////
//                              INLINE STUFF                                  //
////////////////////////////////////////////////////////////////////////////////

inline void
WorldObject::setCollisionHandler(coll::CollisionHandler* collHandler)
{
    ASSERT(collHandler);
    sCollHandler = collHandler;
}

inline void
WorldObject::updateDirectionFromNode(void)
{

    Ogre::Vector3 d = mNode->getOrientation() * Ogre::Vector3::UNIT_Y;
    mNormalizedDir.x = d.x;
    mNormalizedDir.y = d.y;
    mNormalizedDir.normalize();
    // TODO: we will not implement this since is not necessary for this
    //       project and we don't want to decrease the performance :p.
    //
//    mCollObj->setAngle(mNode->getOrientation().getYaw());
}

////////////////////////////////////////////////////////////////////////////
// Ogre stuff methods
//

inline void
WorldObject::setOgreStuff(Ogre::SceneNode* node, Ogre::Entity* entity)
{
    ASSERT(mNode == 0);
    ASSERT(mEntity == 0);
    ASSERT(node);
    ASSERT(entity);

    mNode = node;
    mEntity = entity;
}

inline Ogre::SceneNode*
WorldObject::sceneNode(void)
{
    return mNode;
}
inline const Ogre::SceneNode*
WorldObject::sceneNode(void) const
{
    return mNode;
}
inline Ogre::Entity*
WorldObject::entity(void)
{
    return mEntity;
}
inline const Ogre::Entity*
WorldObject::entity(void) const
{
    return mEntity;
}

////////////////////////////////////////////////////////////////////////////
// Collision stuff methods
//

inline bool
WorldObject::isInCollisionWorld(void) const
{
    ASSERT(mCollObj);
    return mCollObj->isInCollisionWorld();
}

inline bool
WorldObject::isStatic(void) const
{
    ASSERT(mCollObj);
    return mCollObj->isStatic();
}

inline void
WorldObject::enableCollisions(bool enable)
{
    ASSERT(mCollObj);
    mCollObj->setCollisionsEnabled(enable);
}

inline bool
WorldObject::hasCollisionsEnabled(void) const
{
    ASSERT(mCollObj);
    return mCollObj->isCollisionsEnabled();
}

inline float
WorldObject::sqrRadius(void) const
{
    return mSqrRadius;
}

inline const coll::CollObject&
WorldObject::collisionObject(void) const
{
    ASSERT(mCollObj);
    return *mCollObj;
}
inline coll::CollObject*
WorldObject::collisionObjectPtr(void)
{
    return mCollObj;
}

inline void
WorldObject::setCollMask(coll::mask_t mask)
{
    ASSERT(mCollObj);
    mCollObj->setMask(mask);
}
inline coll::mask_t
WorldObject::collMask(void) const
{
    ASSERT(mCollObj);
    return mCollObj->mask();
}


////////////////////////////////////////////////////////////////////////////
// Movement stuff methods
//

inline void
WorldObject::setObjectHeight(float height)
{
    ASSERT(height > 0.f);
    mHeight = height;
}
inline float
WorldObject::objectHeight(void) const
{
    return mHeight;
}

inline float
WorldObject::floorHeight(void) const
{
    // now hardcoded to be 0
    return 0.f;
}

inline void
WorldObject::setPosition(const core::Vector2& pos)
{
    ASSERT(mCollObj);
    // do translation
    const core::Vector2 tvec = pos - mCollObj->center();
    translate(tvec);
}
inline void
WorldObject::setPosition(const Ogre::Vector3& pos)
{
    ASSERT(mCollObj);
    // do translation
    core::Vector2 tvec(pos.x, pos.y);
    tvec -= mCollObj->center();
    translate(tvec);

}
inline void
WorldObject::setPosition(float x, float y)
{
    ASSERT(mCollObj);
    // do translation
    core::Vector2 tvec(x,y);
    tvec -= mCollObj->center();
    translate(tvec);

}
inline core::Vector2
WorldObject::position(void) const
{
    ASSERT(mCollObj);
    return mCollObj->center();
}

inline void
WorldObject::set3DPosition(const Ogre::Vector3& pos)
{
    ASSERT(mNode);
    ASSERT(mCollObj);
    mNode->setPosition(pos);
    mCollObj->setPosition(core::Vector2(pos.x, pos.y));
}
inline const Ogre::Vector3&
WorldObject::position3D(void) const
{
    ASSERT(mNode);
    return mNode->getPosition();
}

// @brief Translations for 2D and 3D
// @param tvec  The translation vector
//
inline void
WorldObject::translate(const core::Vector2& tvec)
{
    ASSERT(mNode);
    ASSERT(mCollObj);
    mCollObj->translate(tvec);
    // put the right position of the node
    const core::Vector2& pos = mCollObj->center();
    mNode->setPosition(pos.x, pos.y, floorHeight() + mHeight);
}
inline void
WorldObject::translate(float x, float y)
{
    translate(core::Vector2(x,y));
}
inline void
WorldObject::translate(const Ogre::Vector3& tvec)
{
    translate(core::Vector2(tvec.x, tvec.y));
}

inline void
WorldObject::translate3D(const Ogre::Vector3& tvec)
{
    ASSERT(mCollObj);
    ASSERT(mNode);
    mCollObj->translate(core::Vector2(tvec.x,tvec.y));
    mNode->translate(tvec);
}

inline void
WorldObject::advance(const core::Vector2& avec)
{
    ASSERT(mNode);
    ASSERT(mCollObj);
    // translate the 3D using ogre logic and then translate the coll object
    // to where the node is
    Ogre::Vector3 oldPos = mNode->getPosition();
    mNode->translate(avec.x, avec.y, floorHeight() + mHeight);
    oldPos -= mNode->getPosition();

    // move coll object
    mCollObj->translate(core::Vector2(oldPos.x, oldPos.y));
}
inline void
WorldObject::advance(float x, float y)
{
    advance(core::Vector2(x,y));
}
inline void
WorldObject::advance(const Ogre::Vector3& avec)
{
    advance(core::Vector2(avec.x,avec.y));
}

inline void
WorldObject::rotate(float r)
{
    ASSERT(mNode);
    mNode->rotate(Ogre::Vector3::UNIT_Z, Ogre::Radian(r));
    // update normalized direction
    updateDirectionFromNode();
}
inline void
WorldObject::rotate(const Ogre::Quaternion& q)
{
    ASSERT(mNode);
    mNode->rotate(q);
    updateDirectionFromNode();
}

inline void
WorldObject::setDirection(const core::Vector2& d)
{
    ASSERT(mNode);
    mNode->setDirection(d.x, d.y, mNode->getPosition().z);
    updateDirectionFromNode();
}
inline void
WorldObject::setDirection(const Ogre::Vector3& d)
{
    ASSERT(mNode);
    mNode->setDirection(d);
    updateDirectionFromNode();
}
inline const core::Vector2&
WorldObject::getNormalizedDir(void) const
{
    return mNormalizedDir;
}

inline void
WorldObject::lookAt(const core::Vector2& pos)
{
    lookAt(Ogre::Vector3(pos.x,pos.y,position3D().z));
}
inline void
WorldObject::lookAt(float x, float y)
{
    lookAt(Ogre::Vector3(x, y, position3D().z));
}
inline void
WorldObject::lookAt(const Ogre::Vector3& pos)
{
    ASSERT(mNode);
    mNode->lookAt(pos, Ogre::SceneNode::TS_WORLD);
    updateDirectionFromNode();
}

} /* namespace cz */
#endif /* WORLDOBJECT_H_ */
