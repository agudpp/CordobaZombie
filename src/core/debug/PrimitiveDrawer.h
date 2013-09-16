/*
 * PrimitiveDrawer.h
 *
 *  Created on: Jun 25, 2013
 *      Author: agustin
 */

#ifndef PRIMITIVEDRAWER_H_
#define PRIMITIVEDRAWER_H_


#include <vector>
#include <memory>

#include <OgreManualObject.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreVector4.h>
#include <OgreSceneManager.h>
#include <OgreMaterial.h>

#include <types/basics.h>
#include <math/Vec2.h>
#include <math/AABB.h>

#include "DebugUtil.h"

namespace core {

////////////////////////////////////////////////////////////////////////////////

// Define the interface of all the primitives
struct Primitive {
    union {
        Ogre::Entity* ent;
        Ogre::ManualObject* manual;
    } obj;

    Primitive(Ogre::SceneNode* n = 0, Ogre::Entity* m = 0) :
        node(n), isManual(false) {obj.ent = m; obj.ent->setQueryFlags(0);}
    Primitive(Ogre::SceneNode* n = 0, Ogre::ManualObject* m = 0) :
        node(n), isManual(true) {obj.manual = m; obj.manual->setQueryFlags(0);}

    Ogre::SceneNode *node;


    // @brief Helper method to hide/show the primitive
    //
    inline void
    setVisible(bool visible)
    {
        ASSERT(node);
        node->setVisible(visible);
    }
    inline void
    flipVisibility()
    {
        ASSERT(node);
        return node->flipVisibility();
    }

    // @brief Set an specific color for this Primitive
    // @param color     The RGBA values to be set (between 0 and 1)
    //
    void
    setColor(const Ogre::ColourValue& color);

    // @brief Set the alpha for this element
    // @param alpha     The alpha value [0,1]
    //
    void
    setAlpha(Ogre::Real alpha);

    // @brief translate the primitive
    // @param t     The translation vector
    inline void
    translate(const Ogre::Vector3& t)
    {
        ASSERT(node);
        node->translate(t);
    }

    // internal purposes
    unsigned int id;
    bool isManual;
};

////////////////////////////////////////////////////////////////////////////////


class PrimitiveDrawer
{
public:
    // Ugly singleton, but is debug, doesn't matter!
    //
    static PrimitiveDrawer& instance(void)
    {
        static PrimitiveDrawer instance;
        return instance;
    }

    ~PrimitiveDrawer();

    // @brief Create a new box object
    // @param position  The center position of the box
    // @param sizes     The sizes of the box
    // @param color     The color to be used
    // @param offset    Move each vertex the offset value (this way the box
    //                  will not be centered).
    // @return the allocated Primitive* (DO NOT DELETE manually this memory)
    //
    Primitive*
    createBox(const Ogre::Vector3& center,
              const Ogre::Vector3& sizes,
              const Ogre::Vector3& offset,
              const Ogre::ColourValue& color = Ogre::ColourValue::White);
    Primitive*
    createBox(const Ogre::Vector3& center,
              const Ogre::Vector3& sizes,
              const Ogre::ColourValue& color = Ogre::ColourValue::White);

    // @brief Create a new box2D object
    // @param bb        The bounding box to draw
    // @param color     The color to be used
    // @return the allocated Primitive* (DO NOT DELETE manually this memory)
    //
    Primitive*
    createBox(const core::AABB& bb,
              const Ogre::ColourValue& color = Ogre::ColourValue::White);

    // @brief Create a finite plane object (with normal pointing Y+)
    // @param center    The center position of the plane
    // @param dims      The dimensions (width and height of the plane).
    // @param color     The color of the plane
    // @return the allocated Primitive* (DO NOT DELETE manually this memory)
    //
    Primitive*
    createPlane(const Ogre::Vector3& center,
                const Ogre::Vector2& dims,
                const Ogre::ColourValue& color = Ogre::ColourValue::White);

    // @brief Create a sphere
    // @param center    The center position
    // @param radius    The radius
    // @param color     The color
    // @return the allocated Primitive* (DO NOT DELETE manually this memory)
    //
    Primitive*
    createSphere(const Ogre::Vector3& center,
                 Ogre::Real radius,
                 const Ogre::ColourValue& color = Ogre::ColourValue::White);

    // @brief Create a line with a given color
    // @param p1        first point of the line
    // @param p2        second point of the line
    // @param color     The color
    // @return the allocated Primitive* (DO NOT DELETE manually this memory)
    //
    Primitive*
    createLine(const Ogre::Vector3& p1,
               const Ogre::Vector3& p2,
               const Ogre::ColourValue& color = Ogre::ColourValue::White);


    // @brief Create a multiline object from a list of points
    // @param points    The list of points that will construct the multiline
    // @param color     The color
    // @return the allocated Primitive* (DO NOT DELETE manually this memory)
    //
    Primitive*
    createMultiline(const std::vector<Ogre::Vector3>& points,
                    const Ogre::ColourValue& color = Ogre::ColourValue::White);

    // @brief Create an 3 axis where all of the lines should be pointing to the
    //        positive values (R,0,0), (0,R,0), (0,0,R) where R is the size
    //        of the lines and with colors:
    //        X = red, Y = green, Z = blue,
    // @param position  The position where we want to create the axis
    // @param r         The lenght of the lines (optional)
    //
    Primitive*
    create3DAxis(const Ogre::Vector3& position, Ogre::Real r = 15.0f);

    // @brief Delete or remove an allocated primitive
    // @param primitive The previously allocated primitive
    //
    void
    deletePrimitive(Primitive* primitive);

    // @brief This method is used to get a new fresh color (different from the
    //        one obtained when calling this method the last time).
    // @returns the new fresh colour
    //
    Ogre::ColourValue
    getFreshColour(void);

private:
    PrimitiveDrawer();
    PrimitiveDrawer(const PrimitiveDrawer&);
    PrimitiveDrawer& operator=(const PrimitiveDrawer&);

private:
    typedef std::shared_ptr<Primitive> PrimitivePtr;
    typedef std::vector<PrimitivePtr> PrimitivePtrVec;


    PrimitivePtrVec mPrimitives;
    Ogre::SceneManager* mSceneMngr;
    Ogre::MaterialPtr mBaseMat;
    unsigned int mLastColourCount;
};

} /* namespace core */
#endif /* PRIMITIVEDRAWER_H_ */
