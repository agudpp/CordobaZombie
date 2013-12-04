/*
 * ObjectiveObject.h
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#ifndef OBJECTIVEOBJECT_H_
#define OBJECTIVEOBJECT_H_


#include "PhysicObject.h"


namespace demo_app {

class ObjectiveObject : public PhysicObject
{
public:
    ObjectiveObject();
    virtual
    ~ObjectiveObject();

    // @brief Method called whenever some other physics object hits this one
    //
    virtual void
    performCollision(CollisionInfo* collInfo);
};

} /* namespace demo_app */
#endif /* OBJECTIVEOBJECT_H_ */
