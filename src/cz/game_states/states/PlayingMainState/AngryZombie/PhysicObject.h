/*
 * PhysicObject.h
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#ifndef PHYSICOBJECT_H_
#define PHYSICOBJECT_H_

#include <physics/BulletObject.h>

namespace demo_app {

// Forward
//
class PhysicsHandler;
class PhysicObject;

// Helpful data used for collisions
//
struct CollisionInfo {
    PhysicObject* obj1;
    PhysicObject* obj2;
};

class PhysicObject
{
public:
    // Useful enum to set the different type of physics objects
    //
    enum Type {
        POT_WORLD =         (1 << 0),
        POT_PROJECTILE =    (1 << 1),
        POT_OBJECTIVE =     (1 << 2),
        POT_OBSTACLE =      (1 << 3),
    };

public:
    PhysicObject(){};
    virtual
    ~PhysicObject(){};

    // @brief Set / get the type
    //
    inline void
    setType(Type t) {mType = t;}
    inline Type
    type(void) const {return mType;}

    // @brief Get the bullet object information
    //
    inline physics::BulletObject&
    bulletObject(void) {return mBulletObject;}
    inline const physics::BulletObject&
    bulletObject(void) const {return mBulletObject;}

    // @brief Method called whenever some other physics object hits this one
    //
    virtual void
    performCollision(CollisionInfo* collInfo) = 0;

private:
    Type mType;
    physics::BulletObject mBulletObject;
private:
    friend class PhysicsHandler;
    unsigned short _id;


};

} /* namespace demo_app */
#endif /* PHYSICOBJECT_H_ */
