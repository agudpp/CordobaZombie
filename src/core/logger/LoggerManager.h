/*
 * LoggerManager.h
 *
 *  Created on: Dec 11, 2013
 *      Author: agustin
 */

#ifndef LOGGERMANAGER_H_
#define LOGGERMANAGER_H_

#include <vector>

#include "Logger.h"

namespace core {

class LoggerManager
{
public:

    // @brief Singleton instance.
    //
    static inline LoggerManager&
    instance(void)
    {
        static LoggerManager instance;
        return instance;
    }
    ~LoggerManager() {};

    // @brief Add a logger to log data.
    // @param logger    The logger to be added
    // @note this class will not handle the memory of the logger instance.
    //
    void
    addLogger(Logger* logger);

    // @brief Remove all the loggers
    //
    void
    removeAllLoggers(void);

    ///////////////////////////////////////////////////////////////////////////
    // Logging functions
    //

    // @brief Log a message in this logger:
    // @param message       The message to be logged.
    // @param style         The style of the message.
    //
    inline void
    log(const char* message, LogMessageStyle style = LogMessageStyle::LOG_MSG_STYLE_NONE);

    // @brief Log a more pretty message using the filename, function name,
    //        line number and msg type header + message and style.
    // @param msgType       The header of the message.
    // @param filename      The filename from where we are logging this message.
    // @param function      The function from where we are logging this message.
    // @param line          The line number from where we are logging this message.
    // @param message       The message itself.
    // @param style         The style.
    // @note this function will generate a log of this kind:
    //          msgType[Filename, function, line]: message.
    //       using the associated style to everything.
    //
    inline void
    log(const char* msgType,
        const char* filename,
        const char* function,
        int line,
        const char* message,
        LogMessageStyle style);


private:
    LoggerManager();
    LoggerManager(const LoggerManager&);
    LoggerManager& operator=(const LoggerManager&);

private:
    std::vector<Logger*> mLoggers;
};






////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//


inline void
LoggerManager::log(const char* message, LogMessageStyle style)
{
    for (Logger* l : mLoggers) {
        l->log(message, style);
    }
}

inline void
LoggerManager::log(const char* msgType,
                   const char* filename,
                   const char* function,
                   int line,
                   const char* message,
                   LogMessageStyle style)
{
    for (Logger* l : mLoggers) {
        l->log(msgType, filename, function, line, message, style);
    }
}

} /* namespace core */

#endif /* LOGGERMANAGER_H_ */
