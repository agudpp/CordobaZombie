/*
 * Logger.h
 *
 *  Created on: Dec 10, 2013
 *      Author: agustin
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <assert.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>


namespace core {

    // Some public enums
    //
    enum LogType {
        LOG_TYPE_STD_ERROR = 0,
        LOG_TYPE_STD_OUT,
        LOG_TYPE_STD_FILE,
    };

    enum LogMode {
        LOG_MODE_APPEND = 0,
        LOG_MODE_OVERWRITE,
    };

    // useful enum for Log message styles
    //
    enum LogMessageStyle {
        LOG_MSG_STYLE_NONE = 0,
        LOG_MSG_STYLE_YELLOW,
        LOG_MSG_STYLE_RED,
        LOG_MSG_STYLE_GREEN,
        LOG_MSG_STYLE_BLUE,
        LOG_MSG_STYLE_INVERT_RED,
        LOG_MSG_STYLE_INVERT_BLUE,
        LOG_MSG_STYLE_UNDERLINE_RED,
    };

class Logger
{
public:
    // @brief Constructor of the logger:
    // @param logType       The logging type we want to use.
    // @param outMode       The mode of the output (append / new log).
    // @param outName       In the case of files we need to set the output filename.
    // @param printHeader   This will be used to check if we want to print the
    //                      header or not of this logger when creating the
    //                      instance
    //
    Logger(LogType logType = LOG_TYPE_STD_ERROR,
           LogMode logMode = LOG_MODE_OVERWRITE,
           const std::string& outName = "cz.log",
           bool printHeader = true);
    ~Logger();

    // @brief Get the curren configuration of the Logger.
    //
    inline LogType
    logType(void) const;
    inline LogMode
    logMode(void) const;
    inline const std::string&
    outName(void) const;

    // @brief Log a message in this logger:
    // @param message       The message to be logged.
    // @param style         The style of the message.
    //
    void
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
    void
    log(const char* msgType,
        const char* filename,
        const char* function,
        int line,
        const char* message,
        LogMessageStyle style);

private:

    // @brief Print the header of the current log! This will print the date
    //        time and everything else.
    //
    void
    printHeader(void);


private:
    std::ostream* mStream;
    LogType mLogType;
    LogMode mLogMode;
    bool mIsFile;
    std::string mOutName;
};





////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline LogType
Logger::logType(void) const
{
    return mLogType;
}
inline LogMode
Logger::logMode(void) const
{
    return mLogMode;
}
inline const std::string&
Logger::outName(void) const
{
    return mOutName;
}


} /* namespace core */

#endif /* LOGGER_H_ */
