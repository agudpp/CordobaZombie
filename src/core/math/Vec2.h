/*
 * Vec2.h
 *
 *  Created on: 21/02/2012
 *      Author: agustin
 *
 */

#ifndef VEC2_H_
#define VEC2_H_

#include <iostream>
#include <cmath>

// Define the unit of conversions from degress to radians
#define DEG_TO_RAD_FAC  0.0174532925f /* PI / 180 */

namespace core {

//http://www.lomont.org/Math/Papers/2003/InvSqrt.pdf
inline float
InvSqrt(float x)
{
    const float xhalf = 0.5f * x;
    int i = *(int*) &x; // get bits for floating value
    i = 0x5f3759df - (i >> 1); // gives initial guess y0
    x = *(float*) &i; // convert bits back to float
    x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy
    return x;
}


// define a vector2
//

struct Vector2
{
    float x;
    float y;

    inline
    Vector2() :
        x(0.0f)
    ,   y(0.0f)
    {
    }

    inline
    Vector2(const Vector2 &o) :
        x(o.x)
    ,   y(o.y)
    {
    }

    inline
    Vector2(float vx, float vy) :
        x(vx)
    ,   y(vy)
    {
    }

    inline Vector2&
    operator =(const Vector2& rkVector)
    {
        x = rkVector.x;
        y = rkVector.y;

        return *this;
    }

    inline float
    crossProduct(const Vector2 &other) const
    {
        return x * other.y - other.x * y;
    }

    inline float
    squaredDistance(const Vector2 &other) const
    {
        const float p = x - other.x;
        const float q = y - other.y;
        return p * p + q * q;
    }

    inline void
    transformToNormal(void)
    {
        const float aux = x;
        x = -y;
        y = aux;
    }

    inline void
    rotate180(void)
    {
        x = -x;
        y = -y;
    }

    // returns true if other is clockwise from this.
    inline bool
    isClockwise(const Vector2 &other) const
    {
        return crossProduct(other) < 0.0f;
    }

    inline Vector2&
    operator =(const float fScalar)
    {
        x = fScalar;
        y = fScalar;

        return *this;
    }

    inline float
    length(void) const
    {
        return 1.0 / InvSqrt(x * x + y * y);
    }

    inline float
    squaredLength(void) const
    {
        return x * x + y * y;
    }

    inline float
    inverseLength(void) const
    {
        return InvSqrt(x * x + y * y);
    }

    inline void
    normalize(void)
    {
        *this *= InvSqrt(x * x + y * y);
    }

    inline bool
    operator ==(const Vector2& rkVector) const
    {
        return (x == rkVector.x && y == rkVector.y);
    }

    inline bool
    operator !=(const Vector2& rkVector) const
    {
        return (x != rkVector.x || y != rkVector.y);
    }

    // arithmetic operations
    inline Vector2
    operator +(const Vector2& rkVector) const
    {
        return Vector2(x + rkVector.x, y + rkVector.y);
    }

    inline Vector2
    operator -(const Vector2& rkVector) const
    {
        return Vector2(x - rkVector.x, y - rkVector.y);
    }

    inline void
    operator -=(const Vector2& rkVector)
    {
        x -= rkVector.x;
        y -= rkVector.y;
    }

    inline void
    operator +=(const Vector2& rkVector)
    {
        x += rkVector.x;
        y += rkVector.y;
    }

    inline void
    operator *=(const float fScalar)
    {
        x *= fScalar;
        y *= fScalar;
    }

    inline Vector2
    operator *(const float fScalar) const
    {
        return Vector2(x * fScalar, y * fScalar);
    }

    inline Vector2
    operator *(const Vector2& rhs) const
    {
        return Vector2(x * rhs.x, y * rhs.y);
    }

    inline float
    dotProduct(const Vector2 &o) const
    {
        return x * o.x + y * o.y;
    }

    inline void
    operator /=(const float fScalar)
    {

        float fInv = 1.0f / fScalar;
        x *= fInv;
        y *= fInv;
    }

    inline Vector2
    operator /(const Vector2& rhs) const
    {
        return Vector2(x / rhs.x, y / rhs.y);
    }

    inline Vector2
    rotateRadians(float rads)
    {
        const float cos = std::cos(rads);
        const float sin = std::sin(rads);
        return Vector2(x * cos + y * sin, x * -sin + y * cos);
    }

    inline Vector2
    rotateDegrees(float degrees)
    {
        return rotateRadians(DEG_TO_RAD_FAC * degrees);
    }

    // @brief truncate the length to an specific one if and only if the current
    //        length is greater than len.
    // @param maxSqrLen   The max squared length to truncate.
    //
    inline void
    truncate(float maxSqrLen)
    {
        const float sqrLen = squaredLength();
        if (sqrLen < maxSqrLen) return;
//        const float newLen = (maxSqrLen/sqrLen);
//        x *= newLen;
//        y *= newLen;
        normalize();
        x *= maxSqrLen;
        y *= maxSqrLen;
    }

    // For debugging printing
    //
    inline friend std::ostream& operator<<(std::ostream& o, const Vector2& v)
    {
        o << "[" << v.x << ", " << v.y << "]";
        return o;
    }
};

}

#endif /* VEC2_H_ */
