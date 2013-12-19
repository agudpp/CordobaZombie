/*
 * LoggerManager.cpp
 *
 *  Created on: Dec 11, 2013
 *      Author: agustin
 */

#include "LoggerManager.h"

#include <assert.h>



namespace core {


LoggerManager::LoggerManager()
{
    // we will construct the default logger for windows and linux (windows
    // will be the cz.log file and linux will be the standard error out).
    //
#ifdef WIN32
    static Logger defaultLogger(LogType::LOG_TYPE_STD_FILE, LogMode::LOG_MODE_APPEND);
#else
    static Logger defaultLogger(LogType::LOG_TYPE_STD_ERROR);
#endif
    addLogger(&defaultLogger);

    // log the message if we want to remove this in the future
    log("We are adding a default logger in the LoggerManager constructor, remove"
        " it in the real release\n", LogMessageStyle::LOG_MSG_STYLE_RED);
}

void
LoggerManager::addLogger(Logger* logger)
{
    assert(logger);
    mLoggers.push_back(logger);
}

// @brief Remove all the loggers
//
void
LoggerManager::removeAllLoggers(void)
{
    mLoggers.clear();
}

} /* namespace core */
