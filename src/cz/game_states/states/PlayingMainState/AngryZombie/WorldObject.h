/*
 * WorldObject.h
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#ifndef WORLDOBJECT_H_
#define WORLDOBJECT_H_


#include "PhysicObject.h"
#include "GameLogicData.h"


namespace demo_app {

class WorldObject : public PhysicObject
{
public:
    WorldObject(float maxSide, bool isGood);
    virtual
    ~WorldObject();

    // @brief Set the static counter variable to be used whenever we hit the
    //        floor (incrementing the variable in one).
    //
    static void
    setGameLogicData(GameLogicData* data) {sGameLogicData = data;}

    // @brief Method called whenever some other physics object hits this one
    //
    virtual void
    performCollision(CollisionInfo* collInfo){}

    // @brief update method used to update the logic of the element if needed
    // @return true if we need to continue updating | false otherwise
    //
    virtual bool
    update(float timeFrame);

private:
    static GameLogicData* sGameLogicData;
    float mMaxSize;
    bool mIsGood;
};

} /* namespace demo_app */
#endif /* WORLDOBJECT_H_ */
