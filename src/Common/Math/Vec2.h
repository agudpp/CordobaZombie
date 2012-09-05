/*
 * Vec2.h
 *
 *  Created on: 21/02/2012
 *      Author: agustin
 *
 */

#ifndef VEC2_H_
#define VEC2_H_


#include <cmath>

//http://www.lomont.org/Math/Papers/2003/InvSqrt.pdf
static inline float InvSqrt(float x){
	float xhalf = 0.5f*x;
	int i = *(int*)&x; // get bits for floating value
	i = 0x5f3759df - (i>>1); // gives initial guess y0
	x = *(float*)&i; // convert bits back to float
	x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
	return x;
}


namespace sm {

// define a point used by the convex poly
struct Point {
	float x;
	float y;

	inline Point():x(0.0f), y(0.0f){};
	inline Point(const Point &o):x(o.x), y(o.y){};
	inline Point(float vx, float vy) :
		x(vx), y(vy) {
	}

	inline Point& operator =(const Point& rkVector) {
		x = rkVector.x;
		y = rkVector.y;

		return *this;
	}

	inline float crossProduct(const Point &other) const
	{
		return x*other.y - other.x * y;
	}

	inline float squaredDistance(const Point &other) const
	{
		const float p = x - other.x;
		const float q = y - other.y;
		return p*p + q*q;
	}

	inline void transformToNormal(void)
	{
		const float aux = x;
		x = -y;
		y = aux;
	}

	inline void rotate180(void) {
		x = -x;
		y = -y;
	}

	// returns true if other is clockwise from this.
	inline bool isClockwise(const Point &other) const
	{
		return crossProduct(other) < 0.0f;
	}

	inline Point& operator =(const float fScalar) {
		x = fScalar;
		y = fScalar;

		return *this;
	}

	inline float length(void) const {
		return 1.0/InvSqrt(x*x + y*y);
	}

	inline float squaredLength(void) const {
		return x*x + y*y;
	}

	inline float inverseLen(void) const {
		return InvSqrt(x*x + y*y);
	}

	inline void normalize(void) {
		*this *= InvSqrt(x*x + y*y);
	}

	inline bool operator ==(const Point& rkVector) const {
		return (x == rkVector.x && y == rkVector.y);
	}

	inline bool operator !=(const Point& rkVector) const {
		return (x != rkVector.x || y != rkVector.y);
	}

	// arithmetic operations
	inline Point operator +(const Point& rkVector) const {
		return Point(x + rkVector.x, y + rkVector.y);
	}

	inline Point operator -(const Point& rkVector) const {
		return Point(x - rkVector.x, y - rkVector.y);
	}

	inline void operator -=(const Point& rkVector) {
		x -= rkVector.x; y -= rkVector.y;
	}

	inline void operator +=(const Point& rkVector) {
		x += rkVector.x; y += rkVector.y;
	}

	inline void operator *=(const float fScalar) {
		x *= fScalar;
		y *= fScalar;
	}

	inline Point operator *(const float fScalar) const {
		return Point(x * fScalar, y * fScalar);
	}


	inline Point operator *(const Point& rhs) const {
		return Point(x * rhs.x, y * rhs.y);
	}

	inline float dotProduct(const Point &o) const {
		return x * o.x + y * o.y;
	}

	inline void operator /=(const float fScalar) {

		float fInv = 1.0f / fScalar;
		x *= fInv;
		y *= fInv;
	}

	inline Point operator /(const Point& rhs) const {
		return Point(x / rhs.x, y / rhs.y);
	}

	inline Point rotaitXDegrees (float rotM[][2]){
		return Point( x * rotM[0][0] + y * rotM[1][0],
					  x * rotM[0][1] + y * rotM[1][1]);
	}
};



typedef Point Vector2;
}

#endif /* VEC2_H_ */
