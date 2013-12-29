#include <QApplication>
#include <QWidget>
#include "OgreWidget.h"
#include <QVBoxLayout>
#include <QtGui>
#include <QVBoxLayout>

int main(int argc, char* argv[])
{
   QApplication app(argc, argv);

   QWidget window;

   window.resize(800, 600);
   window.setWindowTitle(
      QApplication::translate("topLevel", "Top-Level widget"));

   OgreWidget* ogreWidget = new OgreWidget;

   QVBoxLayout* layout = new QVBoxLayout;
   layout->addWidget(ogreWidget);

   window.setLayout(layout);
   window.show();

   return app.exec();
}
