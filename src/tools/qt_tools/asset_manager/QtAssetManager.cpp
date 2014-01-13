/*
 * QtAssetManager.cpp
 *
 *  Created on: Jan 13, 2014
 *      Author: agustin
 */

#include "QtAssetManager.h"


#include <os_utils/OSHelper.h>

namespace tool {



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
QtAssetManager::QtAssetManager(rrh::ResourceHandler* rh,
                               const std::string& plugins,
                               const std::string& ogre,
                               const std::string& resource) :
    QtOgreAppBase(rh, plugins, ogre, resource)
,   mOrbitCamera(0)
{
    ui.setupUi(this);

    // connect signals and slots
//    connect(ui.addRscBtn, SIGNAL(clicked(bool)), this, SLOT(onLoadRscClicked(bool)));

    // add the ogre widget to the layout
    QWidget* ogrew = ogreWidget();
    ogrew->setSizePolicy(QSizePolicy::Policy::Expanding,
                         QSizePolicy::Policy::Expanding);
    mOgreWidget->resize(800, 600);
    ui.verticalLayout->addWidget(ogrew);
}

////////////////////////////////////////////////////////////////////////////////
QtAssetManager::~QtAssetManager()
{
    delete mOrbitCamera;
}

////////////////////////////////////////////////////////////////////////////////
void
QtAssetManager::frameUpdate(float fp)
{

}

////////////////////////////////////////////////////////////////////////////////
void
QtAssetManager::systemsReady(void)
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


    OgreWidget::OgreData& data = ogreWidget()->ogreData();
    mOrbitCamera = new OrbitCamera(data.camera,
                                   data.sceneManager,
                                   data.frameTime,
                                   false);
    mOrbitCamera->setZoomDist(50.f);

    // create the axis to be shown in the scene
    m3DAxis = core::PrimitiveDrawer::instance().create3DAxis(Ogre::Vector3::ZERO, 15);
}


////////////////////////////////////////////////////////////////////////////////
void
QtAssetManager::keyPressEvent(QKeyEvent* event)
{
    // we will move the camera and the sliders too using ASDW
    switch (event->key()) {
    case Qt::Key::Key_A:
        if (mOrbitCamera) {
            mOrbitCamera->setPitchAngle(mOrbitCamera->pitchAngle() - Ogre::Degree(1.f));
        }
        break;
    case Qt::Key::Key_S:
        if (mOrbitCamera) {
            mOrbitCamera->setYawAngle(mOrbitCamera->yawAngle() + Ogre::Degree(1.f));
        }
        break;
    case Qt::Key::Key_D:
        if (mOrbitCamera) {
            mOrbitCamera->setPitchAngle(mOrbitCamera->pitchAngle() + Ogre::Degree(1.f));
        }
        break;
    case Qt::Key::Key_W:
        if (mOrbitCamera) {
            mOrbitCamera->setYawAngle(mOrbitCamera->yawAngle() - Ogre::Degree(1.f));
        }
        break;
    // zoom
    case Qt::Key::Key_Plus:
        if (mOrbitCamera) {
            const float currentZoom = mOrbitCamera->zoom();
            mOrbitCamera->setZoomDist(currentZoom - 5.f);
        }
        break;
    case Qt::Key::Key_Minus:
        if (mOrbitCamera) {
            const float currentZoom = mOrbitCamera->zoom();
            mOrbitCamera->setZoomDist(currentZoom + 5.f);
        }
        break;
    }
    event->accept();
}

////////////////////////////////////////////////////////////////////////////////
void
QtAssetManager::OgreWidgetKeyPressEvent(QKeyEvent* event)
{
    keyPressEvent(event);
}

void
QtAssetManager::OgreWidgetKeyReleaseEvent(QKeyEvent* event)
{
    event->ignore();
}
void
QtAssetManager::OgreWidgetMousePressEvent(QMouseEvent* event)
{
    mLastMousePoint = event->globalPos();
    event->ignore();
}
void
QtAssetManager::OgreWidgetMouseReleaseEvent(QMouseEvent* event)
{
    event->ignore();
}
void
QtAssetManager::OgreWidgetMouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::MouseButton::LeftButton)) {
        return;
    }
    // Note that this is not the right option, we need to implement this in the
    // OgreWidget since we only want to move the camera when moving the mouse
    // inside of that widget. To do that we can emmit an event with the mouse
    // information and catch that signal here. Instead of moving the camera
    // whenever we move the mouse in ALL the widget!.
    //

    const QPoint& globalPos = event->globalPos();
    // then move the camera depending where it was moved
    const float relX = mLastMousePoint.x() - globalPos.x();
    const float relY = mLastMousePoint.y() - globalPos.y();
    mLastMousePoint = globalPos;

    const float factor = -0.01 * 1.5f;
    mOrbitCamera->setPitchAngle(mOrbitCamera->pitchAngle() + Ogre::Degree(relX));
    mOrbitCamera->setYawAngle(mOrbitCamera->yawAngle() + Ogre::Degree(relY));
    event->accept();
}

void
QtAssetManager::OgreWidgetWheelEvent(QWheelEvent * event)
{
    const float currentZoom = mOrbitCamera->zoom();
    mOrbitCamera->setZoomDist(currentZoom + 0.5f * event->delta());
}

}
