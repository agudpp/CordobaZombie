/*
 * CrashHandler.cpp
 *
 *  Created on: Dec 12, 2013
 *      Author: agustin
 */

#include "CrashHandler.h"


// For stack trace print and signal handling
#include <csignal>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <ucontext.h>
#include <unistd.h>
// logger
#include <logger/LoggerManager.h>



// Auxiliary methods
//
namespace {

// this is the bactrace handler method
//
static void
bt_sighandler(int sig, siginfo_t *info, void *secret)
{
    void *array[128];
    char header[250];

    // get the signal
    header[0] = '\0';
    sprintf(header,
            "Caught signal %d (%s)\33[0m\n",
            sig,
            sig == SIGSEGV ? "SIGSEGV" : (sig == SIGABRT ? "SIGABRT" : (sig
                == SIGKILL ? "SIGKILL" : (sig == SIGTERM ? "SIGTERM" : (sig
                == SIGUSR1 ? "SIGUSR1" : "UNKNOWN")))));

    // get void*'s for all entries on the stack
    unsigned int size = backtrace(array, 128);

    // print out all the frames to stderr
    char** symbols = backtrace_symbols(array, size);

    core::LoggerManager::instance().log(
        "\n\n"
        "-----------------------------------------------------------------------\n"
        "\t\t\tCRASH DETECTED!\n"
        "-----------------------------------------------------------------------\n",
        core::LogMessageStyle::LOG_MSG_STYLE_INVERT_RED);

    // if we have header, print it
    if (strlen(header) > 0) {
        core::LoggerManager::instance().log(header,
                                            core::LogMessageStyle::LOG_MSG_STYLE_RED);
    }

    if (symbols) {
        // now create the buffer of the stacktrace
        char backtraceStr[2048];
        backtraceStr[0] = '\0';

        for (int i = 0; i < size; ++i) {
            strcat(backtraceStr, symbols[i]);
            strcat(backtraceStr, "\n");
        }
        core::LoggerManager::instance().log(backtraceStr,
                                            core::LogMessageStyle::LOG_MSG_STYLE_RED);
        free(symbols);
    } else {
        core::LoggerManager::instance().log("Error getting the backtrace\n",
                                            core::LogMessageStyle::LOG_MSG_STYLE_RED);
    }
    exit(1);
}

// Configure signals
//
static void
bt_configureSignals(void)
{
    /* Install our signal handler */
    struct sigaction sa;

    sa.sa_sigaction = bt_sighandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGKILL, &sa, NULL);
    sigaction(SIGILL, &sa, NULL);
    sigaction(SIGFPE, &sa, NULL);

    /* ... add any other signal here */
}
}

namespace core {

// Windows implementation

CrashHandler::CrashHandler()
{

}

CrashHandler::~CrashHandler()
{
}


bool
CrashHandler::configureSignals(void)
{
    bt_configureSignals();
    return true;
}

} /* namespace core */
