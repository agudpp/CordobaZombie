/*
 * BulletLoader.h
 *
 *  Created on: Oct 22, 2013
 *      Author: agustin
 */

#ifndef BULLETLOADER_H_
#define BULLETLOADER_H_

#include <OgreEntity.h>
#include <OgreSceneNode.h>

#include <bullet/btBulletDynamicsCommon.h>

#include <types/StackVector.h>

#include "BulletObject.h"
#include "DataHolder.h"

namespace physics {

// @brief This class will be used to load all the physics representations for all
//        the objects in the scene given a list of entities and scene nodes.
//        Note that the pair [SceneNode, Entity] represents a unique element in
//        the world. So for each of those pairs we will return an BulletObject
//        associated.
//        We will hold the memory of the elements so you don't have to free
//        any of the memory of the elements (since some of the bullet objects
//        share the shape for example, we cannot just remove all the pointers
//        we have). To remove the memory call freeMemory() method.
//

class BulletLoader
{
public:
    // Define the maximum number of elements we can load at once
    static const unsigned int MAX_SIZE = 1024;

    // Define a helper structure to define the pair of elements that defines
    // uniquely an object in the world
    //
    struct Info {
        Ogre::SceneNode* node;
        Ogre::Entity* object;
        float mass;

        Info(Ogre::SceneNode* n = 0, Ogre::Entity* o = 0, float m = 0.f) :
            node(n), object(o), mass(m)
        {}
    };

    typedef core::StackVector<Info, MAX_SIZE> InfoVec;
    typedef core::StackVector<BulletObject*, MAX_SIZE> BulletObjectVec;

public:
    // @brief Note that if you remove the BulletLoader instance you will be
    //        freeing all the data loaded (BulletObjects / Shapes / RigidBody / etc).
    //
    BulletLoader();
    ~BulletLoader();

    // @brief Construct all the associated BulletObjects for a given list of
    //        unique ogre elements in the scene.
    //        This function return the same number of bullet than the input
    //        (size(output) == size(input) && out[i] =(represents)=> input[i]).
    // @param ogreElements      The ogre elements list
    // @param bulletElements    The resulting representation for each element.
    // @return true on success | false otherwise
    // @note That you should call this method once you have already loaded all
    //       the graphics data (ogre data), if not, this will probably load
    //       twice the same data (shape for example).
    //
    bool
    construct(const InfoVec& ogreElements, BulletObjectVec& bulletElements);

    // @brief Return all the BulletObjects we are holding now.
    // @param size      Here we will return the number of BulletObject instances
    //                  we have
    //
    inline BulletObject**
    bulletObjects(unsigned int& size);
    inline const BulletObject*const*
    bulletObjects(unsigned int& size) const;

    // @brief Free all the memory allocated. Note that you probably first want
    //        to remove all the bullet objects from the BulletWorld
    //
    void
    freeMemory(void);

private:
    DataHolder<btCollisionShape*> mShapes;
    DataHolder<BulletObject*> mBulletObjects;
};




////////////////////////////////////////////////////////////////////////////////
// Inline Stuff
//

inline BulletObject**
BulletLoader::bulletObjects(unsigned int& size)
{
    size = mBulletObjects.elements().size();
    return &(mBulletObjects.elements().front());
}
inline const BulletObject*const*
BulletLoader::bulletObjects(unsigned int& size) const
{
    size = mBulletObjects.elements().size();
    return &(mBulletObjects.elements().front());
}

} /* namespace physics */
#endif /* BULLETLOADER_H_ */
