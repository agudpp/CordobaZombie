/*
 * Logger.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: agustin
 */

#include "Logger.h"

#include <time.h>
#include <stdio.h>

#include <fstream>


// internal methods
namespace {

// @brief This methods will return the beginning of the string to be added
//        to print with colors in the console or any other place.
//
inline const char*
styleHeader(const core::LogMessageStyle style)
{
    switch (style) {
    case core::LogMessageStyle::LOG_MSG_STYLE_NONE:           return "";
    case core::LogMessageStyle::LOG_MSG_STYLE_YELLOW:         return "\33[00;33m";
    case core::LogMessageStyle::LOG_MSG_STYLE_RED:            return "\33[22;31m";
    case core::LogMessageStyle::LOG_MSG_STYLE_GREEN:          return "\33[22;32m";
    case core::LogMessageStyle::LOG_MSG_STYLE_BLUE:           return "\33[22;34m";
    case core::LogMessageStyle::LOG_MSG_STYLE_INVERT_RED:     return "\33[22;31m" "\33[7m";
    case core::LogMessageStyle::LOG_MSG_STYLE_INVERT_BLUE:    return "\33[22;34m" "\33[7m";
    case core::LogMessageStyle::LOG_MSG_STYLE_UNDERLINE_RED:  return "\33[4m" "\33[22;31m";
    }
    return "";
}

#define LOG_MESSAGE_STYLE_NORMAL    "\33[0m"

}


namespace core {

////////////////////////////////////////////////////////////////////////////////
void
Logger::printHeader(void)
{
    assert(mStream);
    // get the time
    time_t rawtime;
    time (&rawtime);

    // print the time into a string buff
    char str[512];
    sprintf(str, "%s", ctime(&rawtime));

    // print it into the stream

    *mStream <<
        "\n\n-----------------------------------------------------------------------------\n"
        "            New logger started on " << str << "\n"
        "-----------------------------------------------------------------------------\n\n";
}

////////////////////////////////////////////////////////////////////////////////
Logger::Logger(LogType logType,
               LogMode logMode,
               const std::string& outName,
               bool aPrintHeader) :
    mStream(0)
,   mLogType(logType)
,   mLogMode(logMode)
,   mIsFile(false)
,   mOutName(outName)
{
    // get the stream
    switch (logType) {
    case LogType::LOG_TYPE_STD_ERROR:
        mStream = &(std::cerr);
        break;
    case LogType::LOG_TYPE_STD_OUT:
        mStream = &(std::cout);
        break;
    case LogType::LOG_TYPE_STD_FILE:
        assert(mOutName.size() > 0);
        // set the open mode
        const std::ios_base::openmode mode = (logMode == LOG_MODE_APPEND) ? std::ios::app :
            std::ios::out;
        mStream = new std::ofstream(outName.c_str(), mode);
        mIsFile = true;
        break;
    }

    assert(mStream != 0);
    if (aPrintHeader) {
        printHeader();
    }
}

////////////////////////////////////////////////////////////////////////////////
Logger::~Logger()
{
    // delete the stream if was allocated
    assert(mStream);
    mStream->flush();
    if (mIsFile) {
        delete mStream;
        mStream = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
void
Logger::log(const char* message, LogMessageStyle style)
{
    assert(mStream);

    // if is file we skip the style
    if (mIsFile) {
        *mStream << message;
    } else {
        *mStream << styleHeader(style) << message << LOG_MESSAGE_STYLE_NORMAL;
    }

    // in any case flush the stream
    mStream->flush();
}

////////////////////////////////////////////////////////////////////////////////
void
Logger::log(const char* msgType,
            const char* filename,
            const char* function,
            int line,
            const char* message,
            LogMessageStyle style)
{
    assert(mStream);

    // if it is file skip style
    if (mIsFile) {
        *mStream << msgType << " [" << filename << ", " << function << ", " <<
            line << "]: " << message;
    } else {
        *mStream << styleHeader(style) << msgType << " [" << filename << ", " <<
            function << ", " << line << "]: " << message <<
            LOG_MESSAGE_STYLE_NORMAL;
    }
    // in any case flush the stream
    mStream->flush();
}

} /* namespace core */
