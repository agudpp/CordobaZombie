/*
 * MotionState.h
 *
 *  Created on: Jan 7, 2014
 *      Author: agustin
 */

#ifndef MOTIONSTATE_H_
#define MOTIONSTATE_H_


#include <bullet/LinearMath/btMotionState.h>
#include <bullet/btBulletDynamicsCommon.h>



namespace physics {

// @brief We will define the trivial MotionState to be used.
//

class MotionState : public btMotionState
{
public:
    MotionState() {};
    virtual
    ~MotionState() {};

     virtual void
     getWorldTransform(btTransform &worldTrans) const{};
     virtual void
     setWorldTransform(const btTransform &worldTrans){};
};

} /* namespace physics */
#endif /* MOTIONSTATE_H_ */
