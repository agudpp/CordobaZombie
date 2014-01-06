/*
 * QtOverlayViewer.cpp
 *
 *  Created on: Dec 29, 2013
 *      Author: agustin
 */

#include "QtOverlayViewer.h"

#include <algorithm>
#include <iterator>

#include <QFileDialog>
#include <QFileInfo>

#include <OgreOverlayManager.h>

#include <input/InputHelper.h>
#include <input/InputMouse.h>

#include <os_utils/OSHelper.h>



// Help stuff
//
namespace {

}




namespace tool {


////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::getOvleraysLoaded(std::vector<Ogre::Overlay*>& overlays)
{
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
    Ogre::OverlayManager::OverlayMapIterator mapIt = om.getOverlayIterator();

    overlays.clear();
    while (mapIt.hasMoreElements()) {
        Ogre::Overlay* ov = mapIt.getNext();
        if (ov) {
            overlays.push_back(ov);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::destroyOverlayData(const std::set<Ogre::Overlay*>& except)
{
    std::vector<OverlayWidgetItem*> toDestroy;
    auto exceptEnd = except.end();

    for (OverlayWidgetItem* ow : mOverlayWidgets) {
        Ogre::Overlay* ov = ow->overlay();
        if (except.find(ov) != exceptEnd) {
            // we don't have to destroy this one
            continue;
        }

        // we have to destroy this one
        delete ow;
        toDestroy.push_back(ow);

        // destroy from the map also
        auto mapElem = mOverlayMap.find(ov);
        mOverlayMap.erase(ov);
    }

    for (OverlayWidgetItem* o : toDestroy) {
        // destroy this element
        mOverlayWidgets.erase(std::find(mOverlayWidgets.begin(), mOverlayWidgets.end(), o));
    }
}

////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::updateAllOverlays(void)
{
    // get the current parsed overlays
    std::vector<Ogre::Overlay*> overlays;
    getOvleraysLoaded(overlays);

    // check which are the widgets we need to create
    std::set<Ogre::Overlay*> currentOverlays;
    std::set<Ogre::Overlay*> toLoad;
    std::set<Ogre::Overlay*> ogreOverlays(overlays.begin(), overlays.end());

    for (OverlayWidgetItem* ow : mOverlayWidgets) {
        currentOverlays.insert(ow->overlay());
    }

    // remove all the current overlays widgets except for the ones we have now
    destroyOverlayData(currentOverlays);

    // now we have to get all the overlays we have to load that are
    // ogreOverlays - currentOverlays
    std::set_difference(ogreOverlays.begin(), ogreOverlays.end(),
                     currentOverlays.begin(), currentOverlays.end(),
                     std::inserter(toLoad, toLoad.end()));

    if (toLoad.empty()) {
        // nothing to do
        return;
    }

    for (Ogre::Overlay* o : toLoad) {
        OverlayWidgetItem* item = new OverlayWidgetItem(o, this);
        item->build();
        mScrollAreaLayout->addWidget(item);

        // track it
        mOverlayWidgets.push_back(item);

        // add the item to the map
        mOverlayMap.insert(std::make_pair(o, item));
    }

    // set the layout to the scroll area
    if (ui.overlayScrollArea->widget() != mScrollWidget) {
        ui.overlayScrollArea->setWidget(mScrollWidget);
    }

}

////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::onLoadRscClicked(bool)
{
    // try to load the .mesh file
     const QString fileName = QFileDialog::getOpenFileName(this,
                                                          tr("Open Resource File"),
                                                          mLastPathLoaded,
                                                          tr("Resources files (*.cfg)"));

    // we will only want the first
    if (fileName.isEmpty()) {
        return;
    }

    QFileInfo finfo(fileName);
    if (finfo.isDir()) {
        // we need to add the location only
        loadLocationFromPath(fileName);
    } else {
        // else is a cfg file
        loadResourceFile(fileName);
    }

    // save the last path used
    mLastPathLoaded = finfo.path();

    // load all the overlays
    updateAllOverlays();
}


////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::frameUpdate(float fp)
{
}

////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::systemsReady(void)
{
    ASSERT(ogreWidget());
    ASSERT(ogreWidget()->ogreData().sceneManager);
    ASSERT(ogreWidget()->ogreData().camera);
    ASSERT(ogreWidget()->ogreData().viewport);
    ASSERT(ogreWidget()->ogreData().renderWindow);

    // load the GlobalResources.cfg file
    char* envPath = 0;
    if (!core::OSHelper::getEnvVar("CZ01_RC_PATH", envPath)) {
        QTDEBUG_WARNING("You haven't set the environment variable CZ01_RC_PATH. This"
            " can cause some problems or errors during the execution of the tool\n");
    } else {
        std::string path = envPath;
        core::OSHelper::addEndPathVar(path);
        path.append("GlobalResources.cfg");
        if (!loadResourceFile(QString(path.c_str()))) {
            QTDEBUG_WARNING("We couldn't load the GlobalResources.cfg file from " <<
                            path << " this can cause some problems during the "
                                "execution of this app.\n");
        }
    }

    // build the cursor now
    mCursor.build();
    Ogre::RenderWindow* rw = ogreWidget()->ogreData().renderWindow;
    mCursor.setWindowDimensions(rw->getWidth(), rw->getHeight());
}

////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::OgreWidgetKeyPressEvent(QKeyEvent* event)
{

}

////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::OgreWidgetKeyReleaseEvent(QKeyEvent* event)
{

}

////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::OgreWidgetMousePressEvent(QMouseEvent* event)
{

}

////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::OgreWidgetMouseReleaseEvent(QMouseEvent* event)
{

}

////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::OgreWidgetMouseMoveEvent(QMouseEvent* event)
{
    // set the position of the cursor
    if (mCursor.isBuilt()) {
        mCursor.updatePosition(event->x(), event->y());
    }
}

////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::OgreWidgetResizeEvent(QResizeEvent* event)
{
    if (mCursor.isBuilt()) {
        ASSERT(ogreWidget());
        Ogre::RenderWindow* rw = ogreWidget()->ogreData().renderWindow;
        ASSERT(rw);
        mCursor.setWindowDimensions(rw->getWidth(), rw->getHeight());
    }
}

////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::keyPressEvent(QKeyEvent* event)
{
}

////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::keyReleaseEvent(QKeyEvent* event)
{
    event->ignore();
}
////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::mousePressEvent(QMouseEvent* event)
{
    event->ignore();
}

////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::mouseReleaseEvent(QMouseEvent* event)
{
    event->ignore();
}

////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::mouseMoveEvent(QMouseEvent* event)
{

}

////////////////////////////////////////////////////////////////////////////////
QtOverlayViewer::QtOverlayViewer(rrh::ResourceHandler* rh,
                           const std::string& plugins,
                           const std::string& ogre,
                           const std::string& resource) :
    QtOgreAppBase(rh, plugins, ogre, resource)
,   mLastPathLoaded(".")
,   mScrollWidget(new QWidget(this))
,   mScrollAreaLayout(new QVBoxLayout(mScrollWidget))
,   mCursor(false)
{
    ui.setupUi(this);

    // connect signals and slots
    connect(ui.addRscBtn, SIGNAL(clicked(bool)), this, SLOT(onLoadRscClicked(bool)));

    // add the ogre widget to the layout
    QWidget* ogrew = ogreWidget();
    ogrew->setSizePolicy(QSizePolicy::Policy::Expanding,
                         QSizePolicy::Policy::Expanding);
    ui.horizontalLayout->addWidget(ogreWidget());

    // receive events when mouse is moving and hide the cursor
    ogrew->setMouseTracking(true);
    ogrew->setCursor(Qt::BlankCursor);
}


////////////////////////////////////////////////////////////////////////////////
QtOverlayViewer::~QtOverlayViewer()
{
}



////////////////////////////////////////////////////////////////////////////////
// Slots

////////////////////////////////////////////////////////////////////////////////

}
