/*
 * QtDebug.cpp
 *
 *  Created on: Dec 29, 2013
 *      Author: agustin
 */

#include "QtDebug.h"

namespace tool {

QtDebug::QtDebug()
{
}

QtDebug::~QtDebug()
{
}

void
QtDebug::log(const std::stringstream& message,
             Type type,
             bool showWindow)
{
    const std::string msg = message.str();
    switch (type) {
        case Type::QTDEBUG_CRITICAL:
            debugERROR("%s", msg.c_str());
            if (showWindow) {
                QMessageBox::critical(0, "Critical Error", msg.c_str());
            }
            break;
        case Type::QTDEBUG_WARNING:
            debugWARNING("%s", msg.c_str());
            if (showWindow) {
                QMessageBox::warning(0, "Warning!", msg.c_str());
            }
            break;
        case Type::QTDEBUG_DEBUG:
            debug("%s", msg.c_str());
            if (showWindow) {
                QMessageBox::information(0,
                                         "Not an error, only information",
                                         msg.c_str());
            }
            break;
        default:
            break;
    }
}

}
