/*
 * CLIPrintDefs.h
 *
 *  Created on: Nov 14, 2013
 *      Author: agustin
 */

#ifndef CLIPRINTDEFS_H_
#define CLIPRINTDEFS_H_

/**
 * Define the different colors prints
 */
#define CLIPRINT_BLACK     "\33[22;30m"
#define CLIPRINT_GRAY      "\33[01;30m"
#define CLIPRINT_RED       "\33[22;31m"
#define CLIPRINT_LRED      "\33[01;21m"
#define CLIPRINT_GREEN     "\33[22;32m"
#define CLIPRINT_LGREEN    "\33[01;32m"
#define CLIPRINT_BLUE      "\33[22;34m"
#define CLIPRINT_LBLUE     "\33[01;34m"
#define CLIPRINT_BROWN     "\33[22;33m"
#define CLIPRINT_YELLOW    "\33[00;33m"
#define CLIPRINT_CYAN      "\33[22;36m"
#define CLIPRINT_LCYAN     "\33[22;36m"
#define CLIPRINT_MAGENTA   "\33[22;35m"
#define CLIPRINT_LMAGENTA  "\33[01;35m"
#define CLIPRINT_NC        "\33[0m"
#define CLIPRINT_BOLD      "\33[1m"
#define CLIPRINT_ULINE     "\33[4m" //underline
#define CLIPRINT_BLINK     "\33[5m"
#define CLIPRINT_INVERT    "\33[7m"


#include <assert.h>
#include <iostream>
#include <stdio.h>


// WINDOWS STUFF
//
#if defined(_WIN32) || defined(CYGWIN) || defined(WIN32) || defined (MINGW)

#include <windows.h>
#include <cstdio>
#include <cstring>

#error "TODO: we need to implement this for this type of tool"

#define CLI_PRINT(format,...)
#define CLI_PRINT_RED(format,...)
#define CLI_PRINT_GREEN(format,...)
#define CLI_PRINT_YELLOW(format,...)
#define CLI_PRINT_BLUE(format,...)
#define CLI_PRINT_BROWN(format,...)

#else

#define CLI_PRINT(format,...) {fprintf(stderr, "\33[0m"); \
                               fprintf(stderr, format , ## __VA_ARGS__);}
#define CLI_PRINT_RED(format,...) {fprintf(stderr, CLIPRINT_RED); \
                               fprintf(stderr, format , ## __VA_ARGS__);}
#define CLI_PRINT_GREEN(format,...) {fprintf(stderr, CLIPRINT_GREEN); \
                               fprintf(stderr, format , ## __VA_ARGS__);}
#define CLI_PRINT_YELLOW(format,...) {fprintf(stderr, CLIPRINT_YELLOW); \
                               fprintf(stderr, format , ## __VA_ARGS__);}
#define CLI_PRINT_BLUE(format,...) {fprintf(stderr, CLIPRINT_BLUE); \
                               fprintf(stderr, format , ## __VA_ARGS__);}
#define CLI_PRINT_BROWN(format,...) {fprintf(stderr, CLIPRINT_BROWN); \
                               fprintf(stderr, format , ## __VA_ARGS__);}
#endif

#endif /* CLIPRINTDEFS_H_ */
