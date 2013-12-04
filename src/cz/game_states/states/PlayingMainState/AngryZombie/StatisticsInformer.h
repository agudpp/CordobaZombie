/*
 * StatisticsInformer.h
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#ifndef STATISTICSINFORMER_H_
#define STATISTICSINFORMER_H_

#include <string>

namespace demo_app {

class StatisticsInformer
{
public:
    StatisticsInformer();
    virtual
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
};

} /* namespace demo_app */
#endif /* STATISTICSINFORMER_H_ */
