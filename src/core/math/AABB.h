/*
 * AABB.h
 *
 *  Created on: 21/02/2012
 *      Author: agustin
 *
 */

#ifndef AABB_H_
#define AABB_H_

#include <iostream>

#include "Vec2.h"


namespace core {

// This class represents an Axis Aligned Bounding Box and using the normal
// coordinates system, where x grows positive to right and y grows positive for
// top, so top is higher than bottom and left is lesser than right
//

// Aligned box2D
struct AABB
{
    Vector2 tl;
    Vector2 br;

    AABB()
    {
    }
    AABB(const Vector2 &topLeft, const Vector2 &bottomRight) :
        tl(topLeft)
    ,   br(bottomRight)
    {
    }
    AABB(float top, float left, float bottom, float right) :
        tl(Vector2(left, top))
    ,   br(Vector2(right, bottom))
    {
    }

    // check if a point is inside of the box
    inline bool
    checkPointInside(const Vector2 &p) const
    {
        return p.x >= tl.x && p.x <= br.x && p.y >= br.y && p.y <= tl.y;
    }

    // translate the bounding box
    inline void
    translate(const Vector2 &v)
    {
        tl += v;
        br += v;
    }

    // Change the size maintaining the top left position
    inline void
    setSize(const float x, const float y)
    {
        br.x = tl.x + x;
        tl.y = br.y + y;
    }

    inline float
    getHeight(void) const
    {
        return tl.y - br.y;
    }

    inline float
    getWidth(void) const
    {
        return br.x - tl.x;
    }

    // set the position of the AABB taking into account the top left vertex
    inline void
    setPosition(const Vector2 &v)
    {
        translate(tl - v);
    }

    // set the position taking into account the center of the bounding box
    inline void
    setCenterPosition(const Vector2 &p)
    {
        // get half size
        const float halfX = (br.x - tl.x) * 0.5f;
        const float halfY = (tl.y - br.y) * 0.5f;
        tl.x = p.x - halfX;
        tl.y = p.y + halfY;
        br.x = p.x + halfX;
        br.y = p.y - halfY;
    }

    // get the center position
    inline void
    getCenter(Vector2& center) const
    {
        center.x = (br.x + tl.x) * 0.5f;
        center.y = (tl.y + br.y) * 0.5f;
    }
    inline Vector2
    center(void) const
    {
        return Vector2((br.x + tl.x) * 0.5f, (tl.y + br.y) * 0.5f);
    }


    // check the collision
    inline bool
    collide(const AABB &o) const
    {
        return !((o.br.x < tl.x) || (o.tl.x > br.x) || (o.tl.y < br.y)
            || (tl.y < o.br.y));
    }

    // @brief Increase the size of the current bounding box to contain another
    // @param other     The other bounding box to be contained
    //
    inline void
    increaseToContain(const AABB& other)
    {
        if (other.tl.x < tl.x) {
            tl.x = other.tl.x;
        }
        if (other.tl.y > tl.y) {
            tl.y = other.tl.y;
        }
        if (other.br.x > br.x) {
            br.x = other.br.x;
        }
        if (other.br.y < br.y) {
            br.y = other.br.y;
        }
    }

    // @brief Increase the size of the current bounding box to contain a vector
    // @param vec   The vector we want to contain
    //
    inline void
    increaseToContain(const Vector2& vec)
    {
        if (tl.x > vec.x) {
            tl.x = vec.x;
        }
        if (br.x < vec.x) {
            br.x = vec.x;
        }
        if (tl.y < vec.y) {
            tl.y = vec.y;
        }
        if (br.y > vec.y) {
            br.y = vec.y;
        }
    }

    // For debugging printing
    inline friend std::ostream& operator<<(std::ostream& o, const AABB& aabb)
    {
        o << "AABB(tl:" << aabb.tl << ", br: " << aabb.br << ")" << std::endl;
        return o;
    }


    // @brief compare operators
    //
    inline bool
    operator ==(const AABB& o) const
    {
        return tl == o.tl && br == o.br;
    }
    inline bool
    operator !=(const AABB& o) const
    {
        return !(*this == o);
    }

};

typedef AABB AlignedBox;

}

#endif /* AABB_H_ */
