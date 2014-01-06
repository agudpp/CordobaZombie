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
    std::vector<OverlayData*> toDestroy;
    auto exceptEnd = except.end();

    for (OverlayData* od : mOverlayData) {
        if (except.find(od->overlay) != exceptEnd) {
            // do nothing with this, continue
            continue;
        }

        // we need to remove this one
        toDestroy.push_back(od);
        for (OverlayElementData* oed : od->elements) {
            delete oed->checkbox;
            delete oed->label;
            delete oed;
        }
        delete od->checkbox;
        delete od->label;
        delete od->layout;
        delete od;
    }
    for (OverlayData* o : toDestroy) {
        // destroy this element
        mOverlayData.erase(std::find(mOverlayData.begin(), mOverlayData.end(), o));
    }
}

////////////////////////////////////////////////////////////////////////////////
void
QtOverlayViewer::updateAllOverlays(void)
{
    // get the current parsed overlays
    std::vector<Ogre::Overlay*> overlays;
    getOvleraysLoaded(overlays);

    // now get all the overlays we currently have.
    std::set<Ogre::Overlay*> currentOverlays;
    std::set<Ogre::Overlay*> toLoad;
    std::set<Ogre::Overlay*> ogreOverlays(overlays.begin(), overlays.end());

    for (OverlayData* od : mOverlayData) {
        currentOverlays.insert(od->overlay);
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

    // else we need to create all the overlays to be shown in the scroll area
    mOverlayData.reserve(toLoad.size());

    for (Ogre::Overlay* ov : toLoad) {
        OverlayData* ovd = new OverlayData;
        ovd->label = new QLabel(ov->getName().c_str(), this);
        ovd->checkbox = new QCheckBox(this);
        ovd->checkbox->setChecked(ov->isVisible());
        ovd->layout = new QHBoxLayout(0);
        ovd->overlay = ov;

        // create the overlay and add them
        ovd->layout->addWidget(ovd->label);
        ovd->layout->addWidget(ovd->checkbox);

        // connect the signal to show / hide elements
        connect(ovd->checkbox, SIGNAL(stateChanged(int)),
                this, SLOT(onOverlayCheckBoxChanged(int)));

        // now we have to create the child elements here
        // TODO:

        // now add the elements to the scroll area
        mScrollAreaLayout->addLayout(ovd->layout);

        // add to the current data
        mOverlayData.push_back(ovd);

        ovd->layout->addStretch(1);
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
QtOverlayViewer::onOverlayCheckBoxChanged(int state)
{
    // we will iterate over all the overlays and detect its state
    for (OverlayData* od : mOverlayData) {
        if (od->checkbox->isChecked()) {
            od->overlay->show();
        } else {
            od->overlay->hide();
        }
    }
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
{
    ui.setupUi(this);

    // connect signals and slots
    connect(ui.addRscBtn, SIGNAL(clicked(bool)), this, SLOT(onLoadRscClicked(bool)));

    // add the ogre widget to the layout
    QWidget* ogrew = ogreWidget();
    ogrew->setSizePolicy(QSizePolicy::Policy::Expanding,
                         QSizePolicy::Policy::Expanding);
    ui.horizontalLayout->addWidget(ogreWidget());
}


////////////////////////////////////////////////////////////////////////////////
QtOverlayViewer::~QtOverlayViewer()
{
}



////////////////////////////////////////////////////////////////////////////////
// Slots

////////////////////////////////////////////////////////////////////////////////

}
