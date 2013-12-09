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


// common stuff
//
#define ASSERT(x)   assert(x);
#define OGRELOG(x)  std::cerr << "OGRELOG: " << (x) << std::endl;


// WINDOWS STUFF
//
#if defined(_WIN32) || defined(CYGWIN) || defined(WIN32) || defined (MINGW)

#include <windows.h>
#include <cstdio>
#include <cstring>

#include <fstream>

#define DEBUG_FILE_NAME	"debug.txt"

static std::ofstream DEBUG_FILE;

/*#define WIN_DEBUG_MACRO(TYPE, showWindow) {char msg[1024]; std::sprintf(msg, TYPE "[%s,%s, %d]: ", \
		__FILE__, __FUNCTION__, __LINE__);char buff[512];\
		std::sprintf(buff, format, ## __VA_ARGS__);\
		std::strcat(msg, " "); std::strcat(msg, buff);\
		if (showWindow) MessageBox( NULL, msg, TYPE, MB_OK | MB_ICONERROR | MB_TASKMODAL);\
		if (!DEBUG_FILE.is_open()) {\
			DEBUG_FILE.open(DEBUG_FILE_NAME);\
		}\
		DEBUG_FILE << msg;\
		DEBUG_FILE.flush();}
*/
// TODO: note that there are not outputs for printf in console for mingw apps since 
// we are compiling it probably compiled with the -mwindows flag (this disconnect
// the stdout and stderr from the console). We will need to write the data to some log
// file
	#define debug(format,...) {char msg[1024]; std::sprintf(msg, "debug" "[%s,%s, %d]: ", \
		__FILE__, __FUNCTION__, __LINE__);char buff[512];\
		std::sprintf(buff, format, ## __VA_ARGS__);\
		std::strcat(msg, " "); std::strcat(msg, buff);\
		bool showWindow = false;\
		if (showWindow) MessageBox( NULL, msg, "debug", MB_OK | MB_ICONERROR | MB_TASKMODAL);\
		if (!DEBUG_FILE.is_open()) {\
			DEBUG_FILE.open(DEBUG_FILE_NAME);\
		}\
		DEBUG_FILE << msg;\
		DEBUG_FILE.flush();}
	#define debugYELLOW(format, ...) {char msg[1024]; std::sprintf(msg, "YELLOW" "[%s,%s, %d]: ", \
		__FILE__, __FUNCTION__, __LINE__);char buff[512];\
		std::sprintf(buff, format, ## __VA_ARGS__);\
		std::strcat(msg, " "); std::strcat(msg, buff);\
		bool showWindow = false;\
		if (showWindow) MessageBox( NULL, msg, "YELLOW", MB_OK | MB_ICONERROR | MB_TASKMODAL);\
		if (!DEBUG_FILE.is_open()) {\
			DEBUG_FILE.open(DEBUG_FILE_NAME);\
		}\
		DEBUG_FILE << msg;\
		DEBUG_FILE.flush();}
	#define debugRED(format, ...)  {char msg[1024]; std::sprintf(msg, "RED" "[%s,%s, %d]: ", \
		__FILE__, __FUNCTION__, __LINE__);char buff[512];\
		std::sprintf(buff, format, ## __VA_ARGS__);\
		std::strcat(msg, " "); std::strcat(msg, buff);\
		bool showWindow = false;\
		if (showWindow) MessageBox( NULL, msg, "RED", MB_OK | MB_ICONERROR | MB_TASKMODAL);\
		if (!DEBUG_FILE.is_open()) {\
			DEBUG_FILE.open(DEBUG_FILE_NAME);\
		}\
		DEBUG_FILE << msg;\
		DEBUG_FILE.flush();}
	#define debugGREEN(format, ...)  {char msg[1024]; std::sprintf(msg, "GREEN" "[%s,%s, %d]: ", \
		__FILE__, __FUNCTION__, __LINE__);char buff[512];\
		std::sprintf(buff, format, ## __VA_ARGS__);\
		std::strcat(msg, " "); std::strcat(msg, buff);\
		bool showWindow = false;\
		if (showWindow) MessageBox( NULL, msg, "GREEN", MB_OK | MB_ICONERROR | MB_TASKMODAL);\
		if (!DEBUG_FILE.is_open()) {\
			DEBUG_FILE.open(DEBUG_FILE_NAME);\
		}\
		DEBUG_FILE << msg;\
		DEBUG_FILE.flush();}
	#define debugBLUE(format, ...) {char msg[1024]; std::sprintf(msg, "BLUE" "[%s,%s, %d]: ", \
		__FILE__, __FUNCTION__, __LINE__);char buff[512];\
		std::sprintf(buff, format, ## __VA_ARGS__);\
		std::strcat(msg, " "); std::strcat(msg, buff);\
		bool showWindow = false;\
		if (showWindow) MessageBox( NULL, msg, "BLUE", MB_OK | MB_ICONERROR | MB_TASKMODAL);\
		if (!DEBUG_FILE.is_open()) {\
			DEBUG_FILE.open(DEBUG_FILE_NAME);\
		}\
		DEBUG_FILE << msg;\
		DEBUG_FILE.flush();}
	#define debugOPTIMIZATION(format, ...)  {char msg[1024]; std::sprintf(msg, "OPTIMIZATION" "[%s,%s, %d]: ", \
		__FILE__, __FUNCTION__, __LINE__);char buff[512];\
		std::sprintf(buff, format, ## __VA_ARGS__);\
		std::strcat(msg, " "); std::strcat(msg, buff);\
		bool showWindow = false;\
		if (showWindow) MessageBox( NULL, msg, "OPTIMIZATION", MB_OK | MB_ICONERROR | MB_TASKMODAL);\
		if (!DEBUG_FILE.is_open()) {\
			DEBUG_FILE.open(DEBUG_FILE_NAME);\
		}\
		DEBUG_FILE << msg;\
		DEBUG_FILE.flush();}
	#define debugERROR(format, ...) {char msg[1024]; std::sprintf(msg, "ERROR" "[%s,%s, %d]: ", \
		__FILE__, __FUNCTION__, __LINE__);char buff[512];\
		std::sprintf(buff, format, ## __VA_ARGS__);\
		std::strcat(msg, " "); std::strcat(msg, buff);\
		bool showWindow = true;\
		if (showWindow) MessageBox( NULL, msg, "ERROR", MB_OK | MB_ICONERROR | MB_TASKMODAL);\
		if (!DEBUG_FILE.is_open()) {\
			DEBUG_FILE.open(DEBUG_FILE_NAME);\
		}\
		DEBUG_FILE << msg;\
		DEBUG_FILE.flush();}
	#define debugWARNING(format, ...)  {char msg[1024]; std::sprintf(msg, "WARNING" "[%s,%s, %d]: ", \
		__FILE__, __FUNCTION__, __LINE__);char buff[512];\
		std::sprintf(buff, format, ## __VA_ARGS__);\
		std::strcat(msg, " "); std::strcat(msg, buff);\
		bool showWindow = true;\
		if (showWindow) MessageBox( NULL, msg, "WARNING", MB_OK | MB_ICONERROR | MB_TASKMODAL);\
		if (!DEBUG_FILE.is_open()) {\
			DEBUG_FILE.open(DEBUG_FILE_NAME);\
		}\
		DEBUG_FILE << msg;\
		DEBUG_FILE.flush();}
	#define testBEGIN(format, ...)
	#define testSUCCESS(format, ...)
	#define testFAIL(format, ...)

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
