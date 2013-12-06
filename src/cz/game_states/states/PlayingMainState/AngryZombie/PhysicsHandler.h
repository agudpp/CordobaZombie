/*
 * PhysicsHandler.h
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#ifndef PHYSICSHANDLER_H_
#define PHYSICSHANDLER_H_


#include <physics/DynamicWorld.h>
#include <types/StackVector.h>

#include "PhysicObject.h"


namespace demo_app {

class PhysicsHandler
{
public:
    static const unsigned int MAX_ELEMENTS = 4096;

    typedef core::StackVector<PhysicObject*, MAX_ELEMENTS> PhysicObjectVec;
public:
    PhysicsHandler();
    ~PhysicsHandler();

    // @brief Return the list of physics object to be filled from outside.
    //
    inline PhysicObjectVec&
    physicObjectsVec(void) {return mObjects;}

    // @brief Clear the current data (this will not free any memory).
    //
    void
    clear(void);

    // @brief Let this class know that you build all the objects you need (after
    //        setting in the physicObjectsVec)
    //
    void
    build(void);

    // @param Process collisions pairs (call the performCollision for each pair
    //        of objects that collide of physicObjects).
    //
    void
    performCollisions(float timeFrame);

private:
    PhysicObjectVec mObjects;
};



} /* namespace demo_app */
#endif /* PHYSICSHANDLER_H_ */
