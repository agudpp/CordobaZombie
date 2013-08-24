/*
 * FloatComp.h
 *
 *  Created on: Aug 7, 2013
 *      Author: agustin
 */

#ifndef FLOATCOMP_H_
#define FLOATCOMP_H_

#include <cmath>

// Here we will define some inline functions to compare float (less than / greater
// than / equal) for floating point values using a epsilon (threshold value).
//

#define FLOAT_COMP_THRESHOLD    1.e-5

namespace core {

// @brief Compare if two floating points are equal
//
inline bool
fcomp_equal(float a, float b, float epsilon = FLOAT_COMP_THRESHOLD)
{
    return std::abs(a-b) <= epsilon;
}

// @brief compare <= (a <= b).
//
inline bool
fcomp_leq(float a, float b, float epsilon = FLOAT_COMP_THRESHOLD)
{
    return a < b || fcomp_equal(a, b, epsilon);
}

// @brief compare >= (a >= b)
//
inline bool
fcomp_geq(float a, float b, float epsilon = FLOAT_COMP_THRESHOLD)
{
    return fcomp_leq(b, a, epsilon);
}


}


#endif /* FLOATCOMP_H_ */
