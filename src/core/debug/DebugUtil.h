/*
*  Useful functions used in debug
*
*  Created on: 18/11/2010
*      Author: agustin
*/

#ifndef DEBUGUTIL_H_
#define DEBUGUTIL_H_



#if defined(_WIN32) || defined(CYGWIN)
#  include <libgen.h>
#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__)
#  ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#  endif
#  include <cstring>
#else
#  error "Unsupported platform, aborting compilation."
#endif
// Relative file name string, i.e. without path.
#define  __FILENAME__  (basename(__FILE__))

/**
 * Define the different colors prints
 */
#define DEBUG_BLACK 	"\33[22;30m"
#define DEBUG_GRAY 		"\33[01;30m"
#define DEBUG_RED 		"\33[22;31m"
#define DEBUG_LRED	 	"\33[01;21m"
#define DEBUG_GREEN 	"\33[22;32m"
#define DEBUG_LGREEN 	"\33[01;32m"
#define DEBUG_BLUE 		"\33[22;34m"
#define DEBUG_LBLUE 	"\33[01;34m"
#define DEBUG_BROWN 	"\33[22;33m"
#define DEBUG_YELLOW 	"\33[00;33m"
#define DEBUG_CYAN 		"\33[22;36m"
#define DEBUG_LCYAN	 	"\33[22;36m"
#define DEBUG_MAGENTA 	"\33[22;35m"
#define DEBUG_LMAGENTA 	"\33[01;35m"
#define DEBUG_NC 		"\33[0m"
#define DEBUG_BOLD	 	"\33[1m"
#define DEBUG_ULINE	 	"\33[4m" //underline
#define DEBUG_BLINK	 	"\33[5m"
#define DEBUG_INVERT	"\33[7m"



#ifdef DEBUG
	#include <assert.h>
	#include <iostream>
	#include <stdio.h>
    #include <logger/LoggerManager.h>
    #include <logger/Logger.h>

// WINDOWS STUFF
//
/*
#if defined(_WIN32) || defined(CYGWIN) || defined(WIN32) || defined (MINGW)
*/

#include <cstdio>

	#define debug(format,...) {char msg[1024]; std::sprintf(msg, format, ## __VA_ARGS__);\
        core::LoggerManager::instance().log("debug",\
                                            __FILE__,\
                                            __FUNCTION__,\
                                            __LINE__, \
                                            msg, \
                                            core::LogMessageStyle::LOG_MSG_STYLE_NONE);}

	#define debugYELLOW(format, ...) {char msg[1024]; std::sprintf(msg, format, ## __VA_ARGS__);\
        core::LoggerManager::instance().log("DEBUG_YELLOW",\
                                            __FILE__,\
                                            __FUNCTION__,\
                                            __LINE__, \
                                            msg, \
                                            core::LogMessageStyle::LOG_MSG_STYLE_YELLOW);}
	#define debugRED(format, ...)  {char msg[1024]; std::sprintf(msg, format, ## __VA_ARGS__);\
        core::LoggerManager::instance().log("DEBUG_RED",\
                                            __FILE__,\
                                            __FUNCTION__,\
                                            __LINE__, \
                                            msg, \
                                            core::LogMessageStyle::LOG_MSG_STYLE_RED);}
	#define debugGREEN(format, ...)  {char msg[1024]; std::sprintf(msg, format, ## __VA_ARGS__);\
        core::LoggerManager::instance().log("DEBUG_GREEN",\
                                            __FILE__,\
                                            __FUNCTION__,\
                                            __LINE__, \
                                            msg, \
                                            core::LogMessageStyle::LOG_MSG_STYLE_GREEN);}
	#define debugBLUE(format, ...) {char msg[1024]; std::sprintf(msg, format, ## __VA_ARGS__);\
        core::LoggerManager::instance().log("DEBUG_BLUE",\
                                            __FILE__,\
                                            __FUNCTION__,\
                                            __LINE__, \
                                            msg, \
                                            core::LogMessageStyle::LOG_MSG_STYLE_BLUE);}
	#define debugOPTIMIZATION(format, ...) {char msg[1024]; std::sprintf(msg, format, ## __VA_ARGS__);\
        core::LoggerManager::instance().log("DEBUG_OPTIMIZATION",\
                                            __FILE__,\
                                            __FUNCTION__,\
                                            __LINE__, \
                                            msg, \
                                            core::LogMessageStyle::LOG_MSG_STYLE_INVERT_BLUE);}
	#define debugERROR(format, ...) {char msg[1024]; std::sprintf(msg, format, ## __VA_ARGS__);\
        core::LoggerManager::instance().log("ERROR",\
                                            __FILE__,\
                                            __FUNCTION__,\
                                            __LINE__, \
                                            msg, \
                                            core::LogMessageStyle::LOG_MSG_STYLE_INVERT_RED);}
	#define debugWARNING(format, ...) {char msg[1024]; std::sprintf(msg, format, ## __VA_ARGS__);\
        core::LoggerManager::instance().log("WARNING",\
                                            __FILE__,\
                                            __FUNCTION__,\
                                            __LINE__, \
                                            msg, \
                                            core::LogMessageStyle::LOG_MSG_STYLE_UNDERLINE_RED);}

	#define testBEGIN(format, ...) {char msg[1024]; std::sprintf(msg, format, ## __VA_ARGS__);\
        core::LoggerManager::instance().log("TEST_BEGIN",\
                                            __FILE__,\
                                            __FUNCTION__,\
                                            __LINE__, \
                                            msg, \
                                            core::LogMessageStyle::LOG_MSG_STYLE_YELLOW);}
	#define testSUCCESS(format, ...) {char msg[1024]; std::sprintf(msg, format, ## __VA_ARGS__);\
        core::LoggerManager::instance().log("TEST_SUCCESS",\
                                            __FILE__,\
                                            __FUNCTION__,\
                                            __LINE__, \
                                            msg, \
                                            core::LogMessageStyle::LOG_MSG_STYLE_GREEN);}

	#define testFAIL(format, ...) {char msg[1024]; std::sprintf(msg, format, ## __VA_ARGS__);\
        core::LoggerManager::instance().log("TEST_FAIL",\
                                            __FILE__,\
                                            __FUNCTION__,\
                                            __LINE__, \
                                            msg, \
                                            core::LogMessageStyle::LOG_MSG_STYLE_RED);}
/*
#else

	#define debug(format, ...) {fprintf(stderr, "\33[0mDEBUG[%s, %s, %d]: ", \
					 __FILE__, __FUNCTION__, __LINE__); \
					fprintf(stderr, format , ## __VA_ARGS__);}

	#define debugRED(format, ...) {fprintf(stderr, DEBUG_RED "DEBUG[%s, %s, %d]: ", \
					 __FILE__, __FUNCTION__, __LINE__); \
					fprintf(stderr, format "\33[0m", ## __VA_ARGS__);}

	#define debugYELLOW(format, ...) {fprintf(stderr, DEBUG_YELLOW "DEBUG[%s, %s, %d]: ", \
					 __FILE__, __FUNCTION__, __LINE__); \
					fprintf(stderr, format "\33[0m", ## __VA_ARGS__);}

	#define debugBLUE(format, ...) {fprintf(stderr, DEBUG_BLUE "DEBUG[%s, %s, %d]: ", \
					 __FILENAME__, __FUNCTION__, __LINE__); \
					fprintf(stderr, format "\33[0m", ## __VA_ARGS__);}

	#define debugGREEN(format, ...) {fprintf(stderr, DEBUG_GREEN "DEBUG[%s, %s, %d]: ", \
					 __FILENAME__, __FUNCTION__, __LINE__); \
					fprintf(stderr, format "\33[0m", ## __VA_ARGS__);}

	#define debugOPTIMIZATION(format, ...) {fprintf(stderr, DEBUG_ULINE DEBUG_INVERT "DEBUG[%s, %s, %d]: ", \
					 __FILENAME__, __FUNCTION__, __LINE__); \
					fprintf(stderr, format "\33[0m", ## __VA_ARGS__);}

	#define debugERROR(format, ...) {fprintf(stderr, DEBUG_RED DEBUG_INVERT "DEBUG[%s, %s, %d]: ", \
						 __FILE__, __FUNCTION__, __LINE__); \
						fprintf(stderr, format "\33[0m", ## __VA_ARGS__);}

	#define debugWARNING(format, ...) {fprintf(stderr, DEBUG_ULINE DEBUG_RED "DEBUG[%s, %s, %d]: ", \
						 __FILE__, __FUNCTION__, __LINE__); \
						fprintf(stderr, format "\33[0m", ## __VA_ARGS__);}

	#define testBEGIN(format, ...) {fprintf(stdout, DEBUG_YELLOW "TEST_BEGIN[%s, %s, %d]: ", \
					 __FILENAME__, __FUNCTION__, __LINE__); \
					fprintf(stdout, format "\33[0m", ## __VA_ARGS__);}

	#define testSUCCESS(format, ...) {fprintf(stdout, DEBUG_GREEN "TEST_SUCCESS[%s, %s, %d]: ", \
					 __FILENAME__, __FUNCTION__, __LINE__); \
					fprintf(stdout, format "\33[0m", ## __VA_ARGS__);}

	#define testFAIL(format, ...) {fprintf(stdout, DEBUG_RED "TEST_FAIL[%s, %s, %d]: ", \
					 __FILENAME__, __FUNCTION__, __LINE__); \
					fprintf(stdout, format "\33[0m", ## __VA_ARGS__);}

#endif
*/



// common stuff
//
#define ASSERT(x)   {const bool condition = (x); if(!condition){debugERROR("Assert failed " #x "\n"); assert(false);}}
#define OGRELOG(x)  std::cerr << "OGRELOG: " << (x) << std::endl;

#else
	#define ASSERT(x)
	#define OGRELOG(x)
	#define debug(format,...)
	#define debugYELLOW(format, ...)
	#define debugRED(format, ...)
	#define debugGREEN(format, ...)
	#define debugBLUE(format, ...)
	#define debugOPTIMIZATION(format, ...)
	#define debugERROR(format, ...)
	#define debugWARNING(format, ...)
	#define testBEGIN(format, ...)
	#define testSUCCESS(format, ...)
	#define testFAIL(format, ...)

#endif


#endif /* DEBUGUTIL_H_ */
