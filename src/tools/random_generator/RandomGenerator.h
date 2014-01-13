/*
 * RandomGenerator.h
 *
 *   Created on: Jan 12, 2014
 *       Author: Carlos E. Budde
 *      License: GPLv3
 */
#ifndef RANDOMGENERATOR_H_
#define RANDOMGENERATOR_H_

// Forward declarations
struct drand48_data;

namespace tool {

struct RandomGenerator
{
    /**
     * @brief Construct and initialize the Random Number Generator
     * @remarks The RNG is seeded on creation: thus it can be used immediately
     *          after creation, there's NO need to call seed() first.
     */
    RandomGenerator();
    ~RandomGenerator();

    /**
     * @brief Initialize the Random Number Generator engine with given seed
     * @param   s   Seed to initialize the RNG engine with
     * @remarks If no parameter (or == 0) is given, RNG engine is initialized
     *          with a hashing of time(0)
     * @remarks This struct is initialized once on construction
     */
    void
    seed(unsigned int s = 0);

    /**
     * @brief Return value uniformly distributed in range [low,up]
     * @param   low     Lower bound for returned value
     * @param   up      Upper bound for returned value
     * @remarks up should be greater than low, else low is returned
     */
    double
    getUniform_D(double low=0.0, double up=1.0) const;

    /**
     * @brief Return value uniformly distributed in range [0.0, 1.0]
     * @remarks Faster than above "getUniform(0.0,1.0)", ONLY for this range
     */
    double
    getStdUniform_D();

    /**
     * @brief Return value uniformly distributed in range [low,up]
     * @param   low     Lower bound for returned value
     * @param   up      Upper bound for returned value
     * @remarks up should be greater than low, else low is returned
     */
    long
    getUniform_L(long low=0, long up=100) const;


    /**
     * @brief Return value uniformly distributed in range [0, 2^31]
     * @param   negatives   Optionally include negative values: [-2^31, 2^31]
     * @remarks Faster than above "getUniform(0,2^31)", ONLY for this range
     * @remarks Faster than above "getUniform(-2^31,2^31)", ONLY for this range
     */
    long
    getStdUniform_L(bool negatives = false) const;

    /**
     * @brief Randomly reorder the elements of a sequence inside given range
     * @param   __first C++ stdlib-like iterator at the start of the sequence
     * @param   __last  C++ stdlib-like iterator at the end   of the sequence
     * @remarks Considered valid range is [__first,__last)
     * @remarks _RandomAccessIterator must offer C++ stdlib iterator syntax
     * @remarks every possible ordering of the sequence is equally likely
     */
    template<typename _RandomAccessIterator>
    inline void
    randomShuffle(_RandomAccessIterator __first,
                  _RandomAccessIterator __last) const;

private:
    drand48_data* RNGstate;
};

} /* namespace tool */

#endif /* RANDOMGENERATOR_H_ */
