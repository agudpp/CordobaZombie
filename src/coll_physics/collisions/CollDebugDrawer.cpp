/*
 * CollDebugDrawer.cpp
 *
 *  Created on: Jan 9, 2014
 *      Author: agustin
 */

#include "CollDebugDrawer.h"

#include <Ogre.h>

#include <collisions/Box2D/Collision/Shapes/b2Shape.h>
#include <collisions/Box2D/Collision/Shapes/b2CircleShape.h>
#include <collisions/Box2D/Collision/Shapes/b2EdgeShape.h>
#include <collisions/Box2D/Collision/Shapes/b2ChainShape.h>
#include <collisions/Box2D/Collision/Shapes/b2PolygonShape.h>
#include <collisions/Box2D/Collision/b2Distance.h>
#include <collisions/Box2D/Common/b2Math.h>

#include <debug/DebugUtil.h>


// Auxiliary macro used to get the box2D shape
//
#define GETBOXSHAPE(co, shapeType, shapeVarName, shapeBox2DType) ASSERT(co);\
    ASSERT(co->preciseInfo());\
    ASSERT(co->preciseInfo()->shape);\
    const shapeType* shapeVarName = \
        reinterpret_cast<const shapeType*>(co->preciseInfo()->shape);\
    ASSERT(shapeVarName->GetType() == shapeBox2DType);



namespace coll {

////////////////////////////////////////////////////////////////////////////////
void
CollDebugDrawer::drawCircle(const CollObject* co)
{
    // we need to ensure that we have the shape
    GETBOXSHAPE(co, b2CircleShape, circle, b2Shape::Type::e_circle);

    // we will draw the circle using N lines.
    static const unsigned int N = 15;
    Ogre::Vector3 points[N];
    const b2Vec2& center = circle->GetVertex(0);
    const float32 radius = circle->m_radius;
    const float step = (2*M_PI) / static_cast<float>(N);

    // calculate the points
    for (int i = 0; i < N; ++i) {
        points[i] = Ogre::Vector3(radius * std::cos(i * step) + center.x,
                                  radius * std::sin(i * step) + center.y,
                                  0.f);
    }

    // now draw the lines
    for (unsigned int i = 0; i < N; ++i) {
        mLines->position(points[i]);
        mLines->colour(mColour);
    }
    // draw the last line
    mLines->position(points[N-1]);
    mLines->colour(mColour);
    mLines->position(points[0]);
    mLines->colour(mColour);

}
////////////////////////////////////////////////////////////////////////////////
void
CollDebugDrawer::drawEdge(const CollObject* co)
{
    ASSERT(false && "Not implemented yet! Should be trival, probably the code "
        "above works perfectly!");

    GETBOXSHAPE(co, b2EdgeShape, edge, b2Shape::Type::e_edge);
    const b2Vec2& p1 = edge->m_vertex1;
    const b2Vec2& p2 = edge->m_vertex2;
    mLines->position(Ogre::Vector3(p1.x, p1.y, 0));
    mLines->colour(mColour);
    mLines->position(Ogre::Vector3(p2.x, p2.y, 0));
    mLines->colour(mColour);
}

////////////////////////////////////////////////////////////////////////////////
void
CollDebugDrawer::drawPolygon(const CollObject* co)
{
    GETBOXSHAPE(co, b2PolygonShape, poly, b2Shape::Type::e_polygon);

    // draw all the points now
    const unsigned int count = poly->GetChildCount();

    // now draw the lines
    for (unsigned int i = 0; i < count; ++i) {
        const b2Vec2& point = poly->GetVertex(i);
        mLines->position(Ogre::Vector3(point.x, point.y, 0));
        mLines->colour(mColour);
    }
    // draw the last line
    const b2Vec2& last = poly->GetVertex(count-1);
    const b2Vec2& first = poly->GetVertex(0);
    mLines->position(Ogre::Vector3(last.x, last.y, 0));
    mLines->colour(mColour);
    mLines->position(Ogre::Vector3(first.x, first.y, 0));
    mLines->colour(mColour);
}

////////////////////////////////////////////////////////////////////////////////
void
CollDebugDrawer::drawChain(const CollObject* co)
{
    ASSERT(false && "We need to implement this, but it will be trivial, follow"
        " the examples above");
}
////////////////////////////////////////////////////////////////////////////////
void
CollDebugDrawer::drawBoundingBox(const CollObject* co)
{
    ASSERT(co);
    const core::AABB& bb = co->boundingBox();

    // we will draw the bounding box now using 4 lines
    Ogre::Vector3 p1, p2;
    p1.z = 0; p2.z = 0;

    // tl - tr
    p1.x = bb.tl.x; p1.y = bb.tl.y;
    p2.x = bb.br.x; p2.y = bb.tl.y;
    mLines->position(p1); mLines->colour(mColour);
    mLines->position(p2); mLines->colour(mColour);

    // tr - br
    p1.x = bb.br.x; p1.y = bb.tl.y;
    p2.x = bb.br.x; p2.y = bb.br.y;
    mLines->position(p1); mLines->colour(mColour);
    mLines->position(p2); mLines->colour(mColour);

    // br - bl
    p1.x = bb.br.x; p1.y = bb.br.y;
    p2.x = bb.tl.x; p2.y = bb.br.y;
    mLines->position(p1); mLines->colour(mColour);
    mLines->position(p2); mLines->colour(mColour);

    // bl - tl
    p1.x = bb.tl.x; p1.y = bb.br.y;
    p2.x = bb.tl.x; p2.y = bb.tl.y;
    mLines->position(p1); mLines->colour(mColour);
    mLines->position(p2); mLines->colour(mColour);
}



////////////////////////////////////////////////////////////////////////////////
bool
CollDebugDrawer::frameStarted(const Ogre::FrameEvent& evt)
{
    mLines->end();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CollDebugDrawer::frameEnded(const Ogre::FrameEvent& evt)
{
    mLines->beginUpdate(0);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CollDebugDrawer::CollDebugDrawer(Ogre::SceneManager* scm) :
    mEnabled(false)
,   mRootNode(0)
,   mColour(Ogre::ColourValue::Red)
{
    mLines = new Ogre::ManualObject("coll2DDebuglines");
    mLines->setDynamic(true);

    mRootNode = scm->getRootSceneNode()->createChildSceneNode();
    mRootNode->attachObject(mLines);

    static const char * matName = "Coll2DMatName";
    Ogre::MaterialPtr mtl =
        Ogre::MaterialManager::getSingleton().getDefaultSettings()->clone(matName);
    mtl->setReceiveShadows(false);
    mtl->setSceneBlending( Ogre::SBT_TRANSPARENT_ALPHA );
    mtl->setDepthBias( 0.1, 0 );
    Ogre::TextureUnitState * tu = mtl->getTechnique(0)->getPass(0)->createTextureUnitState();
    ASSERT(tu);
    tu->setColourOperationEx( Ogre::LBX_SOURCE1, Ogre::LBS_DIFFUSE );
    mtl->getTechnique(0)->setLightingEnabled(false);

    // we will create lines specified by two points each one.
    //
    mLines->begin(matName, Ogre::RenderOperation::OT_LINE_LIST);
    mLines->position(Ogre::Vector3::ZERO);
    mLines->colour(mColour);
    mLines->position(Ogre::Vector3::ZERO);
    mLines->colour(mColour);
}

CollDebugDrawer::~CollDebugDrawer()
{
    setEnable(false);
    mRootNode->getCreator()->destroySceneNode(mRootNode);
    delete mLines;
}


////////////////////////////////////////////////////////////////////////////////
void
CollDebugDrawer::setEnable(bool enabled)
{
    mEnabled = enabled;
    if (enabled) {
        Ogre::Root::getSingleton().addFrameListener(this);
    } else {
        Ogre::Root::getSingleton().removeFrameListener(this);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
CollDebugDrawer::drawCollObject(const CollObject* co)
{
    if (!mEnabled) {
        return; // do nothing
    }

    if (co == 0) {
        debugWARNING("Null co?\n");
        return;
    }
    // check if we have precise information or not
    if (co->preciseInfo()) {
        // we need to analyze the precise information
        const CollPreciseInfo* pi = co->preciseInfo();
        ASSERT(pi->shape);
        switch (pi->shape->GetType()) {
        case b2Shape::Type::e_chain:    drawChain(co);      break;
        case b2Shape::Type::e_circle:   drawCircle(co);     break;
        case b2Shape::Type::e_edge:     drawEdge(co);       break;
        case b2Shape::Type::e_polygon: drawPolygon(co);     break;
        default:
            ASSERT(false && "Type not supported? mmm something is wrong here.. "
                "Probably we update Box2D with new types and we are not supporting"
                " it to being drawn? Add the new type!");
        }
    } else {
        // draw directly the bounding box
        drawBoundingBox(co);
    }
}




} /* namespace coll */
