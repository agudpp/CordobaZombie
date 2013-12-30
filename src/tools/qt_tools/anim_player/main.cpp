#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QtGui>
#include <QVBoxLayout>

#include <ResourceHandler.h>
#include <qt_tools/qt_debug/QtDebug.h>
#include <qt_tools/ogre_widget/OgreWidget.h>
#include <qt_tools/ogre_widget/QtOgreAppBase.h>

#include "QtAnimPlayer.h"

int
main(int argc, char* argv[])
{
   QApplication app(argc, argv);

   // the resource handler instance to be used by the app
   rrh::ResourceHandler resourceHandler;
   // TODO: configure the handler here

   // create the QtOgreAppBase
   tool::QtOgreAppBase* ogreApp = new tool::QtAnimPlayer(&resourceHandler,
                                                         true);

   // we could load the ogre widget right, now load the current app
   if (!ogreApp->loadApp()) {
       QTDEBUG_CRITICAL("Error loading the application itself! we need to abort!\n");
       return -2;
   }

   // everything is fine, show the window and run the app
   ogreApp->show();

   const int ret = app.exec();

   // delete the current ogre app
   delete ogreApp;

   return ret;
}
