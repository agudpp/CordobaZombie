/*
 * WorldObject.h
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#ifndef WORLDOBJECT_H_
#define WORLDOBJECT_H_


#include "PhysicObject.h"


namespace demo_app {

class WorldObject : PhysicObject
{
public:
    WorldObject();
    virtual
    ~WorldObject();

    // @brief Method called whenever some other physics object hits this one
    //
    virtual void
    performCollision(CollisionInfo* collInfo);
};

} /* namespace demo_app */
#endif /* WORLDOBJECT_H_ */
