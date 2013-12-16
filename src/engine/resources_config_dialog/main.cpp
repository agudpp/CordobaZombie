/*
 * ConfigDialog main
 *
 *  Created on: Dec 15, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban
 */

#include <string>
#include <QApplication>
#include <debug/DebugUtil.h>
#include <EngineConfiguration.h>
#include "CbaZombieConfigDialog.h"

// Engine Configuration file default name
#define  EC_DEFAULT_FNAME  "EngineConfig.xml"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    engine::EngineConfiguration ec;
    engine::CbaZombieConfigDialog configDialog;

    if (argc > 1)
        // User-passed Engine Configuration file
        ec.load(argv[1]);
    else
        // Default Engine Configuration file
        ec.load(EC_DEFAULT_FNAME);

    // Run Qt application
    configDialog.init(ec);
    configDialog.show();
    int fail = app.exec();
    if (fail)
        debugWARNING("Bad Ogre configuration, discarding changes.\n");

    return fail;
}
