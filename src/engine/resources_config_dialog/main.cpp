/*
 * ConfigDialog main
 *
 *  Created on: Dec 15, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban
 */

#include <QApplication>
#include <debug/DebugUtil.h>
#include "CbaZombieConfigDialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    engine::CbaZombieConfigDialog configDialog;
    configDialog.restoreConfig();
    configDialog.show();

    // Run Qt application
    int fail = app.exec();
    if (fail)
        debugWARNING("Bad Ogre configuration, discarding changes.\n");

    return fail;
}
