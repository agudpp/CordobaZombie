/*
 * ConfigDialog main
 *
 *  Created on: Dec 15, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban
 */


#include <QString>
#include <QMessageBox>
#include <QApplication>
#include <EngineConfiguration.h>
#include "CbaZombieConfigDialog.h"

// Engine Configuration file default name
#define  EC_DEFAULT_FNAME  "EngineConfig.xml"


int main(int argc, char *argv[])
{
    int ok(0);
    QApplication app(argc, argv);
    engine::EngineConfiguration ec;
    engine::CbaZombieConfigDialog configDialog;

    if (argc > 1) {
        // User specified Engine Configuration file
        ok = ec.load(argv[1]);
    } else {
        // Default Engine Configuration file
        QMessageBox::information(0,"Warning",
                                 QString("Searching for file \"") +
                                 EC_DEFAULT_FNAME + "\" in current directory.");
        ok = ec.load(EC_DEFAULT_FNAME);
    }
    if (!ok) {
        QMessageBox::critical(0, "Error","Bad or inexistent "
                              "engine configuration file, discarding changes.");
        return 1;
    }

    // Run Qt application
    ok = configDialog.init(ec);
    if (!ok) {
        QMessageBox::critical(0, "Error","Bad engine configuration , "
                              "discarding changes.");
        return 1;
    }
    configDialog.show();
    ok = !app.exec();

    return ok;
}
