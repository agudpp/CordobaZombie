/*
 * QtAssetManager.cpp
 *
 *  Created on: Jan 13, 2014
 *      Author: agustin
 */

#include "QtAssetManager.h"

#include <QFileDialog>
#include <QFileInfo>

#include <os_utils/OSHelper.h>
#include <asset/AssetFile.h>
#include <WorldObjectBuilder.h>

namespace tool {

////////////////////////////////////////////////////////////////////////////////
void
QtAssetManager::updateCurrentAsset(const core::Asset& newAsset)
{
    ASSERT(ogreWidget()->ogreData().sceneManager);
    Ogre::SceneManager* scnMngr = ogreWidget()->ogreData().sceneManager;

    // destroy the current world object if we have any one
    w_o::WorldObjectBuilder::destroyPlainWorldObject(scnMngr, mCurrentWO);

    // now we have to construct the new one from the new asset
    if (!w_o::WorldObjectBuilder::buildFromAsset(newAsset,
                                                 scnMngr,
                                                 mCurrentWO)) {
        QTDEBUG_WARNING("Error trying to construct the WorldObject from the asset" <<
                        newAsset.fullAssetPath << "\n");
        return;
    }

    // we have a new asset now
    mCurrentAsset = newAsset;

    // configure the panel we will use to handle the visual stuff.
    if (mCurrentWO.hasGraphicRepresentation()) {
        // we know that the entity is already visible
        ui.graphicCheckbox->setChecked(true);
        ui.graphicCheckbox->setEnabled(true);
    } else {
        ui.graphicCheckbox->setEnabled(false);
    }

    // configure now the coll2d checkbox
    if (mCurrentWO.hasColl2DRepresentation()) {
        ui.coll2DCheckbox->setEnabled(true);
        ui.coll2DCheckbox->setChecked(mCollHandler.debugDrawer() != 0);
    } else {
        ui.coll2DCheckbox->setEnabled(false);
    }

    // physics
    if (mCurrentWO.hasPhysicsRepresentation() && mBulletDbgDrawer) {
        ui.physicsCheckbox->setEnabled(true);
        ui.physicsCheckbox->setChecked(mBulletDbgDrawer->isEnabled());
    } else {
        ui.physicsCheckbox->setEnabled(false);
    }

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
QtAssetManager::QtAssetManager(rrh::ResourceHandler* rh,
                               const std::string& plugins,
                               const std::string& ogre,
                               const std::string& resource) :
    QtOgreAppBase(rh, plugins, ogre, resource)
,   mOrbitCamera(0)
,   mLastPathLoaded(".")
,   mConfigWindow(0)
,   mCollDbgDrawer(0)
,   mBulletDbgDrawer(0)
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

    // configure the ConfigAssetGUI
    mConfigWindow.setOgreData(ogreWidget()->ogreData());
}

////////////////////////////////////////////////////////////////////////////////
QtAssetManager::~QtAssetManager()
{
    delete mOrbitCamera;
    delete mCollDbgDrawer;
    delete mBulletDbgDrawer;
}

////////////////////////////////////////////////////////////////////////////////
void
QtAssetManager::frameUpdate(float fp)
{
    // update dynamic world
    mDynamicWorld.simulate(fp);

    // update coll2D world
    mCollHandler.update();
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

    // construct the Physics and collisions debug drawer
    Ogre::SceneManager* scnMngr = ogreWidget()->ogreData().sceneManager;

    mCollDbgDrawer = new coll::CollDebugDrawer(scnMngr);
    mBulletDbgDrawer = new physics::BulletDebugDrawer(scnMngr,
                                                      &(mDynamicWorld.bulletDynamicWorld()));
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

////////////////////////////////////////////////////////////////////////////////
void
QtAssetManager::onCreateAssetClicked(bool)
{
    // here we need to create an asset from an existing .mesh file
    // so we will open the FileDialog and let only .mesh files be visible.
    // We also need to check that we haven't an existing .assetInfo file in the
    // same directory.
    // try to load the .mesh file
    const QString fileName = QFileDialog::getOpenFileName(this,
                                                          tr("Create Asset from an Ogre Mesh"),
                                                          mLastPathLoaded,
                                                          tr("OgreMesh files (*.mesh)"));
    if (fileName.isEmpty()) {
        // do nothing
        return;
    }

    // check if we already have an .assetInfo file over there
    QFileInfo finfo(fileName);
    const QString path = finfo.path();
    QFileInfo assetInfo(path + finfo.baseName() + ".assetInfo");
    if (assetInfo.exists()) {
        QTDEBUG_WARNING("El directorio en donde se encuentra el archivo " <<
                        fileName.toStdString() << " ya contiene"
            " un .assetInfo, por lo que deberia cargarlo en vez de crearlo. Si quiere"
            " borrarlo hagalo manualmente y luego vuelva a intentar crear el asset.\n");
        return;
    }

    // now we can just create the asset
    core::Asset tmpAsset;

    // update the render window with the current asset
    updateCurrentAsset(tmpAsset);

    // configure the ConfigWindow
    mConfigWindow.setAssetToConfigure(&mCurrentAsset);
    mConfigWindow.show();
}

////////////////////////////////////////////////////////////////////////////////
void
QtAssetManager::onLoadAssetClicked(bool)
{
    // here we need to load the asset file directly and then show it. To do that
    // we will use the AssetFile class
    const QString fileName = QFileDialog::getOpenFileName(this,
                                                          tr("Create Asset from an Ogre Mesh"),
                                                          mLastPathLoaded,
                                                          tr("OgreMesh files (*.mesh)"));
    if (fileName.isEmpty()) {
        // do nothing
        return;
    }
    core::Asset tmpAsset;
    if (!core::AssetFile::assetFromFile(fileName.toStdString(), tmpAsset)) {
        QTDEBUG_WARNING("Error al intentar cargar el archivo asset " <<
                        fileName.toStdString() << "\n");
        return;
    }

    // update the render window with the current asset
    updateCurrentAsset(tmpAsset);

    // configure the ConfigWindow
    mConfigWindow.setAssetToConfigure(&mCurrentAsset);
    mConfigWindow.show();

}


}