/*
 * Shape.h
 *
 *  Created on: Jan 7, 2014
 *      Author: agustin
 */

#ifndef SHAPE_H_
#define SHAPE_H_

#include <bullet/btBulletCollisionCommon.h>

#include <debug/DebugUtil.h>


// useful macros
#define SHAPE_PRECONDITION     ASSERT(mShape && "You must set a shape "\
                                      "object before being able to use this" \
                                      " class instance!")

namespace physics {


// forward
//
class PhysicsBuilder;


class Shape {
public:
    inline
    Shape() : mShape(0){};
    ~Shape(){};

    // @brief BECAREFUL USING THE COPY CONSTRUCTOR, we will not reallocate
    //        another shape for this, we will just use the same pointer!
    //
    inline
    Shape(const Shape&) = default;
    inline
    Shape& operator=(const Shape&) = default;

    inline btCollisionShape*
    bulletShape(void);
    inline const btCollisionShape*
    bulletShape(void) const;

private:
    // @brief We will let only to the PhysicsBuilder to construct this objects
    //        with bullet information.
    //
    friend class PhysicsBuilder;

    inline
    Shape(btCollisionShape* shape);
    inline void
    setShape(btCollisionShape* shape);

private:
    btCollisionShape* mShape;
};






////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline
Shape::Shape(btCollisionShape* shape) :
    mShape(shape)
{}

inline btCollisionShape*
Shape::bulletShape(void)
{
    return mShape;
}
inline const btCollisionShape*
Shape::bulletShape(void) const
{
    return mShape;
}

inline void
Shape::setShape(btCollisionShape* shape)
{
    mShape = shape;
}


} /* namespace physics */
#endif /* SHAPE_H_ */
