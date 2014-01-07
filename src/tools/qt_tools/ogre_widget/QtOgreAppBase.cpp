/*
 * QtOgreAppBase.cpp
 *
 *  Created on: Dec 29, 2013
 *      Author: agustin
 */

#include "QtOgreAppBase.h"

#include <QFileInfo>

#include <OgreResourceManager.h>

#include <ResourceHandler.h>

namespace tool {

QtOgreAppBase::QtOgreAppBase(rrh::ResourceHandler* rh,
                             const std::string& plugins,
                             const std::string& ogre,
                             const std::string& resource) :
    QWidget(0)
,   mOgreWidget(0)
,   mResourceHandler(rh)
{
    mOgreWidget = new OgreWidget(0,
                                 rh);
    mOgreWidget->resize(240, 240);

    // connect the frame event
    connect(mOgreWidget, SIGNAL(frameUpdate(float)), this, SLOT(frameUpdate(float)));
    connect(mOgreWidget, SIGNAL(systemsReady(void)), this, SLOT(systemsReady(void)));
    connect(mOgreWidget, SIGNAL(keyPressEventSig(QKeyEvent*)),
            this, SLOT(OgreWidgetKeyPressEvent(QKeyEvent*)));
    connect(mOgreWidget, SIGNAL(keyReleaseEventSig(QKeyEvent*)),
            this, SLOT(OgreWidgetKeyReleaseEvent(QKeyEvent*)));
    connect(mOgreWidget, SIGNAL(mousePressEventSig(QMouseEvent*)),
            this, SLOT(OgreWidgetMousePressEvent(QMouseEvent*)));
    connect(mOgreWidget, SIGNAL(mouseReleaseEventSig(QMouseEvent*)),
            this, SLOT(OgreWidgetMouseReleaseEvent(QMouseEvent*)));
    connect(mOgreWidget, SIGNAL(mouseMoveEventSig(QMouseEvent*)),
            this, SLOT(OgreWidgetMouseMoveEvent(QMouseEvent*)));

    connect(mOgreWidget, SIGNAL(resizeSig(QResizeEvent*)),
            this, SLOT(OgreWidgetResizeEvent(QResizeEvent*)));
}

QtOgreAppBase::~QtOgreAppBase()
{
    delete mOgreWidget;
}

////////////////////////////////////////////////////////////////////////////////
void
QtOgreAppBase::loadLocationFromPath(const QString& path)
{
    Ogre::ResourceGroupManager& rmngr = Ogre::ResourceGroupManager::getSingleton();

    if (mLocations.contains(path)) {
        // do nothing
        return;
    }
    // else add it to the locations
    try {
        rmngr.addResourceLocation(path.toStdString(), "FileSystem", "QtSources");
        rmngr.initialiseAllResourceGroups();
        rmngr.initialiseResourceGroup("QtSources");
        rmngr.loadResourceGroup("QtSources");
        mLocations.insert(path);
    } catch (std::exception& e) {
        QTDEBUG_CRITICAL("Error trying to load resource location " <<
                         path.toStdString() << " exception: " <<
                         e.what() << "\n");
    }
}

////////////////////////////////////////////////////////////////////////////////
void
QtOgreAppBase::loadLocationFromFile(const QString& filepath)
{
    Ogre::ResourceGroupManager& rmngr = Ogre::ResourceGroupManager::getSingleton();

    QFileInfo finfo(filepath);
    const QString path = finfo.path();
    loadLocationFromPath(path);
}

////////////////////////////////////////////////////////////////////////////////
bool
QtOgreAppBase::loadResourceFile(const QString& cfg)
{
    QFileInfo finfo(cfg);
    if (!finfo.exists()) {
        QTDEBUG_WARNING("Resource file " << cfg.toStdString() << " doesn't exists\n");
        return false;
    }

    const QString path = finfo.absoluteFilePath();
    // check if the resource file already exists
    if (mLocations.contains(path)) {
       // do nothing
       return true;
    }

    rrh::ResourceGroup rg;

    rg.setOgreResourceFile(path.toStdString());
    ASSERT(mResourceHandler);

    if (!mResourceHandler->loadResourceGroup(rg)) {
        return false; // some error occur
    }

    // add this to the paths already tracked
    mLocations.insert(path);
    return true;
}


}
