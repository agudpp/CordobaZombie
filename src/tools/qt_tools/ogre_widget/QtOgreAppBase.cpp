/*
 * QtOgreAppBase.cpp
 *
 *  Created on: Dec 29, 2013
 *      Author: agustin
 */

#include "QtOgreAppBase.h"

#include <QFileInfo>

#include <OgreResourceManager.h>

namespace tool {

QtOgreAppBase::QtOgreAppBase(rrh::ResourceHandler* rh,
                             bool useInputHelper,
                             const std::string& plugins,
                             const std::string& ogre,
                             const std::string& resource) :
    QWidget(0)
,   mOgreWidget(0)
{
    mOgreWidget = new OgreWidget(0,
                                 rh,
                                 useInputHelper);
    mOgreWidget->resize(800, 600);

    // connect the frame event
    connect(mOgreWidget, SIGNAL(frameUpdate(float)), this, SLOT(frameUpdate(float)));
    connect(mOgreWidget, SIGNAL(systemsReady(void)), this, SLOT(systemsReady(void)));
}

QtOgreAppBase::~QtOgreAppBase()
{
}


////////////////////////////////////////////////////////////////////////////////
void
QtOgreAppBase::loadLocationFromFile(const QString& filepath)
{
    Ogre::ResourceGroupManager& rmngr = Ogre::ResourceGroupManager::getSingleton();

    QFileInfo finfo(filepath);
    const QString path = finfo.path();
    if (mLocations.contains(path)) {
        // do nothing
        return;
    }
    // else add it to the locations
    mLocations.insert(path);
    rmngr.addResourceLocation(path.toStdString(), "FileSystem");
}


}
