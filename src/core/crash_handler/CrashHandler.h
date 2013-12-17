/*
 * CrashHandler.h
 *
 *  Created on: Dec 12, 2013
 *      Author: agustin
 */

#ifndef CRASHHANDLER_H_
#define CRASHHANDLER_H_

namespace core {

// @brief This class will be used to set and configure the signals to handle
//        the crashes (exceptions / abort / asserts) to create a report
//        and save it using the current LoggerManager.
//


class CrashHandler
{
public:
    CrashHandler();
    ~CrashHandler();

    // @brief Configure the sig handlers to catch the signals and exceptions.
    // @return true on success | false otherwise
    //
    bool
    configureSignals(void);

};

} /* namespace core */

#endif /* CRASHHANDLER_H_ */
