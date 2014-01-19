/*
 * RandomGenerator.cpp
 *
 *   Created on: Jan 12, 2014
 *       Author: Carlos E. Budde
 *      License: GPLv3
 */

#include <cmath>    // lrint()
#include <ctime>    // time()
#include <climits>  // UCHAR_MAX
#include <cstdlib>  // drand48_r()

#include <debug/DebugUtil.h>
#include <random_generator/RandomGenerator.h>

namespace core {


/**********************************************************************
**  We use the reentrant version of C's rand48 functions family      **
**  Check the man pages of drand48 and drand48_r                     **
**  ----                                                             **
**  Also consider using C++ STL random facilities:                   **
**   http://stackoverflow.com/questions/18676375/generating-random-numbers-in-c
**   http://en.cppreference.com/w/cpp/numeric/random                 **
**********************************************************************/


RandomGenerator::RandomGenerator()
{
    RNGstate = new drand48_data;
    seed();
}
///////////////////////////////////////////////////////////////////////////////


RandomGenerator::~RandomGenerator()
{
    delete RNGstate;
}
///////////////////////////////////////////////////////////////////////////////


void
RandomGenerator::seed(unsigned int s)
{
    if (s == 0) {
        // Store in s some hash of time(0)
        // (http://eternallyconfuzzled.com/arts/jsw_art_rand.aspx)
        time_t now = time(0);
        unsigned char *p = (unsigned char*)&now;    // c-style cast is safe
        for (int i = 0 ; i < sizeof(now) ; i++)     // ...for numerical casts
            s = s * (UCHAR_MAX + 2U) + p[i];
    }
    int ok = srand48_r(static_cast<long int>(s), RNGstate);
#ifdef DEBUG
    if (ok < 0)
        debugERROR("RNG seeding failed\n");
#endif
}
///////////////////////////////////////////////////////////////////////////////


double
RandomGenerator::getUniform_D(double low, double up) const
{
    double number(0);
    if (low >= up) {
        debugWARNING("Bad parameters, it should be low < up\n");
        return low;
    }
    int ok = drand48_r(RNGstate, &number);
    if (ok < 0) {
        debugERROR("RNG generation failed\n");
        return low;
    } else
        return low + number * (up-low);
}
///////////////////////////////////////////////////////////////////////////////


double
RandomGenerator::getStdUniform_D()
{
    double number(0);
    int ok = drand48_r(RNGstate, &number);
    if (ok < 0) {
        debugERROR("RNG generation failed\n");
        return 0.F;
    } else
        return number;
}
///////////////////////////////////////////////////////////////////////////////


long
RandomGenerator::getUniform_L(long low, long up) const
{
    // TODO: consider using lrand48_r, since integer arithmetic may be faster
    return lrint(getUniform_D(static_cast<double>(low),
                              static_cast<double>(up)));
}
///////////////////////////////////////////////////////////////////////////////


long
RandomGenerator::getStdUniform_L(bool negatives) const
{
    int ok(0);
    long number(0);
    if (!negatives)
        ok = lrand48_r(RNGstate, &number);  // only positives: [0, 2^31]
    else
        ok = mrand48_r(RNGstate, &number);  // also negatives: [-2^31, 2^31]
    if (ok < 0) {
        debugERROR("RNG generation failed\n");
        return 0;
    } else
        return number;
}
///////////////////////////////////////////////////////////////////////////////



} /* namespace tool */
