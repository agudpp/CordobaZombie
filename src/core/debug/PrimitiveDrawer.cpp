/*
 * PrimitiveDrawer.cpp
 *
 *  Created on: Jun 25, 2013
 *      Author: agustin
 */

#include "PrimitiveDrawer.h"

#include <OgreRoot.h>
#include <OgreMaterialManager.h>
#include <OgreSubEntity.h>

#include "OgreNameGen.h"

namespace core {

////////////////////////////////////////////////////////////////////////////////
void
Primitive::setColor(const Ogre::ColourValue& color)
{
    const Ogre::String& matName = (isManual) ? obj.manual->getSection(0)->getMaterialName() :
        obj.ent->getSubEntity(0)->getMaterialName();

    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(matName);
    mat->setReceiveShadows(false);
    mat->getTechnique(0)->setLightingEnabled(true);
    mat->getTechnique(0)->getPass(0)->setDiffuse(color.r, color.g, color.b, color.a);
    mat->getTechnique(0)->getPass(0)->setAmbient(color.r, color.g, color.b);
    mat->getTechnique(0)->getPass(0)->setSelfIllumination(color.r, color.g, color.b);
}

////////////////////////////////////////////////////////////////////////////////
void
Primitive::setAlpha(Ogre::Real alpha)
{
    const Ogre::String& matName = (isManual) ? obj.manual->getSection(0)->getMaterialName() :
        obj.ent->getSubEntity(0)->getMaterialName();
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(matName);

    const Ogre::ColourValue& diffuse = mat->getTechnique(0)->getPass(0)->getDiffuse();
    mat->getTechnique(0)->getPass(0)->setDiffuse(diffuse.r, diffuse.g, diffuse.b, alpha);

    const Ogre::ColourValue& specular = mat->getTechnique(0)->getPass(0)->getSpecular();
    mat->getTechnique(0)->getPass(0)->setSpecular(specular.r, specular.g, specular.b, alpha);
}


PrimitiveDrawer::PrimitiveDrawer() :
    mSceneMngr(0)
,   mLastColourCount(0)
{
    Ogre::SceneManagerEnumerator::SceneManagerIterator it =
        Ogre::Root::getSingleton().getSceneManagerIterator();
    mSceneMngr = it.getNext();

    // create base mat
    mBaseMat = Ogre::MaterialManager::getSingleton().create("PrimitiveDrawerBaseMat",
                                                            "General");
    mBaseMat->getTechnique(0)->getPass(0)->setSceneBlending(
            Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
}

PrimitiveDrawer::~PrimitiveDrawer()
{
}

////////////////////////////////////////////////////////////////////////////////
Primitive*
PrimitiveDrawer::createBox(const Ogre::Vector3& center,
                           const Ogre::Vector3& sizes,
                           const Ogre::ColourValue& color)
{
    Ogre::MaterialPtr newMat = mBaseMat->clone(OgreNameGen::getFreshName());
    Ogre::ManualObject* manual = mSceneMngr->createManualObject(OgreNameGen::getFreshName());

    // construct the manual
    manual->begin(newMat->getName());

    const Ogre::Vector3 halfSize = sizes * 0.5f;



    // create the different vertices, first the top four and then the bottom
    // four
    manual->position(Ogre::Vector3(halfSize.x, halfSize.y, halfSize.z));
    manual->position(Ogre::Vector3(-halfSize.x, halfSize.y, halfSize.z));
    manual->position(Ogre::Vector3(-halfSize.x, halfSize.y, -halfSize.z));
    manual->position(Ogre::Vector3(halfSize.x, halfSize.y, -halfSize.z));

    // bottom
    manual->position(Ogre::Vector3(halfSize.x, -halfSize.y, halfSize.z));
    manual->position(Ogre::Vector3(-halfSize.x, -halfSize.y, halfSize.z));
    manual->position(Ogre::Vector3(-halfSize.x, -halfSize.y, -halfSize.z));
    manual->position(Ogre::Vector3(halfSize.x, -halfSize.y, -halfSize.z));

    // set the triangles
    manual->triangle(2, 1, 0); manual->triangle(0, 3, 2);   // top
    manual->triangle(4, 5, 6); manual->triangle(7, 4, 6);   // bottom
    manual->triangle(1, 2, 6); manual->triangle(1, 6, 5);   // left
    manual->triangle(2, 7, 6); manual->triangle(2, 3, 7);
    manual->triangle(3, 4, 7); manual->triangle(4, 3, 0);
    manual->triangle(4, 0, 1); manual->triangle(5, 4, 1);

    manual->end(); // and building the manual

    // create the scene node
    Ogre::SceneNode* node = mSceneMngr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(manual);
    node->setPosition(center);

    PrimitivePtr box(new Primitive(node, manual));
    box->setColor(color);

    box->id = mPrimitives.size();
    mPrimitives.push_back(box);

    return box.get();
}

////////////////////////////////////////////////////////////////////////////////
Primitive*
PrimitiveDrawer::createBox(const core::AABB& bb,
                           const Ogre::ColourValue& color)
{
    core::Vector2 center = bb.center();
    Ogre::Vector3 c(center.x, center.y, 0);
    Ogre::Vector2 dim(bb.getWidth(), bb.getHeight());
    return createPlane(c, dim, color);
}

////////////////////////////////////////////////////////////////////////////////
Primitive*
PrimitiveDrawer::createPlane(const Ogre::Vector3& center,
                             const Ogre::Vector2& dims,
                             const Ogre::ColourValue& color)
{
    Ogre::MaterialPtr newMat = mBaseMat->clone(OgreNameGen::getFreshName());
    Ogre::ManualObject* manual = mSceneMngr->createManualObject(OgreNameGen::getFreshName());

    // construct the manual (2 triangles)
    manual->begin(newMat->getName());

    const Ogre::Vector2 halfDims = dims * .5f;

    // 1---------2
    // |         |
    // |         |
    // 0---------3

    manual->position(-halfDims.x, -halfDims.y, 0);
    manual->position(-halfDims.x, halfDims.y, 0);
    manual->position(halfDims.x, halfDims.y, 0);
    manual->position(halfDims.x, -halfDims.y, 0);

    // create triangles
    manual->triangle(0, 2, 1);
    manual->triangle(3, 2, 0);

    manual->end(); // and building the manual

   // create the scene node
   Ogre::SceneNode* node = mSceneMngr->getRootSceneNode()->createChildSceneNode();
   node->attachObject(manual);
   node->setPosition(center);

   PrimitivePtr plane(new Primitive(node, manual));
   plane->setColor(color);

   plane->id = mPrimitives.size();
   mPrimitives.push_back(plane);

   return plane.get();
}

////////////////////////////////////////////////////////////////////////////////
Primitive*
PrimitiveDrawer::createSphere(const Ogre::Vector3& center,
                              Ogre::Real radius,
                              const Ogre::ColourValue& color)
{

    Ogre::Entity* ent = mSceneMngr->createEntity(Ogre::SceneManager::PrefabType::PT_SPHERE);
    Ogre::SceneNode* node = mSceneMngr->getRootSceneNode()->createChildSceneNode();

    node->attachObject(ent);
    node->setPosition(center);

    PrimitivePtr sphere(new Primitive(node, ent));

    debugERROR("We are not setting the radius of the sphere here... fix this\n");

    return sphere.get();
}

////////////////////////////////////////////////////////////////////////////////
Primitive*
PrimitiveDrawer::createLine(const Ogre::Vector3& p1,
                            const Ogre::Vector3& p2,
                            const Ogre::ColourValue& color)
{
    Ogre::MaterialPtr newMat = mBaseMat->clone(OgreNameGen::getFreshName());
    Ogre::ManualObject* manual = mSceneMngr->createManualObject(OgreNameGen::getFreshName());

    // construct the manual one line
    manual->begin(newMat->getName(), Ogre::RenderOperation::OT_LINE_LIST);

    manual->position(p1);
    manual->position(p2);

    manual->end(); // and building the manual

   // create the scene node
   Ogre::SceneNode* node = mSceneMngr->getRootSceneNode()->createChildSceneNode();
   node->attachObject(manual);

   PrimitivePtr line(new Primitive(node, manual));
   line->setColor(color);

   line->id = mPrimitives.size();
   mPrimitives.push_back(line);

   return line.get();
}


////////////////////////////////////////////////////////////////////////////////
Primitive*
PrimitiveDrawer::createMultiline(const std::vector<Ogre::Vector3>& points,
                                 const Ogre::ColourValue& color)
{
    ASSERT(points.size() >= 2);

    Ogre::MaterialPtr newMat = mBaseMat->clone(OgreNameGen::getFreshName());
    Ogre::ManualObject* manual = mSceneMngr->createManualObject(OgreNameGen::getFreshName());

    // construct the manual one line
    manual->begin(newMat->getName(), Ogre::RenderOperation::OT_LINE_LIST);

    for (core::size_t i = 1, size = points.size(); i < size; ++i) {
        manual->position(points[i-1]);
        manual->position(points[i]);
    }

    manual->end(); // and building the manual

   // create the scene node
   Ogre::SceneNode* node = mSceneMngr->getRootSceneNode()->createChildSceneNode();
   node->attachObject(manual);

   PrimitivePtr line(new Primitive(node, manual));
   line->setColor(color);

   line->id = mPrimitives.size();
   mPrimitives.push_back(line);

   return line.get();
}

////////////////////////////////////////////////////////////////////////////////
Primitive*
PrimitiveDrawer::create3DAxis(const Ogre::Vector3& position, Ogre::Real r)
{
    Ogre::ManualObject* manual = mSceneMngr->createManualObject(OgreNameGen::getFreshName());

    // construct the manual one line
    manual->begin("", Ogre::RenderOperation::OT_LINE_LIST);

    // X axis
    manual->position(position);
    manual->colour(1.0f, 0, 0);
    manual->position(position + Ogre::Vector3(r, 0, 0));
    manual->position(position);

    // Y axis
    manual->colour(0, 1.0f, 0);
    manual->position(position + Ogre::Vector3(0, r, 0));
    manual->position(position);

    // Z axis
    manual->colour(0, 0, 1.0f);
    manual->position(position + Ogre::Vector3(0, 0, r));

    manual->end(); // and building the manual

    manual->getSection(0)->getTechnique()->getPass(0)->setLightingEnabled(false);

   // create the scene node
   Ogre::SceneNode* node = mSceneMngr->getRootSceneNode()->createChildSceneNode();
   node->attachObject(manual);

   PrimitivePtr axis(new Primitive(node, manual));

   axis->id = mPrimitives.size();
   mPrimitives.push_back(axis);

   return axis.get();
}


////////////////////////////////////////////////////////////////////////////////
void
PrimitiveDrawer::deletePrimitive(Primitive* primitive)
{
    ASSERT(primitive);
    ASSERT(primitive->id < mPrimitives.size());

    if (mPrimitives.size() == 1) {
        mPrimitives.clear();
        return;
    }

    // we should remove the primitive from the sceneManager
    if (primitive->isManual) {
        mSceneMngr->destroyManualObject(primitive->obj.manual);
    } else {
        mSceneMngr->destroyEntity(primitive->obj.ent);
    }
    Ogre::SceneNode* parent = primitive->node->getParentSceneNode();
    parent->removeAndDestroyChild(primitive->node->getName());

    // swap with last
    mPrimitives.back()->id = primitive->id;
    mPrimitives[primitive->id] = mPrimitives.back();
    mPrimitives.pop_back();
}

////////////////////////////////////////////////////////////////////////////////
Ogre::ColourValue
PrimitiveDrawer::getFreshColour(void)
{
    const float r = std::rand() % 256;
    const float g = std::rand() % 256;
    const float b = std::rand() % 256;
    return Ogre::ColourValue(r/256.f, g/256.f, b/256.f);
}


} /* namespace core */
