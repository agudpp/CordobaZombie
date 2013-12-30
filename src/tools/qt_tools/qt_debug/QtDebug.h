/*
 * QtDebug.h
 *
 *  Created on: Dec 29, 2013
 *      Author: agustin
 */

#ifndef QTDEBUG_H_
#define QTDEBUG_H_

#include <sstream>

#include <QMessageBox>

#include <debug/DebugUtil.h>

namespace tool {

// Define a helper information for the debuggin data
//
#define QTDEBUG_HEADER "[" << __FILE__ << ", " << __FUNCTION__ << ", " << __LINE__ << " ]: "
#define QTDEBUG_CRITICAL(x) {std::stringstream ss; ss << QTDEBUG_HEADER << x;\
                             tool::QtDebug::instance().log(ss,\
                                                    tool::QtDebug::Type::QTDEBUG_CRITICAL,\
                                                    true);}
#define QTDEBUG_WARNING(x) {std::stringstream ss; ss << QTDEBUG_HEADER << x;\
                            tool::QtDebug::instance().log(ss,\
                                                   tool::QtDebug::Type::QTDEBUG_WARNING,\
                                                   true);}
#define QTDEBUG_DEBUG(x) {std::stringstream ss; ss << QTDEBUG_HEADER << x;\
                          tool::QtDebug::instance().log(ss,\
                                                 tool::QtDebug::Type::QTDEBUG_DEBUG,\
                                                 false);}

class QtDebug
{
public:
    // Different type of debug information
    //
    enum Type {
        QTDEBUG_CRITICAL = 0,
        QTDEBUG_WARNING,
        QTDEBUG_DEBUG,
    };
public:

    // @brief Get the singleton instance
    //
    static QtDebug&
    instance(void)
    {
        static QtDebug instance;
        return instance;
    }
    ~QtDebug();

    // @brief Log a message and debug a message
    // @param message       The message to show
    // @param type          The message type (critical, warning, etc)
    // @param showWindow    True if we want to show a windows message or false
    //                      if not
    //
    void
    log(const std::stringstream& message,
        Type type,
        bool showWindow = true);

private:
    QtDebug();
    QtDebug(const QtDebug&);
    QtDebug& operator=(const QtDebug&);

};

}

#endif /* QTDEBUG_H_ */
