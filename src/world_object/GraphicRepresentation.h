/*
 * GraphicRepresentation.h
 *
 *  Created on: Jan 8, 2014
 *      Author: agustin
 */

#ifndef GRAPHICREPRESENTATION_H_
#define GRAPHICREPRESENTATION_H_

#include <OgreSceneNode.h>
#include <OgreEntity.h>

#include <debug/DebugUtil.h>

#include "MaskData.h"


namespace w_o {

class GraphicRepresentation
{
public:
    GraphicRepresentation() : mSceneNode(0), mEntity(0) {};
    ~GraphicRepresentation() {};

    // avoid copy constructor
    //
    GraphicRepresentation(const GraphicRepresentation&) = delete;
    GraphicRepresentation& operator=(const GraphicRepresentation&) = delete;

    // The Specific interface methods
    //

    // @brief Set the SceneNode used for the graphical representation.
    // @param node      The scene node
    // @note This node will not be removed when we destroy this class.
    //
    inline void
    setSceneNode(Ogre::SceneNode* node);
    inline Ogre::SceneNode*
    sceneNode(void);
    inline const Ogre::SceneNode*
    sceneNode(void) const;

    // @brief Set the entity representing this world object graphically.
    // @param entity    The entity. This entity should be added to the sceneNode
    //                  externally or just call attachToNode.
    // @note the entity will not be destroyed when this class is destroyed.
    //
    inline void
    setEntity(Ogre::Entity* entity);
    inline Ogre::Entity*
    entity(void);
    inline const Ogre::Entity*
    entity(void) const;

    // @brief Set this graphical interface visible or invisible.
    //
    inline void
    setVisible(bool visible);
    inline bool
    isVisible(void) const;


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

private:
    Ogre::SceneNode* mSceneNode;
    Ogre::Entity* mEntity;

};











////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

// The Specific interface methods
//

inline void
GraphicRepresentation::setSceneNode(Ogre::SceneNode* node)
{
    mSceneNode = node;
}
inline Ogre::SceneNode*
GraphicRepresentation::sceneNode(void)
{
    return mSceneNode;
}
inline const Ogre::SceneNode*
GraphicRepresentation::sceneNode(void) const
{
    return mSceneNode;
}

inline void
GraphicRepresentation::setEntity(Ogre::Entity* entity)
{
    mEntity = entity;
}
inline Ogre::Entity*
GraphicRepresentation::entity(void)
{
    return mEntity;
}
inline const Ogre::Entity*
GraphicRepresentation::entity(void) const
{
    return mEntity;
}

inline void
GraphicRepresentation::setVisible(bool visible)
{
    if (mSceneNode) {
        mSceneNode->setVisible(visible);
    }
}
inline bool
GraphicRepresentation::isVisible(void) const
{
    return !(mSceneNode == 0 || mEntity == 0 || mEntity->isVisible());
}


// The common Interface for all the representations
//
inline void
GraphicRepresentation::setEnabled(bool enabled)
{
    // TODO: here probably we can just remove it from the graph but we will
    //       need the scene manager or the root scene node to be able to do this
    //       for each Graphicrepresentation.
    //       Check issue #387
    //
    setVisible(enabled);
}
inline bool
GraphicRepresentation::isEnabled(void) const
{
    // TODO: issue #387 the same above
    //
    return isVisible();
}

inline void
GraphicRepresentation::setPosition(const Ogre::Vector3& pos)
{
    ASSERT(mSceneNode);
    mSceneNode->setPosition(pos);
}


inline void
GraphicRepresentation::setOrientation(const Ogre::Quaternion& orientation)
{
    ASSERT(mSceneNode);
    mSceneNode->setOrientation(orientation);
}

inline void
GraphicRepresentation::setScale(const Ogre::Vector3& scale)
{
    ASSERT(mSceneNode);
    mSceneNode->setScale(scale);
}

inline void
GraphicRepresentation::setMasks(const MaskData& masks)
{
    ASSERT(mEntity);
    mEntity->setDefaultQueryFlags(masks.filterMask);
}

inline void
GraphicRepresentation::setUserDef(void* userDef)
{
    ASSERT(mEntity);
    Ogre::UserObjectBindings& binding = mEntity->getUserObjectBindings();
    binding.setUserAny(Ogre::Any(userDef));
}

} /* namespace w_o */
#endif /* GRAPHICREPRESENTATION_H_ */
