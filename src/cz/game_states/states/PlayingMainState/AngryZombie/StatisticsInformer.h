/*
 * StatisticsInformer.h
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#ifndef STATISTICSINFORMER_H_
#define STATISTICSINFORMER_H_

#include <string.h>

#include <string>

namespace demo_app {

class StatisticsInformer
{
public:

    struct Data {
        unsigned int numRagdolls;
        unsigned int totalPhysicsObjects;

        Data() {reset();}

        void reset(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

public:
    StatisticsInformer();
    ~StatisticsInformer();

    // @brief Clear / reset the data.
    //
    void
    reset(void);

    // @brief Save the data to a file (append to the last of the file if already
    //        exists).
    // @param fileName      The filename where we will save the info
    // @return true on success | false otherwise
    //
    bool
    dumpInform(const std::string& filename);

    // @brief Call this method each frame with the associated data
    //
    void
    update(float frameTime, const Data& data);
};

} /* namespace demo_app */
#endif /* STATISTICSINFORMER_H_ */
