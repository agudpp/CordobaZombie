/*
 * ConfigDialog main
 *
 *  Created on: Dec 15, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban
 */


#include <cstring>
#include <QString>
#include <QMessageBox>
#include <QApplication>
#include <EngineConfiguration.h>
#include "CbaZombieConfigDialog.h"

// Engine Configuration file default name
#define  EC_DEFAULT_FNAME  "EngineConfig.xml"
#define  EC_FNAME_MAX_LEN  256


int main(int argc, char *argv[])
{
    int ok(0);
    char engineConfigFilename[EC_FNAME_MAX_LEN];
    QApplication app(argc, argv);
    engine::CbaZombieConfigDialog configDialog;

    if (argc > 1) {
        // User specified Engine Configuration file
        strncpy(engineConfigFilename, argv[1], EC_FNAME_MAX_LEN);
    } else {
        /* I will comment this out just to avoid user confusions.
         * You can change it in the future or add a console debug (logger)
         * or any other kind of logging. */
//        // Default Engine Configuration file
//        QMessageBox::information(0,"Warning",
//                                 QString("Searching for file \"") +
//                                 EC_DEFAULT_FNAME + "\" in current directory.");
        strncpy(engineConfigFilename, EC_DEFAULT_FNAME, EC_FNAME_MAX_LEN);
    }

    // Run Qt application
    ok = configDialog.init(engineConfigFilename);
    if (ok) {
        configDialog.show();
        ok = !app.exec();
    } else {
        QMessageBox::critical(0, "Error","Something went wrong, "
                              "discarding configuration changes.");
    }

    return ok;
}
