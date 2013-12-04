/*
 * Projectile.h
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include "PhysicObject.h"

namespace demo_app {

class Projectile : public PhysicObject
{
public:
    Projectile();
    virtual
    ~Projectile();

    // @brief Method called whenever some other physics object hits this one
    //
    virtual void
    performCollision(CollisionInfo* collInfo);
};

} /* namespace demo_app */
#endif /* PROJECTILE_H_ */
