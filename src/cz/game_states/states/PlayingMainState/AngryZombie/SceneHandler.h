/*
 * SceneHandler.h
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#ifndef SCENEHANDLER_H_
#define SCENEHANDLER_H_


#include <string.h>
#include <string>
#include <functional>

#include <OgreSceneManager.h>
#include <OgreAxisAlignedBox.h>

#include <debug/DebugUtil.h>
#include <types/DataHolder.h>
#include <physics/DynamicWorld.h>
#include <physics/BulletShape.h>
#include <physics/BulletObject.h>

#include "Player.h"
#include "PhysicsHandler.h"


namespace demo_app {

class SceneHandler
{
public:

    struct Data {
        Player* player;
        PhysicsHandler* physicsHandler;
        physics::DynamicWorld* dynamicWorld;
        Ogre::SceneManager* sceneManager;

        Data()
        {
            memset(this, 0, sizeof(*this));
        }
    };
public:
    SceneHandler();
    ~SceneHandler();

    // @brief Set the data we need to be able to work correctly here.
    //
    void
    setData(const Data& data);

    // @brief Parse the scenes information.
    // @param scenes        File path to the scenes information
    //
    bool
    parseSceneInformation(const std::string& scenes);

    // @brief Configure / reset the current scene.
    //
    void
    configureCurrentScene(void);

    // @brief Add a physic object to be tracked (and freed) by this class later.
    //        This method will add the physic object to the dynamic world too
    //
    void
    addPhysicObject(PhysicObject* po);

    // @brief Go to next scene. This method will not configure the scene.
    // @return true if we could go to next scene or not
    //
    bool
    goToNextScene(void);

private:
    // avoid copying
    SceneHandler(const SceneHandler&);
    SceneHandler& operator=(const SceneHandler&);

    // Helper internal class to hold elements and free its memory
    //
    struct PhysicObjectHolder {
        physics::DynamicWorld* dynamicWorld;
        core::DataHolder<physics::BulletShape*> shapeHolder;
        core::DataHolder<btRigidBody*> rigidBodyHolder;
        core::DataHolder<Ogre::SceneNode*> nodes;
        core::DataHolder<physics::BulletObject*> bulletObjects;
        core::DataHolder<PhysicObject*> physicObjects;

        // @brief Add a bullet shape element to be destroyed
        //
        inline void
        addShape(physics::BulletShape* shape)
        {
            shapeHolder.add(shape);
        }
        inline void
        addRigidBody(btRigidBody* rb)
        {
            rigidBodyHolder.add(rb);
        }
        inline void
        addNode(Ogre::SceneNode* node)
        {
            nodes.add(node);
        }
        inline void
        addBulletObject(physics::BulletObject* bo)
        {
            bulletObjects.add(bo);
        }
        inline void
        addPhysicObject(PhysicObject* po)
        {
            physicObjects.add(po);
        }

        // clear and free all the data.
        inline void
        clearAll(void)
        {
            rigidBodyHolder.freeAll();
            shapeHolder.freeAll();
            nodes.freeAll();
            bulletObjects.freeAll();
            physicObjects.freeAll();
        }

        // destructor method for dynamic objects
        void
        destroyRididBodies(btRigidBody* rb)
        {
            ASSERT(dynamicWorld);
            if (rb == 0) return;
            dynamicWorld->bulletDynamicWorld().removeRigidBody(rb);
            delete rb;
        }
        // destructor method for nodes
        static void
        destroyNode(Ogre::SceneNode* node)
        {
            if (node == 0) return;
            Ogre::SceneManager* manager = node->getCreator();
            // get all the objects attached and destroy them
            Ogre::SceneNode::ObjectIterator it = node->getAttachedObjectIterator();
            while (it.hasMoreElements()) {
                Ogre::MovableObject* obj = it.getNext();
                if (obj) {
                    manager->destroyMovableObject(obj);
                }
            }
            node->removeAndDestroyAllChildren();
            if (manager == 0) return;
            manager->destroySceneNode(node);
        }

        PhysicObjectHolder() :
            dynamicWorld(0)
        ,   rigidBodyHolder(core::DataHolder<btRigidBody*>::DestructorMethod(
                            std::bind(&PhysicObjectHolder::destroyRididBodies,
                                      this,
                                      std::placeholders::_1)))
        ,   nodes(core::DataHolder<Ogre::SceneNode*>::DestructorMethod(destroyNode))
        {
        }
    };

    // @brief Construct a box from a bounding box, mass and bullet object
    //
    void
    createBox(physics::BulletObject& result,
              const Ogre::AxisAlignedBox& bb,
              float mass);

    // @brief Add a BulletObject into the holder
    //
    void
    addBulletObjectToHolder(physics::BulletObject& object);

    // @brief Construct the limits of the scene from a bb
    //
    void
    buildWorldPhysicsLimits(const Ogre::AxisAlignedBox& bb);

private:
    Data mData;
    PhysicObjectHolder mHolder;
    PhysicsHandler mPhysicsHandler;
};

} /* namespace demo_app */
#endif /* SCENEHANDLER_H_ */
